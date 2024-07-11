#include "yamalloc_free_list_ll.h"
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
// static FindPolicy policy;
// #if defined(YAMALLOC_FREE_LIST_LL_FIND_FIRST)
// policy = FindFirst;
// #elif defined(YAMALLOC_FREE_LIST_LL_FIND_BEST)
// policy = FindBest;
// #endif

#ifdef YAMALLOC_THREAD_SAFE
#include <pthread.h>
static pthread_mutex_t malloc_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t free_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

// static void align(size_t *size)
// {
// 	*size = (*size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
// }

// void free_list_ll_yafree(void *ptr)
// {
//    FreeListLLHeader *header;
// FreeListLLNode *free_node;
// FreeListLLNode *prev;
// FreeListLLNode *node;
//
//    if (!ptr)
//        return;
//
//    header = (FreeListLLHeader *)((char *)ptr - sizeof(FreeListLLHeader));
//    free_node = (FreeListLLNode *)header;
//    free_node->size = header->size + header->padding;
// }

void free_list_ll_coalesce(FreeListLLNode *prev, FreeListLLNode *free_node)
{
	if (free_node->next &&
	    (char *)free_node + free_node->size == (char *)free_node->next) {
		free_node->size += free_node->next->size;
		free_list_ll_remove_node(free_node, free_node->next);
	}
	if (prev->next && (char *)prev + prev->size == (char *)free_node) {
		prev->size += free_node->next->size;
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
