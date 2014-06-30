#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

int
main(int argc, char *argv[]) {

	int in_hex = 0;
	int sup_mod;

	char short_opts[] = "smt:";
	struct option long_opts[] = {
		{"hex", no_argument, &in_hex, 1},
		{"slot_num", no_argument, 0, 's'},
		{"mod_num", no_argument, 0, 'm'},
		{"set_mts_active_sup", required_argument, 0, 't'},
		{0, 0, 0, 0},
	};
	
	char c;
	
	int slot_num = 10;

	while ((c = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1){

		switch (c) {
			case 0:
				break;
			case 's':
				if (in_hex) {
					printf("slot number is %x\n", slot_num);
				} else {
					printf("slot number is %d\n", slot_num);
				}
				break;
			case 'm':
				if (in_hex) {
					printf("mod number is %x\n", slot_num+1);
				} else {
					printf("mod number is %d\n", slot_num+1);
				}
				break;
			case 't':
				if (!optarg)
					return -1;
				printf("Entering here\n");
				sscanf(optarg, "%d", &sup_mod);
				printf("Setting active sup node %d\n", sup_mod);
				break;
			default:
				return -1;
		} // switch
	} // while

	return 0;
} //main
