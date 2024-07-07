#include "yamalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// redefine assert to set a boolean flag
#ifdef assert
#undef assert
#endif
#define assert(x) (rslt = rslt && (x))

// main result return code used by redefined assert
static int rslt;

// test suite main variables
static int done;
static int num_tests;
static int tests_passed;

void TestStart(char *name)
{
	num_tests++;
	rslt = 1;
	printf("-- Testing %s ... ", name);
}

void TestEnd()
{
	if (rslt)
		tests_passed++;
	printf("%s\n", rslt ? "success" : "fail");
}

// ===== TESTS =====
void test_yamalloc_1()
{
	TestStart("test_yamalloc_1");
	char *ptr = (char *)yamalloc(6);
	assert(ptr != NULL);
	strcpy(ptr, "Hello\0");
	assert(strcmp(ptr, "Hello") == 0);
	assert(strlen(ptr) == 5);
	yafree(ptr);
	TestEnd();
}

void test_yacalloc_1()
{
    TestStart("test_yacalloc_1");
    int *ptr = (int *)yacalloc(5, sizeof(int));
    assert(ptr != NULL);
    for (int i = 0; i < 5; ++i) {
        assert(ptr[i] == 0);
    }
    yafree(ptr);
    TestEnd();
}

void test_yarealloc_1()
{
    TestStart("test_yarealloc_1");
    int *ptr = (int *)yamalloc(5 * sizeof(int));
    for (int i = 0; i < 5; ++i) {
        ptr[i] = i;
    }
    int *ptr2 = (int *)yarealloc(ptr, 10 * sizeof(int));
    for (int i = 0; i < 5; ++i) {
        assert(ptr2[i] == i);
    }
    for (int i = 5; i < 10; ++i) {
        assert(ptr2[i] == 0);
    }
    yafree(ptr2);
    TestEnd();
}

void test_yamalloc_2()
{
    TestStart("test_yamalloc_2");
    char *ptr = (char *)yamalloc(12);
    assert(ptr != NULL);
    strcpy(ptr, "Hello World\0");
    assert(strcmp(ptr, "Hello World") == 0);
    assert(strlen(ptr) == 11);
    yafree(ptr);
    TestEnd();
}

void test_yacalloc_2()
{
    TestStart("test_yacalloc_2");
    int *ptr = (int *)yacalloc(10, sizeof(int));
    assert(ptr != NULL);
    for (int i = 0; i < 10; ++i) {
        assert(ptr[i] == 0);
    }
    yafree(ptr);
    TestEnd();
}

void test_yarealloc_2()
{
    TestStart("test_yarealloc_2");
    int *ptr = (int *)yamalloc(10 * sizeof(int));
    for (int i = 0; i < 10; ++i) {
        ptr[i] = i;
    }
    int *ptr2 = (int *)yarealloc(ptr, 20 * sizeof(int));
    for (int i = 0; i < 10; ++i) {
        assert(ptr2[i] == i);
    }
    for (int i = 10; i < 20; ++i) {
        assert(ptr2[i] == 0);
    }
    yafree(ptr2);
    TestEnd();
}

void test_yamalloc_3()
{
    TestStart("test_yamalloc_3");
    struct test_struct {
        int a;
        int b;
        int c;
    };
    struct test_struct *ptr = (struct test_struct *)yamalloc(sizeof(struct test_struct));
    assert(ptr != NULL);
    ptr->a = 1;
    ptr->b = 2;
    ptr->c = 3;
    assert(ptr->a == 1);
    assert(ptr->b == 2);
    assert(ptr->c == 3);
    yafree(ptr);
    TestEnd();
}

void test_yacalloc_3()
{
    TestStart("test_yacalloc_3");
    struct test_struct {
        int a;
        int b;
        int c;
    };
    struct test_struct *ptr = (struct test_struct *)yacalloc(1, sizeof(struct test_struct));
    assert(ptr != NULL);
    assert(ptr->a == 0);
    assert(ptr->b == 0);
    assert(ptr->c == 0);
    yafree(ptr);
    TestEnd();
}

void test_yarealloc_3()
{
    TestStart("test_yarealloc_3");
    struct test_struct {
        int a;
        int b;
        int c;
    };
    struct test_struct *ptr = (struct test_struct *)yamalloc(sizeof(struct test_struct));
    ptr->a = 1;
    ptr->b = 2;
    ptr->c = 3;
    struct test_struct *ptr2 = (struct test_struct *)yarealloc(ptr, 2 * sizeof(struct test_struct));
    assert(ptr2->a == 1);
    assert(ptr2->b == 2);
    assert(ptr2->c == 3);
    yafree(ptr2);
    TestEnd();
}

int main()
{
	num_tests = 0;
	tests_passed = 0;
	done = 0;
	test_yamalloc_1();
    test_yacalloc_1();
    test_yarealloc_1();

    test_yamalloc_2();
    test_yacalloc_2();
    test_yarealloc_2();

    test_yamalloc_3();
    test_yacalloc_3();
    test_yarealloc_3();

	printf("Total tests passed: %d\n", tests_passed);
	done = 1;
	return !(tests_passed == num_tests);
}
