
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdint.h>

int break_loop;
void
sigusr1_hdlr(int num, siginfo_t *info, 
		void *unused) {
	fprintf(stderr, "SIGUSR1 got called\n");
	fprintf(stderr, "si_signo = %d si_errno = %d si_code= %d\n",
			info->si_signo, info->si_errno, info->si_code);
	//if (info->si_addr) {
		fprintf(stderr, "Got an addr %p\n", info->si_addr);
	//}
	break_loop = 1;

}


int 
main(int argc, char *argv[]) {

	unsigned long addr;
	int size;
	uint32_t *mmap_ptr;
	int fd, i;
	struct sigaction act;
	uint32_t pci_dword;
	

	if (argc < 3) {
		fprintf(stderr, "test_iorw <addr> <nr_dwords>\n");
		return 0;
	}

	sscanf(argv[1], "%lx", &addr);
	sscanf(argv[2], "%d", &size);
	
	fd = open("/dev/mem", O_RDONLY);
	if (fd < 0) {
		perror("error opening /dev/mem");
		return 0;
	}


	memset(&act, 0, sizeof(act));
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = &sigusr1_hdlr;
	if (sigaction(SIGUSR1, &act, NULL) < 0) {
		perror("sigaction failed");
		close(fd);
		return 0;
	}

	fprintf(stderr, "mapping %lx size %d\n", addr, size);

	mmap_ptr = mmap(NULL, size*sizeof(*mmap_ptr), PROT_READ, MAP_SHARED,
			fd, addr);
	if (mmap_ptr == MAP_FAILED) {
		perror("mmap failed");
		close(fd);
		return 0;
	}
	for (i = 0; i<size; i++) {
		pci_dword = mmap_ptr[i];
		if (write(1, &pci_dword, sizeof(pci_dword)) < 0) {
			perror("write failed");
			return 0;
		}
		if (break_loop) {
			fprintf(stderr, "breaking loop\n");
			break;
		}
	}

	return 0;
}
