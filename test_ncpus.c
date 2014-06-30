#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int
main() {
	printf("Nr cpus = %d\n", sysconf (_SC_NPROCESSORS_ONLN));
	return 0;
}
