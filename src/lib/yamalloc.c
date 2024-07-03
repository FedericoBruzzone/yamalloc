#include "yamalloc.h"
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

void test() { printf("Hello, World from yamalloc!\n"); }
