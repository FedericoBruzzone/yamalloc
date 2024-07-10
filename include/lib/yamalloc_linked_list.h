#ifndef YAMALLOC_LINKED_LIST_H
#define YAMALLOC_LINKED_LIST_H

#include <stddef.h>
#include <stdint.h>

typedef struct BlockHeaderLinkedList {
	size_t size;
	struct BlockHeaderLinkedList *next;
	uint8_t is_free;
} BlockHeaderLinkedList;

extern void *linked_list_yamalloc(size_t size);
extern void *linked_list_yacalloc(size_t num, size_t size);
extern void *linked_list_yarealloc(void *ptr, size_t size);
extern void linked_list_yafree(void *ptr);
extern BlockHeaderLinkedList *
linked_list_request_space(BlockHeaderLinkedList *last, size_t size);
extern BlockHeaderLinkedList *
linked_list_find_free_block(BlockHeaderLinkedList **last, size_t size);
extern void linked_list_coalesce_free_blocks(void);

#endif // YAMALLOC_LINKED_LIST_H
