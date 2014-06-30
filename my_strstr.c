#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void
print_array(int *a, int len) {
	int i;
	for (i = 0; i < len; i++)
		printf("[%d]", a[i]);  
	printf("\n");
}

#define PRIME 11
#define MOD_PRIME 1299709
unsigned long
hash_new(char *str, int len) {
	int i = 0;
	unsigned long hash = 0;;
	hash += str[0];
	//printf("hash i = %d %c %lu\n", i, str[i], hash);
	for (i = 1; i<len; i++){
		hash *= PRIME;
		hash += str[i];
		//printf("hash i = %d %c %lu\n", i, str[i], hash);
	}
	return hash;
}

unsigned long
hash_add_sub(unsigned long prev_hash,
char sub, char add, int len) {
	int i;
	unsigned long sub_hash = sub;
	//printf("hash sub %c %c %lu\n", sub, add, prev_hash);
	for (i = 0; i<len-1; i++)
		sub_hash*=PRIME;
	//printf("hash sub %lu %c %lu\n", sub_hash, add, prev_hash);
	prev_hash -= sub_hash;
	prev_hash *= PRIME;
	prev_hash += add;
	return prev_hash;
	
}
int my_strstr_hash(char *string, char *substring) {
	
	unsigned long hs, hsub;
	int substr_len, str_len;
	int i;

	substr_len = strlen(substring);
	str_len = strlen(string);
	hsub = hash_new(substring, substr_len);
	hs = hash_new(string, substr_len);
	for (i=0; i<(str_len-substr_len); i++) {
		if (hs == hsub) {
			if(!strncmp(&string[i], substring, substr_len))
				return i;
		}
		//printf("i = %d hs = %lu hsub = %lu\n", i, hs, hsub);
		hs = hash_add_sub(hs, string[i], string[i+substr_len], substr_len);
	}
	return -1;
}
int
my_strstr(char *string, char *substring) {

	int *overlap;
	int substr_len, str_len;
	int i,j, match;

	substr_len = strlen(substring);
	str_len = strlen(string);
	overlap = malloc(substr_len*sizeof(int));
	
	if (substr_len > str_len)
		return -1;
	overlap[0] = 0;
	j = 0;
	for (i=1; i<substr_len; i++) {
		if (substring[i] == substring[j++]) {
			overlap[i] = overlap[i-1]+1;
		}
		else {
			overlap[i] = 0;
			j = 0;
		}
	}
	
	//print_array(overlap, substr_len);
	j = 0;
	match = 0;
	for (i=0; i<str_len; i++) {
		if (j == substr_len) { // match found
			printf("match found at [%d]\n", i-match);
			return (i-match);
		}
		if (string[i] == substring[j++]){
			match++;
		}
		else {
			j = 0;
			if (match > 1) { //partial match
				match = overlap[match-1];
				j = match;		
				i = i -1;
			}
			else {
				match = 0;
			}
		}
		//printf("i = %d match = %d j = %d\n", i, match, j);
		
	}
	return -1;
}

int nr_pages;
#define NARGS 1
#define PAGE_SZ 4096
#define MAX_STRLEN (PAGE_SZ*nr_pages)
int
main(int argc, char *argv[]) {
	
	char *str, *substr;
	char *substr_pos;
	int pos, my_pos, my_hash_pos;
	int i, j;
	if (argc < (NARGS+1)){
		printf("my_strstr <nr_pages>\n");
		return 0;
	}

	sscanf(argv[1], "%d", &nr_pages);
	str = malloc(MAX_STRLEN);	
	substr = malloc(MAX_STRLEN);
	memset(str, 0, MAX_STRLEN);
	memset(substr, 0, MAX_STRLEN);
	j = 0;
	for (i = 0; i<MAX_STRLEN-1; i++) {
		str[i] = random()%26 + 'a';
		if (random()&1)
			substr[j++] = random()%26+'a';
	}
	
	my_pos = my_strstr(str, substr);
	substr_pos  = strstr(str, substr);
	if (substr_pos)
		pos = substr_pos - str;
	else
		pos = -1;
	my_hash_pos = my_strstr_hash(str, substr);
	printf("pos = %d my_pos = %d my_hash_pos = %d\n", 
				pos, my_pos, my_hash_pos);
	if (pos == my_pos)
		printf("Correct\n");
	else
		printf("Incorrect!!\n");

	return 0;
	
}
