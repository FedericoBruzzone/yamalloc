#ifndef YAMALLOC_FREE_LIST_LL_H
#define YAMALLOC_FREE_LIST_LL_H

#include <stddef.h>
#include <stdint.h>

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

extern void *free_list_ll_yamalloc(size_t size);
extern void *free_list_ll_yacalloc(size_t num, size_t size);
extern void *free_list_ll_yarealloc(void *ptr, size_t size);
extern void free_list_ll_yafree(void *ptr);

extern size_t get_padding_with_header(uintptr_t payload, size_t header_size);

#if defined(YAMALLOC_FREE_LIST_LL_FIND_FIRST)
extern FreeListLLNode *free_list_ll_find_first(FreeListLLNode **prev_node,
					       size_t size, size_t *padding);
#elif defined(YAMALLOC_FREE_LIST_LL_FIND_BEST)
extern FreeListLLNode *free_list_ll_find_best(FreeListLLNode **prev_node,
					      size_t size, size_t *padding);
#endif

extern void free_list_ll_coalesce(FreeListLLNode *prev,
				  FreeListLLNode *free_node);
extern void free_list_ll_insert_node(FreeListLLNode *prev,
				     FreeListLLNode *new_node);
extern void free_list_ll_remove_node(FreeListLLNode *prev,
				     FreeListLLNode *del_node);

#endif // YAMALLOC_FREE_LIST_LL_H
