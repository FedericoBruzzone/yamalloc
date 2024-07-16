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
	// yafree(str);
    printf("%s\n", str);
	// char *str1 = (char *)yamalloc(10 * sizeof(char));
 //    if (str1 == NULL) {
 //        return 1;
 //    }
 //    strcpy(str1, "Foo, Bar!\0");
 //    printf("%s\n", str1);
	return 0;
}
