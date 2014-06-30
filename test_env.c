#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
	
	struct stat stbuf;
	if (argc < 2)
		return -1;
	setenv("XYZ","11",1);
	if (stat(argv[1], &stbuf) < 0)
	{
		perror("stat");
	}
	else
	{
		printf("stat successful on %s\n", argv[1]);
	}
	printf("env XYZ = %s\n", getenv("XYZ"));
	pause();
	return 0;

}

