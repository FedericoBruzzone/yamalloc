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

void *yamalloc(size_t size)
{
	pthread_mutex_lock(&malloc_lock);

	BlockHeader *block;

	// First call
	if (!free_list) {
		block = request_space(NULL, size);
		if (!block) {
			pthread_mutex_unlock(&malloc_lock);
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
				pthread_mutex_unlock(&malloc_lock);
				return NULL;
			}
		}
	}

	pthread_mutex_unlock(&malloc_lock);
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

	pthread_mutex_lock(&malloc_lock);
	// Consider using (BlockHeader *)((uint8_t *)ptr -
	// sizeof(BlockHeader))
	BlockHeader *block = (BlockHeader *)ptr - 1;
	block->is_free = 1;
	// TODO: coalesce free blocks
	pthread_mutex_unlock(&malloc_lock);
}
