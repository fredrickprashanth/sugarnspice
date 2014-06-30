#include <stdint.h>
#include <stdio.h>

#define myhtonl(x) \
	({ \
		typeof(x) __y; \
		uint32_t __z; \
		(void)(&__z == &__y); \
		x; \
	})

int main() {
	
	int x = 0;
	char y = 1;
	long z = 2;
	unsigned int aa = 2;
	
	uint32_t res = 1;
	uint8_t xx = 1;

	xx = res;

	z = y;
	y = z;
	res = myhtonl(x);
	res = myhtonl(y);
	res = myhtonl(z);
	res = myhtonl(aa);
	res = myhtonl(res);

	return 0;
}
