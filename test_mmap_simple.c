
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SZ 4096
#define MAX_FILE_SZ PAGE_SZ*10 //20 pages
int
main()
{
	int fd, bytes;
	int i;
	char fname[128];
	void *fp_ptr1, *fp_ptr2, *fp_ptr3;

	sprintf(fname, "/tmp/mmap_test.%d", getpid());
	fd = open(fname, O_CREAT| O_RDWR, 0660);
	if(fd < 0)
	{
		perror("open");
		return 1;
	}
	printf("file desc = %d\n", fd);
	ftruncate(fd, MAX_FILE_SZ/2);

	/*
	bytes = write(fd, fname, strlen(fname)+1);
	if(bytes < 0)
	{
		perror("write");
	}
	printf("bytes wrote=%d\n", bytes);
	*/

	fp_ptr1 = mmap(NULL, MAX_FILE_SZ, PROT_READ|PROT_WRITE, 
				MAP_SHARED, fd, 0);
	if(fp_ptr1 == MAP_FAILED)
	{
		perror("mmap");
		return 2;
	}
	close(fd);
	memset(fp_ptr1, 0xaa, MAX_FILE_SZ);
	munmap(fp_ptr1, MAX_FILE_SZ);
	//close(fd);
}
