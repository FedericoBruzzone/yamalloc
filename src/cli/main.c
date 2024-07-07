#include "yamalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char *str = (char *)yamalloc(6);
	if (str == NULL) {
		printf("Failed to allocate memory\n");
		return 1;
	}
    strcpy(str, "Hello\0");
	printf("str: %s\n", str);
	printf("strlen: %lu\n", strlen(str));

    int *test_malloc = (int *)yamalloc(5 * sizeof(int));
    for (int i = 0; i < 5; ++i) {
        test_malloc[i] = i;
    }
    int *test_realloc = (int *)yarealloc(test_malloc, 10 * sizeof(int));
    for (int i = 5; i < 10; ++i) {
        test_realloc[i] = i;
    }
    for (int i = 0; i < 10; ++i) {
        printf("test_realloc[%d]: %d\n", i, test_realloc[i]);
    }

	int *test = (int *)yacalloc(5, sizeof(int));
	for (int i = 0; i < 5; ++i) {
		printf("test[%d]: %d\n", i, test[i]);
	}

    int *test2 = (int *)yarealloc(test, 10 * sizeof(int));
    for (int i = 0; i < 10; ++i) {
        printf("test2[%d]: %d\n", i, test2[i]);
    }
}
