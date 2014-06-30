#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
my_hash(char *str){
	
	int i;
	int hash = 0;
	char c;
	for (i=0; str[i] != NULL; i++) {
		c = str[i];
		hash = (hash + (c << 4) + (c >> 4)) * 11;
	}
	return hash;
}

int
main(int argc, char *argv[]) {

	if (argc < 2) {
		printf("test_hash <str>\n");
		return 0;
	}
	printf("hash %s %d\n", argv[1], my_hash(argv[1])&(63));
	return 0;
}
