#ifndef YAMALLOC_FREE_LIST_LL_H
#define YAMALLOC_FREE_LIST_LL_H

#include <stddef.h>
#include <stdint.h>

typedef struct BlockHeaderFreeListLL {
	size_t size;
	struct BlockHeaderFreeListLL *next;
	uint8_t is_free;
} BlockHeaderFreeListLL;

extern void *free_list_ll_yamalloc(size_t size);
extern void *free_list_ll_yacalloc(size_t num, size_t size);
extern void *free_list_ll_yarealloc(void *ptr, size_t size);
extern void free_list_ll_yafree(void *ptr);
extern BlockHeaderFreeListLL *free_list_ll_request_space(BlockHeaderFreeListLL *last,
						    size_t size);
extern BlockHeaderFreeListLL *
free_list_ll_find_free_block(BlockHeaderFreeListLL **last, size_t size);
extern void free_list_ll_coalesce_free_blocks(void);

#endif // YAMALLOC_FREE_LIST_LL_H
