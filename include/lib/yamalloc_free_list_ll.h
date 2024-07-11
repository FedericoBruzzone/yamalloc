#ifndef YAMALLOC_FREE_LIST_LL_H
#define YAMALLOC_FREE_LIST_LL_H

#include <stddef.h>
#include <stdint.h>

typedef enum FindPolicy { FindFirst, FindBest } FindPolicy;

typedef struct FreeListLLHeader {
	size_t size;
	// Padding store the amount of bytes that has to be placed before the
	// header to make the payload aligned
	size_t padding;
} FreeListLLHeader;

typedef struct FreeListLLNode {
	size_t size;
	struct FreeListLLNode *next;
} FreeListLLNode;

// static FreeListLLNode *free_list_ll = NULL;

extern void *free_list_ll_yamalloc(size_t size);
extern void *free_list_ll_yacalloc(size_t num, size_t size);
extern void *free_list_ll_yarealloc(void *ptr, size_t size);
extern void free_list_ll_yafree(void *ptr);

extern void free_list_ll_coalesce(FreeListLLNode *prev,
				  FreeListLLNode *free_node);
extern void free_list_ll_insert_node(FreeListLLNode *prev,
				     FreeListLLNode *new_node);
extern void free_list_ll_remove_node(FreeListLLNode *prev,
				     FreeListLLNode *del_node);

// Require space
extern FreeListLLNode *free_list_ll_request_space(FreeListLLNode *last,
						  size_t size);

// Find free block
extern FreeListLLNode *free_list_ll_find_free_block(FreeListLLNode **last,
						    size_t size);

#if defined(YAMALLOC_FREE_LIST_LL_FIND_FIRST)
// extern void free_list_ll_find_first(
#elif defined(YAMALLOC_FREE_LIST_LL_FIND_BEST)
// extern void free_list_ll_find_best(
#endif

#endif // YAMALLOC_FREE_LIST_LL_H
