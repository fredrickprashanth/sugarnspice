#include <stdio.h>
#define __GNU_SOURCE
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define NARGS 4
int
main(int argc, char *argv[]) {

	char tmp_name[256];
	char tmp_path[128];
	int flags;
	char *buf;
	int buf_sz;
	int file_sz;
	int do_pread, do_pwrite, do_ftrunc;
	int fd, ret, i, iters, nr_open, nr_arg;
	char cmd[16];
	

	if (argc < (NARGS+1)) {
		printf("test_fs <path> <iters> <[rwt] <buf_sz>\n");
		printf("r - pread , w - pwrite, t - trunc\n");
		return 0;
	}
	nr_arg = 1;
	sscanf(argv[nr_arg++], "%s", tmp_path);
	sscanf(argv[nr_arg++], "%d", &iters);
	sscanf(argv[nr_arg++], "%s", cmd);
	sscanf(argv[nr_arg++], "%d", &buf_sz);

	do_pread = 0;
	do_pwrite = 0;
	do_ftrunc = 0;
	fd = 0;
	file_sz = iters*buf_sz;

	if (strchr(cmd, 'r'))
		do_pread = 1;
	if (strchr(cmd, 'w'))
		do_pwrite = 1;
	if (strchr(cmd, 't'))
		do_ftrunc = 1;
	
	if (do_pread || do_pwrite)
		nr_open = 1;
	else
		nr_open = iters;

	printf("path = %s iter = %d cmd = %s buf_sz = %d \n",
		tmp_path, iters, cmd, buf_sz);
	printf("file_sz = %d nr_open = %d\n", file_sz, nr_open);
	printf("do_pread = %d do_pwrite = %d do_ftrunc = %d\n",
			do_pread, do_pwrite, do_ftrunc);
	buf = malloc(buf_sz);
	memset(buf, 0xaa, buf_sz);

	for (i = 0; i<iters; i++) {
		
		//printf("iter%d\n", i);
		if (nr_open > 0 && nr_open--) {
			if (fd > 0) {
				close(fd);
				remove(tmp_name);
			}
			sprintf(tmp_name, "%s/test_fscalls.tmp.XXXXXXX", tmp_path);
			fd = mkostemp(tmp_name, O_RDWR);
			//printf("file %s\n", tmp_name);
			if (fd < 0) {
				perror("mkostemp:");
				return -1;
			}
			if (do_ftrunc) {
				if ((ret = ftruncate(fd, file_sz)) < 0) {
					perror("ftruncate:");
					return ret;
				}
			}
				
		}
		if (do_pwrite) {
			if ((ret = pwrite(fd, buf, buf_sz, (i*buf_sz)/iters) < 0)){
				perror("pwrite:");
				return ret;
			}
		}
		if (do_pread) { 
			if ((ret = pread(fd, buf, buf_sz, random()%file_sz))< 0) {
				perror("pread:");
				return ret;
			}
		}
	
		
	}
	
	if (fd > 0) {
		close(fd);
		remove(tmp_name);
	}
	

	return 0;
}
