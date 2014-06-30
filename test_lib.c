#include "test_misc.h"
#include <stdio.h>
struct test_array test_dynamic = {
	6,
	{1, 2, 3, 4, 5, 6},
};

struct test_array test_dynamic2 __attribute__((section("__test_dynamic2"))) = {
	6,
	{2, 2, 4, 4, 6, 6},
} ;

void
print_array(struct test_array *a) {
	int i;
	for (i = 0; i<a->array_len; i++) {
		printf("%d\n", a->array[i]);
	}
}
