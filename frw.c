#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define PAGE_SZ 4096

void
timeit(char *msg)
{
	char *t_msg = "timeit:";
        static struct timespec ts1, ts2, *ts;
        static int first_in = 0;
        first_in = !first_in;
        if(first_in)
                ts = &ts1;
        else
                ts = &ts2;
        clock_gettime(CLOCK_MONOTONIC, ts);
        if(!first_in)
        {
		if(!msg) msg=t_msg;
                printf("%d:%s: time delta msecs=%ld\n", getpid(), msg, (ts2.tv_sec - ts1.tv_sec)*1000 + (ts2.tv_nsec - ts1.tv_nsec)/1000000);
        }
}

void
read_fd(int fd, int max_pages, int rand_flag)
{
	char page_buf[PAGE_SZ];
	int i;	
	int offset = random();
	lseek(fd, offset, random()%2?SEEK_SET:SEEK_END);
	for(i=0; i<max_pages; i++)
	{
		if(rand_flag)
		{
			if(pread(fd, page_buf, sizeof(page_buf), (random()%max_pages)*PAGE_SZ)<0)
			{
				perror("pread:");
			}
		}
		else
		{
			if(read(fd, page_buf, sizeof(page_buf))<0)
			{
				perror("read:");
			}
		}
	}
}
void
write_fd(int fd, int max_pages, int rand_flag)
{
	char page_buf[PAGE_SZ];
	int i;	
	memset(page_buf, random()%255, sizeof(page_buf));
	lseek(fd, 0, SEEK_SET);
	for(i=0; i<max_pages; i++)
	{
		if(rand_flag)
		{
			if(pwrite(fd, page_buf, sizeof(page_buf), (random()%max_pages)*PAGE_SZ)<0);
			{
				perror("pwrite:");
			}
		}
		else
		{
			if(write(fd, page_buf, sizeof(page_buf))<0)
			{
				perror("write:");
			}
		}
	}
}

int slam_fd;
void
sigterm_hdlr(int signo)
{
	printf("Quitting\n");
	close(slam_fd);
	_exit(0);
}
int 
main(int argc, char *argv[])
{
	char **argp = ++argv;
	char *fname;
	char *op;
	int max_pages;
	int rest_secs;
	int iteration = 0;

#define NARGS 4
	if(argc < NARGS + 1)
	{
		printf("frw <fname> <op=r/w> <max_pages> <rest_secs>\n");
		printf("op = r - read , w - random, s - fsync write\n");
		return -2;
	}  
	fname = *argp++;
	op = *argp++;
	sscanf(*argp++, "%d", &max_pages);
	sscanf(*argp++, "%d", &rest_secs);
	printf("fname = %s op=%s  max_pages = %d rest_secs = %d\n", fname, op, max_pages, rest_secs);
	
	if(op[0] == 'r')
	{
		slam_fd = open(fname, O_RDWR);
	}
	else
	{
		slam_fd = open(fname, O_WRONLY| O_TRUNC);
		if(slam_fd < 0 )
			slam_fd = creat(fname, 0666);
		
	}
	if(slam_fd < 0)
	{
		perror("open:");
		return -2;
	}

	signal(SIGINT, sigterm_hdlr);
	signal(SIGTERM, sigterm_hdlr);

	while(1)
	{
		timeit(NULL);
		if(op[0] == 'r')
		{
			read_fd(slam_fd, max_pages, op[1]=='r');
			timeit("read");
		}
		else
		{
			write_fd(slam_fd, max_pages, op[1]=='r');
			if(op[0] == 's')
			{
				fsync(slam_fd);
				timeit("write fsync");
			}
			else
			{
				timeit("write");
			}
		}
		printf("%d:iter=%d\n", getpid(), iteration++);
		
		sleep(rest_secs);
	}
	
	return 0;
	 
}

