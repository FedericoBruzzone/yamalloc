#include "yamalloc_free_list_ll.h"
#include <stdio.h>
#include <string.h>

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

static FreeListLLNode *free_list_ll = NULL;

#ifdef YAMALLOC_THREAD_SAFE
#include <pthread.h>
static pthread_mutex_t malloc_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t free_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

void *free_list_ll_request_space(size_t size)
{
	FreeListLLNode *node;
	size_t total_size =
	    size + sizeof(FreeListLLNode) + sizeof(FreeListLLHeader);
	node = sbrk((intptr_t)total_size);
	if (node == (void *)-1) {
		return NULL;
	}
	node = (FreeListLLNode *)((char *)node + sizeof(FreeListLLHeader));

	node->size = size;
	node->next = NULL;

	return node;
}

void *free_list_ll_yamalloc(size_t size)
{
#if defined(YAMALLOC_THREAD_SAFE)
	pthread_mutex_lock(&malloc_lock);
#endif
	size_t padding = 0;
	FreeListLLNode *prev = NULL;
	FreeListLLNode *node = NULL;
	FreeListLLHeader *header = NULL;
	size_t alignment_padding;
	size_t required_space;
	size_t remaining_space;

	if (size < sizeof(FreeListLLNode)) {
		size = sizeof(FreeListLLNode);
	}

    // Always enter in this scope cause free_list_ll is always NULL
	if (!free_list_ll) {
		node = free_list_ll_request_space(size);
		if (!node) {
			return NULL;
		}
	} else {
#if defined(YAMALLOC_FREE_LIST_LL_FIND_FIRST)
		node = free_list_ll_find_first(&prev, size, &padding);
#elif defined(YAMALLOC_FREE_LIST_LL_FIND_BEST)
		node = free_list_ll_find_best(&prev, size, &padding);
#endif
		if (!node) {
			node = free_list_ll_request_space(size);
			if (!node) {
				return NULL;
			}
		}
	}

	alignment_padding = padding - sizeof(FreeListLLHeader);
	required_space = size + padding;
	remaining_space = node->size - required_space;

	if (remaining_space > 0) {
		FreeListLLNode *new_node =
		    (FreeListLLNode *)((char *)node + required_space);
		new_node->size = remaining_space;
		free_list_ll_insert_node(prev, new_node);
	}

	free_list_ll_remove_node(prev, node);

	header = (FreeListLLHeader *)((char *)node + alignment_padding);
	header->size = size;
	header->padding = alignment_padding;

#if defined(YAMALLOC_THREAD_SAFE)
	pthread_mutex_unlock(&malloc_lock);
#endif

	return (void *)((char *)header + sizeof(FreeListLLHeader));
}

void *free_list_ll_yacalloc(size_t num, size_t size)
{
	size_t total_size = num * size;
	void *ptr = free_list_ll_yamalloc(total_size);
	if (ptr) {
		uint8_t *p = (uint8_t *)ptr;
		for (size_t i = 0; i < total_size; i++) {
			p[i] = 0;
		}
	}
	return ptr;
}

void *free_list_ll_yarealloc(void *ptr, size_t size)
{
	if (!ptr) {
		return free_list_ll_yamalloc(size);
	}

	FreeListLLHeader *header =
	    (FreeListLLHeader *)((char *)ptr - sizeof(FreeListLLHeader));
	void *new_ptr = free_list_ll_yamalloc(size);
	if (new_ptr) {
		uint8_t *p = (uint8_t *)new_ptr;
		for (size_t i = 0; i < header->size; i++) {
			p[i] = ((uint8_t *)ptr)[i];
		}
		free_list_ll_yafree(ptr);
	}

	return new_ptr;
}

