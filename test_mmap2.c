#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

#define PAGE_SZ 4096
#define MAX_FILE_SZ PAGE_SZ*40
void
show_file(char *str)
{
	int fd = open(str, O_RDONLY);
	char buf[128];
	printf("=================\n");
	while(read(fd,buf,sizeof(buf))>0)
		printf("%s", buf);
	close(fd);
	printf("\n=================\n");
}

int 
do_inf_mmap(void *data)
{
	int fd = (int)data;
	int i; 
#define MAX_MMAP_PER_THREAD (1<<10)
	for (i=0; i<MAX_MMAP_PER_THREAD; i++) {
		mmap(NULL, MAX_FILE_SZ/2, PROT_READ|PROT_WRITE,
			MAP_PRIVATE, fd, MAX_FILE_SZ/2);
	}
	return 0;
}

#define MAX_MMAP_THREADS (1<<10)
int
main()
{
	int fd, bytes, i ;
	char fname[128];
	void *fp_ptr1, *fp_ptr2, *fp_ptr3;

	void *child_stack[MAX_MMAP_THREADS];
	int child_pids[MAX_MMAP_THREADS];
	int child_status;

	sprintf(fname, "/tmp/mmap_test", getpid());
	fd = open(fname, O_CREAT| O_RDONLY, 0660);
	if(fd < 0)
	{
		perror("open");
		return 1;
	}
	fp_ptr1 = mmap(NULL, MAX_FILE_SZ/2 + 20, PROT_READ, 
				MAP_PRIVATE, fd, 0);
	if(fp_ptr1 == MAP_FAILED)
	{
		perror("mmap");
		return 2;
	}
	fp_ptr2 = mmap(NULL, MAX_FILE_SZ/2, PROT_READ, 
				MAP_PRIVATE,fd, 0);
	if(fp_ptr2 == MAP_FAILED)
	{
		perror("mmap");
		return 2;
	}
	fp_ptr3 = mmap(NULL, MAX_FILE_SZ/2, PROT_READ, 
				MAP_SHARED,fd, 0);
	if(fp_ptr3 == MAP_FAILED)
	{
		perror("mmap");
		return 2;
	}

	/* TEST 0 */
	show_file("/proc/self/maps");
	printf("p1 %p p2 %p p3 %p\n", fp_ptr1, fp_ptr2, fp_ptr3); 


	/* TEST 1 */
	printf("unmap p1\n");
	munmap(fp_ptr1, MAX_FILE_SZ/2);
	printf("unmap p2\n");
	munmap(fp_ptr2, MAX_FILE_SZ/2);
	printf("map p1 to %p\n", fp_ptr3+MAX_FILE_SZ/2);
	fp_ptr1 = mmap(fp_ptr3+MAX_FILE_SZ/2, MAX_FILE_SZ/2, PROT_READ,
				MAP_PRIVATE|MAP_FIXED, fd, MAX_FILE_SZ/2);
	if(fp_ptr1 == MAP_FAILED)
	{
		perror("mmap");
		return 2;
	}
	show_file("/proc/self/maps");
	printf("p1 %p p2 %p p3 %p\n", fp_ptr1, fp_ptr2, fp_ptr3); 
	

	/* TEST 2 */
	//munmap(fp_ptr1, MAX_FILE_SZ/2);
	printf("map p2 to %p\n", fp_ptr1);
	fp_ptr2 = mmap(fp_ptr1, MAX_FILE_SZ/2, PROT_READ|PROT_WRITE,
				MAP_PRIVATE|MAP_FIXED, fd, MAX_FILE_SZ/4);
	if(fp_ptr2 == MAP_FAILED)
	{
		perror("mmap");
		return 2;
	}
	show_file("/proc/self/maps");
	printf("p1 %p p2 %p p3 %p\n", fp_ptr1, fp_ptr2, fp_ptr3); 

	/* TEST 3 */
	for (i =0; i<MAX_MMAP_THREADS; i++) {

		child_stack[i] = malloc(PAGE_SZ*4);
		if (!child_stack[i]) {
			perror("mmap");
			return -2;
		}
		child_pids[i] = clone(do_inf_mmap, child_stack[i] + PAGE_SZ*4-1, 
			CLONE_VM|CLONE_FILES|CLONE_FS, (void *)fd);
	}
	for (i =0; i<MAX_MMAP_THREADS; i++) {
		waitpid(child_pids[i], &child_status, 0);
		free(child_stack[i]);
	}

	
	//printf("pid %d\n", getpid());
	show_file("/proc/self/maps");
	printf("p1 %p p2 %p p3 %p\n", fp_ptr1, fp_ptr2, fp_ptr3); 
	printf("\n");
	//pause();
	return 0;
}

