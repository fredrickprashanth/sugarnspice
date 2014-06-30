#define _GNU_SOURCE
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "utils.h"

void dump_random_chars(char *str, int sz)
{
	int i;
	char c;
	for(i=0;i<sz;i++)
	{
		c = random()%256;
		if(c<65)c=65;
		str[i] = c;
	} 
}
int main(int argc, char *argv[])
{
	
	int fd;
	char page[4096];
	fd_set fds;
	if(argc < 2)
	{
		printf("pipe_writer.o <pipe_path>\n");
		return -1;
	}

	if(mknod(argv[1], S_IFIFO|0666, 0)<0)
	{
		if(errno == EEXIST)
		{
			remove(argv[1]);
			unlink(argv[1]);
			if(mknod(argv[1], S_IFIFO|0666, 0)<0)
			{
				perror("mknod:");
				return -1;
			}
		}
		else
		{
			perror("mknod:");
			return -1;
		}
	}
	
	register_sigterm();
	
	fd = open(argv[1], O_WRONLY);
	if(fd < 0)
	{
		perror("open:");
	}
	FD_SET(fd, &fds);
	while(1)
	{
		if(select(fd + 1, NULL, &fds, NULL, NULL)<0)
		{
			perror("select:");
			break;
		} 
		dump_random_chars(page, sizeof(page));	
		write(fd, page, sizeof(page));
		sleep(2);
	}

	close(fd);
	remove(argv[1]);
	return 0;
}
	
		
