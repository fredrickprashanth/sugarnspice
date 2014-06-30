#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
	int fd;
	if (argc < 3)
	{
		printf("test_termios <tty file> <cmd>\n");
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		perror("open");
		return -1;
	}
	
	char *cmd = argv[2];
	if (!strcmp(cmd, "flush"))
	{
		if (tcflush(fd, TCIOFLUSH) < 0)
		{
			perror("tcflush");
		}
	}
	else
	if (!strcmp(cmd, "insert"))
	{

		if (ioctl(fd, TIOCSTI, "\r") < 0)
		{
			perror("ioctl");
		}
	}
	else
	if (!strcmp(cmd, "restart"))
	{
		if (tcflush(fd, TCOON) < 0)
		{
			perror("tcflush");
		}
	}
	else
	if (!strcmp(cmd, "suspend"))
	{
		if (tcflush(fd, TCOOFF) < 0)
		{
			perror("tcflush");
		}
	}
	else
	if (!strcmp(cmd, "stop"))
	{
		if (tcflow(fd, TCIOFF) < 0)
		{
			perror("tcflow");
		}
	}
	else
	if (!strcmp(cmd, "stop"))
	{
		if (tcflow(fd, TCION) < 0)
		{
			perror("tcflow");
		}
	}
	else
	{
		printf("Invalid cmd\n");
	}
	close(fd);
	return 0;

}
