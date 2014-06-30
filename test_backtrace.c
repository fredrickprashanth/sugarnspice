#include <stdio.h>
#include <execinfo.h>

void
print_backtrace(void) {
	int stack_trace[100];
	int ret;
	if ((ret = backtrace((void **)&stack_trace, 
			     sizeof(stack_trace))) <= 0){
		printf("backtrace failed\n");
	}
	backtrace_symbols_fd((void *)stack_trace, ret, 1);
}
int
test_backtrace(int level) {
	if (--level == 0)
		print_backtrace();
	else
		test_backtrace(level);
	return 0;
}
		
int main() {
	test_backtrace(8);
}
