#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

int
pci_config_read_dword(int fd, int reg, uint32_t *val) 
{
	int ret;
	lseek(fd, reg<<2, SEEK_SET);
	ret = read(fd, val, sizeof(*val));
	return ret;

}

int
pci_config_write_dword(int fd, int reg, 
		uint32_t val) 
{
	int ret;
	lseek(fd, reg<<2, SEEK_SET);
	ret = write(fd, &val, sizeof(val));
	return ret;
}

int 
main(int argc, char *argv[]) {
	int reg;
	uint32_t val;
	char read_write;
	int fd;

	if (argc < 5)
	{
		printf("pci_config_rw <r/w> <config file> <reg> <val>\n");
		return 0;
	}
	read_write = argv[1][0];
	fd = open(argv[2], O_RDWR);
	if (fd < 0) {
		perror("open failed\n");
		return 1;
	}
	sscanf(argv[3], "%d", &reg);

	if ( pci_config_read_dword(fd, reg, &val) < 0) {
		perror("config read failed:");
		return -1;
	}

	printf("Current REG%d = %x\n", reg, val);

	if (read_write == 'w')	{
		sscanf(argv[4], "%x", &val);
		if ( pci_config_write_dword(fd, reg, val) < 0) {
			perror("config write failed:");
			return -2;
		}
	}

	if ( pci_config_read_dword(fd, reg, &val) < 0) {
		perror("config read failed\n");
		return -1;
	}

	printf("Current REG%d = %x\n", reg, val);

	return 0;
	
}
