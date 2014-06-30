#include <stdio.h>

int
gen_mask(int m, int n) {

	int mask = 0, mask1, mask2;
	if (m < 31)
		mask1 = ((1<<(m+1)) -1);
	else
	 	mask1 = 0;
	mask2 =  (~((1<<n)-1));
	printf("%x mask1 = %x mask2 = %x\n", 1<<(m+1), mask1, mask2);
	mask = mask1 & mask2;
	return mask;
}
int 
main() {
	printf("mask(8,4) = %x\n", gen_mask(8,4));
	printf("mask(31,4) = %x\n", gen_mask(31,4));
	return 0;
}
