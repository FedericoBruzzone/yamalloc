#include "yamalloc_red_black.h"

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

#define ALIGNMENT 8

static BlockHeaderRedBlack *root = NULL;

#ifdef YAMALLOC_THREAD_SAFE
#include <pthread.h>
static pthread_mutex_t malloc_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t free_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

static void align(size_t *size)
{
	*size = (*size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

void *red_black_yamalloc(size_t size)
{

#ifdef YAMALLOC_THREAD_SAFE
	pthread_mutex_lock(&malloc_lock);
#endif

	align(&size);

	BlockHeaderRedBlack *block;
	// First call
	if (!root) {
		block = red_black_request_space(NULL, size);
		if (!block) {
#ifdef YAMALLOC_THREAD_SAFE
			pthread_mutex_unlock(&malloc_lock);
#endif
			return NULL;
		}
		root = block;
	} else {
		block = red_black_find_free_block(&root, size);
		if (block) {
			block->is_free = 0;
		} else {
			block = red_black_request_space(root, size);
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

void *red_black_yacalloc(size_t num, size_t size)
{
	(void)num;
	(void)size;
	return NULL;
}
void *red_black_yarealloc(void *ptr, size_t size)
{
	(void)ptr;
	(void)size;
	return NULL;
}
void red_black_yafree(void *ptr) { (void)ptr; }
BlockHeaderRedBlack *red_black_request_space(BlockHeaderRedBlack *last,
					     size_t size)
{
	(void)last;
	(void)size;

	return NULL;
}
BlockHeaderRedBlack *red_black_find_free_block(BlockHeaderRedBlack **last,
					       size_t size)
{
	(void)last;
	(void)size;

	return NULL;
}
void red_black_coalesce_free_blocks(void) {}
