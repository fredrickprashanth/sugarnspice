#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NCOLS 120
#define NROWS 25
void 
print_hist(int *data, int nr_data)
{
	char plot[NCOLS + 1][NROWS + 1];
	int i,j,k,nr_dots;
	
	for(i=0;i<NCOLS + 1;i++)
		memset(plot[i], ' ', sizeof(plot[i]));
	for(i=0;i<NROWS+1;i++)
		plot[0][i]='+';
	
	for(i=0; i<NCOLS+1;i++)
		plot[i][0]='+';
	
	for(k=0;k<nr_data;k++)
	{
		nr_dots = data[k]/(100/NROWS);
		for(i=1;i<nr_dots;i++)
			plot[k+1][i] = '*';
	}
	for(i=0;i<NROWS+1;i++)
	{
		for(j=0;j<NCOLS+1;j++)
			printf("%c", plot[j][NROWS-i]);
		printf("\n");
	}
}

int
main()
{
	int data[NCOLS - 2];
	int i;
	srandom(time(NULL));
	for(i=0;i<sizeof(data)/sizeof(int);i++)
		data[i] = random()%100;
	print_hist(data, sizeof(data)/sizeof(int));			
	return 0;
}
