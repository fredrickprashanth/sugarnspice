#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int 
main() {
	int ret;
	struct timeval ts;
	ts.tv_sec = 5;
	ts.tv_usec = 0;
	while ((ret = select(0, NULL, NULL, NULL, &ts)) >= 0) {
		printf("test ret = %d\n", ret);
		ts.tv_sec = 5;
		ts.tv_usec = 0;
	}
	printf("select errored with ret = %d\n", ret);	
	return 0;
}
