#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void
gen_brackets(char *bracket_str, int i, int nr_left, int nr_right) {
	
	if (nr_left) {
		bracket_str[i] = '(';
		gen_brackets(bracket_str, i+1, nr_left-1, nr_right);
		if (nr_left < nr_right) {
			bracket_str[i] = ')';
			gen_brackets(bracket_str, ++i, nr_left, --nr_right);
		}
		return;
	}
	while(nr_right--)
		bracket_str[i++] = ')';
	bracket_str[i] = '\0';
	printf("%s\n", bracket_str);
}
int 
main(int argc, char *argv[]) {
	
	int nr_brackets;
	char *bracket_str;

	if (argc < 2){
		printf("test_brackets <nr_brackets>\n");
		return 0;
	}


	sscanf(argv[1], "%d", &nr_brackets);
	bracket_str = malloc(nr_brackets*2+1);
	memset(bracket_str, 0, nr_brackets*2+1);
	gen_brackets(bracket_str, 0, nr_brackets, nr_brackets);

	return 0;
}
