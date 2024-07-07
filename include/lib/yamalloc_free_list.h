#ifndef YAMALLOC_FREE_LIST_H
#define YAMALLOC_FREE_LIST_H

#include <stddef.h>
#include <stdint.h>

typedef struct BlockHeaderFreeList {
	size_t size;
	struct BlockHeaderFreeList *next;
	uint8_t is_free;
} BlockHeaderFreeList;

extern void *yamalloc_free_list(size_t size);
extern void *yacalloc_free_list(size_t num, size_t size);
extern void *yarealloc_free_list(void *ptr, size_t size);
extern void yafree_free_list(void *ptr);
extern BlockHeaderFreeList *request_space_free_list(BlockHeaderFreeList *last,
						    size_t size);
extern BlockHeaderFreeList *
find_free_block_free_list(BlockHeaderFreeList **last, size_t size);
extern void coalesce_free_blocks_free_list(void);

#endif // YAMALLOC_FREE_LIST_H
