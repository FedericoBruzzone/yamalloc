#ifndef YAMALLOC_H
#define YAMALLOC_H

#include <stddef.h>
#include <stdint.h>

extern void *yamalloc(size_t size);
extern void *yacalloc(size_t num, size_t size);
extern void *yarealloc(void *ptr, size_t size);
extern void yafree(void *ptr);

#endif // YAMALLOC_H
