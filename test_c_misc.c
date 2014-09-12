#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
int main()
{
	struct kk
	{
		int a;
		char x;
	} *k;
	int l,m,n;
	unsigned int i = -1;
	k = malloc(2*4);
	int b = 20;
	k->a = 40;
	memcpy(&k->x, &b, 4);
	printf("%d\n", *((int *)&k->x));
	printf("sizeof()=%d\n", sizeof(long int));
	printf("\b\btesting backspace\n");
	printf("i=%d i>>2=%d\n", i,i>>8);
	uint64_t aa[2];
	printf("%s sz = %d %09.04lf %09.04lf\n", "\033[32m", sizeof(aa), 9.3, 0.0);

	void *xx, *yy;
	uint32_t zz;
	zz=222;
	yy=(void *)222;
	xx=yy+zz;
	printf("xx=%lu %u %u\n", xx, 4-6, 6-7);
	l = 10;
	n=l,m=l++;
	printf("n = %d m = %d l = %d\n", n, m, l);
	return 0;
}
