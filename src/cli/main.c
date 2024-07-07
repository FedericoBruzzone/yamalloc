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
}
