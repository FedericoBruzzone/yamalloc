#ifndef YAMALLOC_H
#define YAMALLOC_H

#include <stddef.h>
#include <stdint.h>

typedef struct BlockHeader {
	size_t size;
	struct BlockHeader *next;
	uint8_t is_free;
} BlockHeader;

extern void *yamalloc(size_t size);
extern void *yacalloc(size_t size);
extern void *yarealloc(void *ptr, size_t size);
extern void yafree(void *ptr);
extern BlockHeader *request_space(BlockHeader *last, size_t size);
extern BlockHeader *find_free_block(BlockHeader **last, size_t size);
extern void coalesce_free_blocks(void);

#endif // YAMALLOC_H
