#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PAGE_SZ 4096
do_recurse(){
	char pg[PAGE_SZ];
	memset(pg, 0xba, sizeof(pg));
	do_recurse();
}
int main() {
	char pg[PAGE_SZ];
	do_recurse();
}
