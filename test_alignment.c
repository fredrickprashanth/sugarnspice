#include <stdio.h>
int
main() {
	int a;
	char *ch = (char *)&a;
	ch+=3;
	while (1) {
		*ch = *ch + 1;
	}
	return 0;
}

