#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int
main()
{
	char hd[] = "/dev/sda1";
	int fd;
	struct stat statbuf;
	fd = open(hd, O_RDONLY);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}
	fstat(fd, &statbuf);
	printf("size = %ld\n", statbuf.st_size);
	close(fd);	
	return 0;
}
