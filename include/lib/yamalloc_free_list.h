#ifndef YAMALLOC_FREE_LIST_H
#define YAMALLOC_FREE_LIST_H

#include <stddef.h>
#include <stdint.h>

typedef struct BlockHeaderFreeList {
	size_t size;
	struct BlockHeaderFreeList *next;
	uint8_t is_free;
} BlockHeaderFreeList;

extern void *free_list_yamalloc(size_t size);
extern void *free_list_yacalloc(size_t num, size_t size);
extern void *free_list_yarealloc(void *ptr, size_t size);
extern void free_list_yafree(void *ptr);
extern BlockHeaderFreeList *free_list_request_space(BlockHeaderFreeList *last,
						    size_t size);
extern BlockHeaderFreeList *
free_list_find_free_block(BlockHeaderFreeList **last, size_t size);
extern void free_list_coalesce_free_blocks(void);

#endif // YAMALLOC_FREE_LIST_H
