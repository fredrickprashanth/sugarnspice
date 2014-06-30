#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>

void sig_term_h(int sno)
{
	printf("Quiting\n");
	exit(0);
}
int main(int argc, char *argv[])
{
	char page[4096];
	int fd, max_fd;
	int bytes_read;
	fd_set fds;


	signal(SIGTERM, sig_term_h);
	fd = open(argv[1], O_RDONLY);
	if(fd < 0)
	{
		perror("open:");
		return -1;
	}
	FD_SET(fd, &fds);

	max_fd = fd;
	while(1)
	{
		if(select(fd + 1, &fds, NULL, NULL, NULL)<0)
		{
			perror("select:");
		}
		
		if((bytes_read = read(fd, page, sizeof(page)))<0)
		{
			perror("read:");
		}
		if (bytes_read > 0)
		printf("%d bytes read\n", bytes_read);
	}		
	close(fd);
	perror("select:");
	return 0;
}
