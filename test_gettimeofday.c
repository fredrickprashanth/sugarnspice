#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
int 
main() {
	struct timeval t;
	while (1) {
		gettimeofday(&t, NULL);
		printf("%ld.%ld\n", t.tv_sec, t.tv_usec);
		sleep(1);
	}
	return 0;
}
