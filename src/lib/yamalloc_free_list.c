#include "yamalloc_free_list.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
// #include <sys/mman.h>
#define __USE_XOPEN_EXTENDED
#include <unistd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
// Simulate sbrk using VirtualAlloc
void *sbrk(intptr_t increment)
{
	static void *heap_end = NULL;
	void *prev_heap_end;

	if (heap_end == NULL) {
		heap_end = VirtualAlloc(
		    NULL, CHUNK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!heap_end)
			return (void *)-1;
	}

	prev_heap_end = heap_end;

	if (VirtualAlloc(heap_end, increment, MEM_COMMIT, PAGE_READWRITE) ==
	    NULL) {
		return (void *)-1;
	}

	heap_end = (char *)heap_end + increment;
	return prev_heap_end;
}
#elif defined(__linux__) || defined(__APPLE__)
#define sbrk sbrk
#endif

static BlockHeaderFreeList *free_list = NULL;

#ifdef YAMALLOC_THREAD_SAFE
#include <pthread.h>
static pthread_mutex_t malloc_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t free_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

/**
 * @brief Allocates a block of memory of the given size
 *
 * This function allocates a block of memory of the given size. The block is
 * allocated from the heap and is not initialized.
 *
 * @param[in] size Size (in bytes) of the block to allocate
 * @return void* Pointer to the allocated block of memory
 *
 * @note Remember to free the allocated block using yafree_list()
 * @warning Check the return value for NULL to ensure that the allocation was
 * successful
 */
void *yamalloc_free_list(size_t size)
{
#ifdef YAMALLOC_THREAD_SAFE
	pthread_mutex_lock(&malloc_lock);
#endif

	BlockHeaderFreeList *block;

	// First call
	if (!free_list) {
		block = request_space_free_list(NULL, size);
		if (!block) {
#ifdef YAMALLOC_THREAD_SAFE
			pthread_mutex_unlock(&malloc_lock);
#endif
			return NULL;
		}
		free_list = block;
	} else {
		BlockHeaderFreeList *last = free_list;
		block = find_free_block_free_list(&last, size);
		if (block) {
			block->is_free = 0;
		} else {
			block = request_space_free_list(last, size);
			if (!block) {
#ifdef YAMALLOC_THREAD_SAFE
				pthread_mutex_unlock(&malloc_lock);
#endif
				return NULL;
			}
		}
	}
#ifdef YAMALLOC_THREAD_SAFE
	pthread_mutex_unlock(&malloc_lock);
#endif
	return (void *)(block + 1);
}

/**
 * @brief Allocates a block of memory of the given size and initializes it to
 * zero
 *
 * This function allocates a block of memory of the given size and initializes
 * it to zero. The block is allocated from the heap.
 *
 * @param[in] num Number of elements to allocate
 * @param[in] size Size (in bytes) of the block to allocate
 * @return void* Pointer to the allocated block of memory
 *
 * @note Remember to free the allocated block using yafree_free_list()
 * @warning Check the return value for NULL to ensure that the allocation was
 * successful
 */
void *yacalloc_free_list(size_t num, size_t size)
{
	size_t total_size = num * size;
	void *ptr = yamalloc_free_list(total_size);
	if (ptr) {
		uint8_t *p = (uint8_t *)ptr;
		for (size_t i = 0; i < total_size; i++) {
			p[i] = 0;
		}
	}
	return ptr;
}

/**
 * @brief Reallocates a block of memory to the given size
 *
 * This function reallocates a block of memory to the given size. The contents
 * of the block are preserved up to the minimum of the old and new sizes. If the
 * new size is larger, the contents of the newly allocated portion of the block
 * are undefined.
 *
 * @param[in] ptr Pointer to the block of memory to reallocate
 * @param[in] size New size (in bytes) of the block
 * @return void* Pointer to the reallocated block of memory
 *
 * @note Remember to free the reallocated block using yafree_free_list()
 * @warning Check the return value for NULL to ensure that the reallocation was
 * successful
 */
void *yarealloc_free_list(void *ptr, size_t size)
{
	if (!ptr) {
		return yamalloc_free_list(size);
	}
	BlockHeaderFreeList *block = (BlockHeaderFreeList *)ptr - 1;
	if (block->size >= size) {
		return ptr;
	}
	void *new_ptr = yamalloc_free_list(size);
	if (new_ptr) {
		uint8_t *p = (uint8_t *)new_ptr;
		for (size_t i = 0; i < block->size; i++) {
			p[i] = ((uint8_t *)ptr)[i];
		}
		yafree_free_list(ptr);
	}
	return new_ptr;
}

/**
 * @brief Frees a block of memory
 *
 * This function frees a block of memory that was previously allocated using
 * yamalloc(). The block is returned to the heap and can be reused.
 *
 * @param[in] ptr Pointer to the block of memory to free
 * @return void
 */
void yafree_free_list(void *ptr)
{
	if (!ptr) {
		return;
	}
#ifdef YAMALLOC_THREAD_SAFE
	pthread_mutex_lock(&free_lock);
#endif
	// (BlockHeaderFreeList *)((uint8_t *)ptr - sizeof(BlockHeaderFreeList))
	BlockHeaderFreeList *block = (BlockHeaderFreeList *)ptr - 1;
	block->is_free = 1;
	coalesce_free_blocks_free_list();
#ifdef YAMALLOC_THREAD_SAFE
	pthread_mutex_unlock(&free_lock);
#endif
}

/**
 * @brief Requests space to the kernel
 *
 * This function requests space to the kernel to allocate a block of memory of
 * the given size. The block is allocated from the heap and is not initialized.
 *
 * @param[in] last Pointer to the last block in the free list
 * @param[in] size Size (in bytes) of the block to allocate
 * @return BlockHeaderFreeList* Pointer to the allocated block of memory
 */
BlockHeaderFreeList *request_space_free_list(BlockHeaderFreeList *last,
					     size_t size)
{
	BlockHeaderFreeList *block;
	size_t total_size = sizeof(BlockHeaderFreeList) + size;
#if defined(__linux__) || defined(__APPLE__)
	block = (BlockHeaderFreeList *)sbrk((intptr_t)total_size);
#elif defined(_WIN32) || defined(_WIN64)
	block = NULL;
#endif
	if (block == (void *)-1) {
		return NULL;
	}
	block->size = size;
	block->is_free = 0;
	block->next = NULL;
	if (last) {
		last->next = block;
	}
	return block;
}

/**
 * @brief Finds a free block of memory
 *
 * This function finds a free block of memory of the given size in the free
 * list.
 *
 * @param[in] last Pointer to the last block in the free list
 * @param[in] size Size (in bytes) of the block to allocate
 * @return BlockHeaderFreeList* Pointer to the free block of memory
 */
BlockHeaderFreeList *find_free_block_free_list(BlockHeaderFreeList **last,
					       size_t size)
{
	BlockHeaderFreeList *current = free_list;
	while (current && (current->size >= size && current->is_free)) {
		current = current->next;
	}
	if (current) {
		*last = current;
	}
	return current;
}

/**
 * @brief Coalesces free blocks of memory
 *
 * This function coalesces free blocks of memory in the free list.
 * If two consecutive blocks are free, they are merged into a single block.
 * This helps to reduce fragmentation in the heap.
 *
 * @return void
 */
void coalesce_free_blocks_free_list(void)
{
	BlockHeaderFreeList *current = free_list;
	while (current) {
		if (current->is_free && current->next &&
		    current->next->is_free) {
			current->size +=
			    current->next->size + sizeof(BlockHeaderFreeList);
			current->next = current->next->next;
		}
		current = current->next;
	}
}