#ifndef YAMALLOC_H
#define YAMALLOC_H

#include <stdio.h>

// The CHUNK_SIZE is set to 4096 bytes because it is the size of a page in most
// systems. Using `mmap` to allocate memory is more efficient when the size is a
// multiple of the page size.
#define CHUNK_SIZE 4096

typedef struct BlockHeader {
	size_t size;
	struct BlockHeader *next;
	int free;
} BlockHeader;

void test();

#endif // YAMALLOC_H
