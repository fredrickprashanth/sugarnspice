#include <stdio.h>

#define THRESH 0.000001
void
my_sqrt(float x) {

	float x1,x2,y,diff;
	if (x < 0)
		return;

	if (x >= 1) {
		x1 = 1; x2=x;
	}
	else {
		x1 = x; x2 = 1; 
	}
	y = x1;
	while (1) {
		diff = y*y -x;
		if (diff > THRESH)
			x2=y;
		else
		if ((-1*diff) > THRESH)
			x1=y;
		else
		if ((x2-x1) <= THRESH)
			break;
		else 
			break;
		
		y=(x1+x2)/2;
		printf("x1=%f x2=%f\n", x1, x2);
	}
	
	printf("%f %f %f\n", x1, y, x2);
	
}

#define NARGS 1
int 
main(int argc, char *argv[]){

	float x;
	if (argc < (NARGS+1)) {
		printf("test_mysqrt <num>\n");
		return 0;
	}
	sscanf(argv[1], "%f", &x);
	my_sqrt(x);
	return 0;
}
