#include "yamalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char *str = (char *)yamalloc(14 * sizeof(char));
	if (str == NULL) {
		return 1;
	}
	strcpy(str, "Hello, World!\0");
	printf("%s\n", str);
	yafree(str);
	return 0;
}
