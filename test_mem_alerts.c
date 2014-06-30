#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define NARGS (2)
int
main(int argc, char *argv[]) {

	char *mem;
	uint64_t alloc_bytes;
	int niters;
	
	if (argc < (NARGS+1)) {
		printf("test_mem_alerts <alloc_bytes> <niter> \n");
		return 0;
	}
	sscanf(argv[1], "%llu", &alloc_bytes);
	sscanf(argv[2], "%d", &niters);
	while (niters--) {
		mem = malloc(alloc_bytes);
		if (!mem) {
			perror("malloc failed");
			return -1;
		}
		printf("Allocation of %llu bytes successful\n", alloc_bytes);
		printf("Touching memory...");
		memset(mem, 0xab, alloc_bytes);
		printf("done.\n");
	}
	printf("All allocations done.\n");
	pause();
	return 0;
}