void free_list_ll_yafree(void *ptr)
{
#if defined(YAMALLOC_THREAD_SAFE)
	pthread_mutex_lock(&free_lock);
#endif
	FreeListLLHeader *header;
	FreeListLLNode *free_node;
	FreeListLLNode *prev = NULL;
	FreeListLLNode *node = free_list_ll;

	if (!ptr)
		return;

	header = (FreeListLLHeader *)((char *)ptr - sizeof(FreeListLLHeader));
	free_node = (FreeListLLNode *)header;
	free_node->size = header->size + header->padding;
	free_node->next = NULL;

	while (node != NULL) {
		if (ptr < (void *)node) {
			free_list_ll_insert_node(prev, free_node);
			break;
		}
		prev = node;
		node = node->next;
	}

	free_list_ll_coalesce(prev, free_node);
#if defined(YAMALLOC_THREAD_SAFE)
	pthread_mutex_unlock(&free_lock);
#endif
}

size_t get_padding_with_header(uintptr_t payload, size_t header_size)
{
	uintptr_t p = payload;
	uintptr_t modulo = p & (ALIGNMENT - 1); // p % ALIGNMENT
	uintptr_t needed_space = (uintptr_t)header_size;
	uintptr_t padding = 0;

	if (modulo != 0) {
		padding = ALIGNMENT - modulo;
	}

	if (padding < needed_space) {
		needed_space -= padding;

		if ((needed_space & (ALIGNMENT - 1)) != 0) {
			padding += ALIGNMENT * (1 + (needed_space / ALIGNMENT));
		} else {
			padding += ALIGNMENT * (needed_space / ALIGNMENT);
		}
	}

	return (size_t)padding;
}

FreeListLLNode *free_list_ll_find_first(FreeListLLNode **prev_node, size_t size,
					size_t *padding)
{
	FreeListLLNode *node = free_list_ll;
	FreeListLLNode *prev = NULL;
	size_t padding_size = 0;

	while (node != NULL) {
		padding_size = get_padding_with_header(
		    (uintptr_t)node, sizeof(FreeListLLHeader));
		size_t required_size = size + padding_size;
		if (node->size >= required_size) {
			break;
		}
		prev = node;
		node = node->next;
	}

	if (prev_node)
		*prev_node = prev;
	if (padding)
		*padding = padding_size;
	return node;
}

FreeListLLNode *free_list_ll_find_best(FreeListLLNode **prev_node, size_t size,
				       size_t *padding)
{
	size_t smallest_size = ~(size_t)0;
	FreeListLLNode *node = free_list_ll;
	FreeListLLNode *prev = NULL;
	FreeListLLNode *best = NULL;
	size_t padding_size = 0;

	while (node != NULL) {
		padding_size = get_padding_with_header(
		    (uintptr_t)node, sizeof(FreeListLLHeader));
		size_t required_size = size + padding_size;
		if (node->size >= required_size &&
		    (node->size - required_size) < smallest_size) {
			best = node;
		}
		prev = node;
		node = node->next;
	}

	if (prev_node)
		*prev_node = prev;
	if (padding)
		*padding = padding_size;
	return best;
}

void free_list_ll_coalesce(FreeListLLNode *prev, FreeListLLNode *free_node)
{
	if (free_node->next != NULL &&
	    (void *)((char *)free_node + free_node->size) ==
		(void *)free_node->next) {
		free_node->size += free_node->next->size;
		free_list_ll_remove_node(free_node, free_node->next);
	}

	if (prev != NULL &&
	    (void *)((char *)prev + prev->size) == (void *)free_node) {
		prev->size += free_node->size;
		free_list_ll_remove_node(prev, free_node);
	}
}

void free_list_ll_insert_node(FreeListLLNode *prev, FreeListLLNode *new_node)
{
	if (!prev) {
		if (free_list_ll) {
			new_node->next = free_list_ll;
		} else {
			free_list_ll = new_node;
		}
	} else {
		if (!prev->next) {
			prev->next = new_node;
			new_node->next = NULL;
		} else {
			new_node->next = prev->next;
			prev->next = new_node;
		}
	}
}

void free_list_ll_remove_node(FreeListLLNode *prev, FreeListLLNode *del_node)
{
	if (!prev) {
		free_list_ll = del_node->next;
	} else {
		prev->next = del_node->next;
	}
}
