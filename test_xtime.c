#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>


#define PROC_KALLSYMS "/proc/kallsyms"
#define DEV_KMEM "/dev/kmem"
#define DEV_MEM "/dev/mem"
#define PAGE_SZ (4096)

int
main(int argc, char argv[]) {
	FILE *fp;
	int fd;

	unsigned long kaddr, addr, addr_align;
	char sym_type;
	char sym_str[128];

	void *ptr;
	struct timespec *kernel_xtime;

	fp = fopen(PROC_KALLSYMS, "r");
	if (!fp) {
		printf("Error opening :%s\n", PROC_KALLSYMS);
		return 0;
	}
	
	while (EOF !=  fscanf(fp, "%lx %c %s", 
		&kaddr, &sym_type, sym_str)) {
		
		if (!strcmp(sym_str, "xtime")) {
			printf("%lx %s\n", kaddr, sym_str);
			break;
		}
	}
	fclose(fp);

	fd = open(DEV_KMEM, O_RDONLY);
	if (fd < 0) {
		printf("Error opening %s\n", DEV_KMEM);
		perror("open");
		fd = open(DEV_MEM, O_RDONLY);
		if (fd < 0 ) {
			printf("Error opening %s\n", DEV_MEM);
			perror("open");
			return 0;
		}
		else {
			addr = kaddr - 0xc0000000;
		}
			
	}
	else {
		addr = kaddr;
	}
	
	addr_align = addr & ~(PAGE_SZ-1);
	printf("mmap offset = %lx\n", addr_align);
	ptr = mmap(NULL, sizeof(struct timespec), 
				PROT_READ, MAP_SHARED, fd, addr_align);
	
	if (ptr == MAP_FAILED) {
		perror("mmap");
		return 0;
	}
	kernel_xtime = ptr + (addr - addr_align);
	while(1) {
		printf("%ld.%ld\n", kernel_xtime->tv_sec, 
				kernel_xtime->tv_nsec);
		sleep(1);
	}
	return 0;
}
		
