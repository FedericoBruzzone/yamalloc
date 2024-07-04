#include "yamalloc.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *str = (int *)yamalloc(5);
    if (str == NULL) {
        printf("Failed to allocate memory\n");
        return 1;
    }
    char *tmp = (char *)str;
	tmp[0] = 'H';
	tmp[1] = 'e';
	tmp[2] = 'l';
	tmp[3] = 'l';
	tmp[4] = 'o';
	printf("%s\n", tmp);

	// yafree(str);

    // Using malloc

	printf("Hello, World!\n");
}
