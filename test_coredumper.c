#include<stdio.h>
#include "google/coredumper.h"
int
main(){

	WriteCoreDump("mycoredump.core");
	return 0;	
}
