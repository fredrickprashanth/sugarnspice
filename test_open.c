#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define NARGS 2

int
main(int argc, char *argv[]) {

	if (argc < (NARGS+1)) {
		printf("test_open <filename> <sleep>\n");
		return 0;
	}
	
	char *fname = argv[1];
	int sleep_secs;
	int fd;

	sscanf(argv[2], "%d", &sleep_secs);
	
	printf("my pid = %d sleeping for %d secs\n", getpid(), sleep_secs);
	sleep(sleep_secs);

	fd = open(fname, O_CREAT|O_RDWR, 0755);
	if (fd < 0) {
		perror("open failed");
		return -2;
	}

	if (write(fd, fname, strlen(fname)+1) < 0) {
		perror("write failed");
	}

	
	close(fd);

	return 0;
	
}
