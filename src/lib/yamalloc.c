#include "yamalloc.h"
#ifdef YAMALLOC_FREE_LIST
#include "yamalloc_free_list.h"
#endif // YAMALLOC_FREE_LIST

void *yamalloc(size_t size)
{
#ifdef YAMALLOC_FREE_LIST
	return free_list_yamalloc(size);
#else
	(void)size;
	return NULL;
#endif // YAMALLOC_FREE_LIST
}

void *yacalloc(size_t num, size_t size)
{
#ifdef YAMALLOC_FREE_LIST
	return free_list_yacalloc(num, size);
#else
	(void)num;
	(void)size;
	return NULL;
#endif // YAMALLOC_FREE_LIST
}

void *yarealloc(void *ptr, size_t size)
{
#ifdef YAMALLOC_FREE_LIST
	return free_list_yarealloc(ptr, size);
#else
	(void)ptr;
	(void)size;
	return NULL;
#endif // YAMALLOC_FREE_LIST
}

void yafree(void *ptr)
{
#ifdef YAMALLOC_FREE_LIST
	free_list_yafree(ptr);
#else
	(void)ptr;
	return;
#endif // YAMALLOC_FREE_LIST
}
