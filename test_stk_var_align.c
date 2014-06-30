#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int
main(int argc, char *argv[]) {

	uint8_t a;
	int b;
	uint8_t c;
	
	printf("Address of a = %x\n", &a);
	printf("Address of b = %x\n", &b);
	printf("Address of c = %x\n", &c);

	return 0;

}
