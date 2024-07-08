#ifndef YAMALLOC_RED_BLACK_H
#define YAMALLOC_RED_BLACK_H

#include <stddef.h>
#include <stdint.h>

typedef enum Color { RED = 0, BLACK = 1 } Color;
typedef struct BlockHeaderRedBlack {
	size_t size;
	uint8_t is_free;
	struct BlockHeaderRedBlack *left;
	struct BlockHeaderRedBlack *right;
	struct BlockHeaderRedBlack *parent;
	Color color;
} BlockHeaderRedBlack;

extern void *red_black_yamalloc(size_t size);
extern void *red_black_yacalloc(size_t num, size_t size);
extern void *red_black_yarealloc(void *ptr, size_t size);
extern void red_black_yafree(void *ptr);
extern BlockHeaderRedBlack *red_black_request_space(BlockHeaderRedBlack *last,
						    size_t size);
extern BlockHeaderRedBlack *
red_black_find_free_block(BlockHeaderRedBlack **last, size_t size);
extern void red_black_coalesce_free_blocks(void);

#endif // YAMALLOC_RED_BLACK_H
