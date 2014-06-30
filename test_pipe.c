#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define SPIN_MAX 9999999
#define PAGE_SZ (4096)
#define BUF_SZ (20*PAGE_SZ)
#define PARENT_DATA ('a')
#define CHILD_DATA ('b')
#define DO_IDLE_SPIN 0
#define VERIFY_DATA 0


static void inline
do_pipe_read_write(int fd, char *buf, int size, int read_write) {

	int bytes = 0 ;
	
	while (1) {
		if (read_write == 0)
			bytes += read(fd, buf, size);
		else
			bytes += write(fd, buf, size);
		if (bytes < 0) {
			perror("child pipe read error");
			return;
		}
		if (bytes < size){
			printf("PID:%d Partial %s %d\n", getpid(), 
					read_write?"write":"read", bytes);
			size -= bytes;
		}
		else {
			bytes = 0;
			break;
		}
	}
}
static void inline 
do_pipe_read(int fd, char *buf, int size) {
	//printf("PID:%d calling read\n", getpid());
	do_pipe_read_write(fd, buf, size, 0);
	//printf("PID:%d done read\n", getpid());
}
static void inline
do_pipe_write(int fd, char *buf, int size) {
	
	//printf("PID:%d calling write\n", getpid());
	do_pipe_read_write(fd, buf, size, 1);
	//printf("PID:%d done write\n", getpid());
}
#if DO_IDLE_SPIN
static void inline 
do_idle_spin(void) {
	//int spin_count = SPIN_MAX;
	//while(--spin_count > 0);
}
#else
static void inline do_idle_spin(void) { }
#endif
#if VERIFY_DATA
static int inline
verify_pipe_data(char *buf, int size, char c) {
	int i;
	for (i = 0; i<size&& 
			buf[i] == c; i++);
	if (i < size) {
		printf("PID:%d: Corrupt bytes in data %d\n",
				getpid(), size - i);
		return -1;
	}
	return 0;
}
#else
static int inline verify_pipe_data(char *buf, int size, char c) { }
#endif 

static inline alloc_buf(char **buf, int size) {
	*buf = mmap(NULL, size, 
		PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
	if (*buf == MAP_FAILED) {
		perror("mmap failed");
		exit(EXIT_FAILURE);
	}
}
static inline free_buf(char *buf, int size) {
	munmap(buf, size);
}

static void inline
do_pipe_actions(int fd_read, int fd_write, int read_write) {
	char *buf;

	alloc_buf(&buf, BUF_SZ);
	if (read_write) {
		do_pipe_read(fd_read, buf, BUF_SZ);
		verify_pipe_data(buf, BUF_SZ, PARENT_DATA);
	}
	else {
		memset(buf, PARENT_DATA, BUF_SZ);
		do_pipe_write(fd_write, buf, BUF_SZ);
	}
	free_buf(buf, BUF_SZ);

	alloc_buf(&buf, BUF_SZ);
	if (read_write) {
		memset(buf, CHILD_DATA, BUF_SZ);
		do_pipe_write(fd_write, buf, BUF_SZ);
	}
	else {
		do_pipe_read(fd_read, buf, BUF_SZ);
		verify_pipe_data(buf, BUF_SZ, CHILD_DATA);
	}
	free_buf(buf, BUF_SZ);

	do_idle_spin();
}


#define PARENT_PIPE_ACTION 0
#define CHILD_PIPE_ACTION 1
int
main(int argc, char *argv[])
{
	int pfd_parent[2], pfd_child[2];
	int fd_read, fd_write;
	pid_t cpid;
	char buf[BUF_SZ];
	int i;
	int bytes_corrupt;
    
	if (pipe(pfd_parent) == -1) { 
		perror("pipe"); 
		exit(EXIT_FAILURE); 
	}
	if (pipe(pfd_child) == -1) { 
		perror("pipe"); 
		exit(EXIT_FAILURE); 
	}

	if (0 == fork()) { 
		/* parent */   
		sleep(1);
		close(pfd_parent[0]);
		close(pfd_child[1]);
		fd_read = pfd_child[0];
		fd_write = pfd_parent[1];
		while (1) {
			do_pipe_actions(fd_read, fd_write, 
					PARENT_PIPE_ACTION);
		}
	} else {            
		/* child */
		close(pfd_parent[1]);
		close(pfd_child[0]);
		fd_read = pfd_parent[0];
		fd_write = pfd_child[1];
		while(1) {
			do_pipe_actions(fd_read, fd_write, 
					CHILD_PIPE_ACTION);
		}
		wait(NULL);           
		exit(EXIT_SUCCESS);
    	}

	close(pfd_parent[0]);
	close(pfd_parent[1]);
	close(pfd_child[0]);
	close(pfd_child[1]);
}
