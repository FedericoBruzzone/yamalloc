#ifndef YAMALLOC_H
#define YAMALLOC_H

#include <stdint.h>
#include <stdio.h>

typedef struct BlockHeader {
	size_t size;
	struct BlockHeader *next;
	uint8_t is_free;
} BlockHeader;

extern void *yamalloc(size_t size);
extern void yafree(void *ptr);

#endif // YAMALLOC_H
