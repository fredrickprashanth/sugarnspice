#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>

int main()
{
	char *str = "vim (29297, #threads: 1)";
	char *str1="        124544";
	char *dev_str = "../../devices/pci0000:00/0000:00:1d.7/usb1/1-2/";
	char comm[100];
	int pid;
	int ad;
	int no_threads;
	char str2[20];
	int i = 20;
	union{
		int a[4];
		struct{
		int _a[3];
		int last_a;
		};
	} u_a;
	
	u_a.last_a = 10;
	printf("last a check %d=%d\n", u_a.last_a, u_a.a[3]);
	sscanf(str, "%s (%d, #threads: %d)", comm, &pid, &no_threads);
	printf("comm=%s pid=%d no_threads=%d\n", comm, pid, no_threads);
	sscanf(str1, "%*[ ]%s", str2);
	printf("str2=%s|\n", str2);
	ad=1;

	char *p;
	char *block_str = "/block/sda";
	char blk_str[100];
	strcpy(blk_str, block_str);

  if (
            (p = strtok(blk_str+1, "/")) &&
            !strncmp(p, "block", 5) &&
            (p = strtok(NULL, "/")) &&
            !strncmp(p, "sd", 2)
        )
		printf("got it\n");

	__sync_fetch_and_add(&ad, 1);
	printf("ad=%d\n""sdfsdf\n",ad);
	printf("ran=%ld",random());

	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	printf("before select\n");
	//select(1, NULL, NULL, NULL, &tv);
	printf("after select\n");
	printf("%ld\n", 0xffffff);

	printf("iuuuuuuuu=%u\n", -1284048327);
	system("./sa2.o");
	printf("I am still here \n");
	printf("1 2 3\n");

	int8_t iii = -1;
	uint32_t jjj = iii;
	printf("%u %u %02x\n", iii, jjj, 1);

	if(fork())
	{
		pause();
		
	}
	else
	{
		exit(20);
	}
	
	return 0;
	
	
}
