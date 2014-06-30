#include <stdio.h>
#include <string.h>
#include <time.h>

struct test_a {
	int x;
	int y;
};

struct test_s {
	struct test_a ta;
	int b;
};

struct test_a test_aa = {
	.x = 1,
	.y = 2,
};

#if 0
struct test_s ss = {
	.ta = test_aa,
	.b = 1,
};
#endif
	
char strs[][32] = {
"xyx",
"rrter",
"ewer",
};

int test_fn(int a[16]) {
	printf("a = %p sizeof = %d\n", a, sizeof(a));
}


#define LS_CMD "/bin/ls "
#define LS_CMD_DIR "/tmp"
call_sleep_inf() { while(1) { sleep(1);}}
main() { 
	int a;
	time_t ts;
	int aa[16];

	printf("aa = %p\n", aa);
	test_fn(aa);
	//call_sleep_inf(); 
        printf("Hello C |%-10d|%20s|\n", 2, "g"); 
	//printf("%=*d\n", 20, 0);
	printf(" size of strs = %d\n", sizeof(strs)/sizeof(strs[0]));
	//a= *((int*)0);

	
	time(&ts);
	for (a = 0; a<5; a++) {
		printf("time elapsed = %d\n", time(NULL) - ts);
		sleep(1);
	}
	system(LS_CMD LS_CMD_DIR);
}
