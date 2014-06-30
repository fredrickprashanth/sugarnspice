#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#define PAGE_SZ 4096
#define MAX_FILE_SZ PAGE_SZ*2000
void
show_file(char *str)
{
	int fd = open(str, O_RDONLY);
	char buf[128];
	while(read(fd,buf,sizeof(buf))>0)
		printf("%s", buf);
	close(fd);
}
int 
main(int argc, char *argv[])
{
	int fd;
	int ret;
	int file_sz = 0;
	char *dirname;
	char buf[PAGE_SZ], filename[128];
	char fd_cmd[128];
	char du_cmd[128];

	if (argc < 2)
	{
		printf("filename please\n");
		return -1;
	}

	dirname = argv[1];
	sprintf(filename, "%s/test_del", dirname);

	fd = open(filename, O_WRONLY| O_CREAT, 0777);
	if (fd < 0)
	{
		perror("open");
		return -1;
	}

	memset(buf, 0x11, sizeof(buf));
	sprintf(fd_cmd, "ls -l /proc/%d/fd", getpid());
	sprintf(du_cmd, "du -sh %s", dirname);

	printf("MY pid is %d\n", getpid());

	while (file_sz < MAX_FILE_SZ/2)
	{
		ret = write(fd, buf, sizeof(buf));
		printf("Wrote %d bytes\n", ret);
		file_sz += ret;
		
	}
	printf("File_sz is %d\n", file_sz);

	system(fd_cmd);
	system(du_cmd);
	getchar();

	printf("Removing file %s\n", filename);
	ret = remove(filename);
	if (ret < 0)
	{
		perror("remove");
	}

	system(fd_cmd);
	system(du_cmd);

	while (file_sz < MAX_FILE_SZ)
	{
		ret = write(fd, buf, sizeof(buf));
		printf("Wrote %d bytes\n", ret);
		file_sz += ret;
		
	}

	printf("File_sz is %d\n", file_sz);

	system(fd_cmd);
	system(du_cmd);
	getchar();

	printf("Removing again\n");
	ret = remove(filename);
	if (ret < 0)
	{
		perror("remove");
	}

	printf("Sync-ing\n");
	ret = fsync(fd);
	printf("Sync ret = %d\n", ret);

	system(fd_cmd);	
	system(du_cmd);
	getchar();

	close(fd);
	printf("Closed file\n");

	system(fd_cmd);	
	system(du_cmd);
	getchar();

	return 0;
}

