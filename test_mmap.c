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
	bytes = write(fd, fname, strlen(fname)+1);
	if(bytes < 0)
	{
		perror("write");
	}
	printf("bytes wrote=%d\n", bytes);
	ftruncate(fd, MAX_FILE_SZ);
	fp_ptr1 = mmap(NULL, MAX_FILE_SZ/2 + 20, PROT_READ|PROT_WRITE, 
				MAP_SHARED,fd, 0);
	if(fp_ptr1 == MAP_FAILED)
	{
		perror("mmap");
		return 2;
	}
	sprintf(fp_ptr1+ 20, "%s\n", "Writing SOME JUnk1");
	//close(fd);
	
	bzero(fp_ptr1, MAX_FILE_SZ/2);	
	fp_ptr2 = mmap(NULL, MAX_FILE_SZ/2, PROT_READ|PROT_WRITE,
				MAP_SHARED, fd, 0);
	if((unsigned long)fp_ptr2 < 0)
	{
		perror("mmap");
		munmap(fp_ptr1, MAX_FILE_SZ/2);
		return 3;
	}

	sprintf(fp_ptr2, "%s\n", "Writing SOME JUnk2");
	
	if((unsigned long)fp_ptr1 < 0)
	{
		perror("mmap");
		return 2;
	}
	fp_ptr3 = mmap(NULL, MAX_FILE_SZ, PROT_READ|PROT_WRITE,
				MAP_SHARED, fd, 0);
	if((unsigned long)fp_ptr3 < 0)
	{
		perror("mmap");
		munmap(fp_ptr1, MAX_FILE_SZ/2);
		munmap(fp_ptr2, MAX_FILE_SZ/2);
		return 3;
	}

	printf("fp_ptr1 %p fp_ptr2 %p fp_ptr3 %p\n", fp_ptr1, 
		fp_ptr2, fp_ptr3);

	sprintf(fp_ptr3 + MAX_FILE_SZ/4, "%s\n", "Writing SOME Junk33");


	munmap(fp_ptr2, MAX_FILE_SZ/2);
	fp_ptr2 = mmap(NULL, MAX_FILE_SZ/2, PROT_READ|PROT_WRITE, 
				MAP_SHARED,fd, 0);
	sprintf(fp_ptr2, "%s\n", "Writing SOME Junk3");

	
	msync(fp_ptr1, MAX_FILE_SZ, MS_SYNC);
	msync(fp_ptr2, MAX_FILE_SZ/2, MS_SYNC);
	msync(fp_ptr3, MAX_FILE_SZ/2, MS_SYNC);

	sbrk(10*PAGE_SZ);
	//printf("Pausing before mumapping PID %d\n", getpid());	
	//pause();


	//munmap(fp_ptr1, MAX_FILE_SZ);
	munmap(fp_ptr2, MAX_FILE_SZ/2);
	//close(fd);


	return 0;
}
	
