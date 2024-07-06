#include "yamalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char *str = (char *)yamalloc(5);
	if (str == NULL) {
		printf("Failed to allocate memory\n");
		return 1;
	}
    strcpy(str, "Hello\0");
	printf("str: %s\n", str);
    printf("strlen: %lu\n", strlen(str));
    printf("size: %lu\n", sizeof(str));
    // yafree(str);

	char *str2 = (char *)yamalloc(6);
	if (str2 == NULL) {
		printf("Failed to allocate memory\n");
		return 1;
	}
    strcpy(str2, "World\0");
    printf("str2: %s\n", str2);
    printf("strlen: %lu\n", strlen(str2));
    printf("size: %lu\n", sizeof(str2));

	// printf("Hello, World!\n");
}
