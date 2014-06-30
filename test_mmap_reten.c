
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
main() {

	void *fp_ptr3;
	int i;
	
	fp_ptr3 = mmap(NULL, MAX_FILE_SZ, PROT_READ|PROT_WRITE,
				MAP_SHARED|MAP_ANON, -1, 0);
	if (fp_ptr3 == MAP_FAILED) {
		printf("mmap anon fp_ptr3 failed\n");
		perror("mmap");
		return 0;
	}
	printf("anon fp_ptr3 = %p\n", fp_ptr3);
	memset(fp_ptr3, 0xaa, MAX_FILE_SZ);

	munmap(fp_ptr3, MAX_FILE_SZ);
	fp_ptr3 = mmap(NULL, MAX_FILE_SZ, PROT_READ|PROT_WRITE,
				MAP_SHARED|MAP_ANON, -1, 0);
	if (fp_ptr3 == MAP_FAILED) {
		printf("mmap anon fp_ptr3 failed\n");
		perror("mmap");
		return 0;
	}
	printf("after unmap anon fp_ptr3 = %p\n", fp_ptr3);
	((char *)fp_ptr3)[0] = 0xcc;
	for (i = 0; i<100; i+=4)
		printf("[%x]", ((int *)fp_ptr3)[i]);
	return 0;
}

