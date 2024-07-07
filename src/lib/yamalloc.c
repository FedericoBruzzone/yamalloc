/**
 * @file yamalloc.c
 * @brief Yet Another Malloc implementation
 */

#include "yamalloc.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
// #include <sys/mman.h>
#define __USE_XOPEN_EXTENDED
#include <unistd.h>
#endif

static BlockHeader *free_list = NULL;

#ifdef YAMALLOC_THREAD_SAFE
static pthread_mutex_t malloc_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

/**
 * @brief Allocates a block of memory of the given size
 *
 * This function allocates a block of memory of the given size. The block is
 * allocated from the heap and is not initialized.
 *
 * @note Remember to free the allocated block using yafree()
 *
 * @param[in] size Size (in bytes) of the block to allocate
 * @return void* Pointer to the allocated block of memory
 */
void *yamalloc(size_t size)
{
#ifdef YAMALLOC_THREAD_SAFE
	pthread_mutex_lock(&malloc_lock);
#endif

	BlockHeader *block;

	// First call
	if (!free_list) {
		block = request_space(NULL, size);
		if (!block) {
#ifdef YAMALLOC_THREAD_SAFE
			pthread_mutex_unlock(&malloc_lock);
#endif
			return NULL;
		}
		free_list = block;
	} else {
		BlockHeader *last = free_list;
		block = find_free_block(&last, size);
		if (block) {
			block->is_free = 0;
		} else {
			block = request_space(last, size);
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

BlockHeader *request_space(BlockHeader *last, size_t size)
{
	BlockHeader *block;
	size_t total_size = sizeof(BlockHeader) + size;
	block = (BlockHeader *)sbrk((intptr_t)total_size);
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

BlockHeader *find_free_block(BlockHeader **last, size_t size)
{
	BlockHeader *current = free_list;
	while (current && (current->size >= size && current->is_free)) {
		current = current->next;
	}
	if (current) {
		*last = current;
	}
	return current;
}

void yafree(void *ptr)
{
	if (!ptr) {
		return;
	}
#ifdef YAMALLOC_THREAD_SAFE
	pthread_mutex_lock(&malloc_lock);
#endif
	// Consider using (BlockHeader *)((uint8_t *)ptr -
	// sizeof(BlockHeader))
	BlockHeader *block = (BlockHeader *)ptr - 1;
	block->is_free = 1;
	// TODO: coalesce free blocks
#ifdef YAMALLOC_THREAD_SAFE
	pthread_mutex_unlock(&malloc_lock);
#endif
}
