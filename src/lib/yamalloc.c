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
static pthread_mutex_t malloc_lock = PTHREAD_MUTEX_INITIALIZER;

void yafree(void *ptr)
{
	if (!ptr) {
		return;
	}

	pthread_mutex_lock(&malloc_lock);
	// (BlockHeader *)((uint8_t *)ptr - sizeof(BlockHeader))
	BlockHeader *block = (BlockHeader *)ptr - 1;
	block->is_free = 1;
	// TODO: coalesce free blocks
	pthread_mutex_unlock(&malloc_lock);
}

void *yamalloc(size_t size)
{
	pthread_mutex_lock(&malloc_lock);

	BlockHeader *block;
	// Adjust block size including the header
	size_t total_size = sizeof(BlockHeader) + size;
	if (!free_list) {
		// First call using sbrk
		block = (BlockHeader *)sbrk((intptr_t)total_size);
		if (block == (void *)-1) {
			pthread_mutex_unlock(&malloc_lock);
			return NULL;
		}
		block->size = size;
		block->is_free = 0;
		block->next = NULL;
		free_list = block;
		return (void *)(block + 1);
	}

    block = NULL;
	pthread_mutex_unlock(&malloc_lock);
	return (void *)(block + 1);
}

// void *yamalloc(size_t size)
// {
// 	pthread_mutex_lock(&malloc_lock);
//
// 	BlockHeader *block;
// 	// Adjust block size including the header
// 	size_t total_size = sizeof(BlockHeader) + size;
//     if (free_list) {
//         BlockHeader *last = free_list;
//         block = find_free_block(&last, total_size);
//         if (block) {
//             block->is_free = 0;
//             pthread_mutex_unlock(&malloc_lock);
//             return (void *)(block + 1);
//         }
//     } else {
//         // First call
//         block = (BlockHeader *)mmap(0, total_size, PROT_READ | PROT_WRITE,
//         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); if (block == MAP_FAILED) {
//             pthread_mutex_unlock(&malloc_lock);
//             return NULL;
//         }
//         block->size = size;
//         block->is_free = 0;
//         block->next = NULL;
//         free_list = block;
//         pthread_mutex_unlock(&malloc_lock);
//         return (void *)(block + 1);
//     }
//
// 	pthread_mutex_unlock(&malloc_lock);
// 	return NULL; // return (void *)(block + 1);
// }
