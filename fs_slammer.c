#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

char valid_chars[]="abcdefghiklmonpqrstuvwxyz0123456789";
int gen_random_filename(char *str, int len)
{
	int i=0;
	while(i<len-1 && (str[i++] = valid_chars[random()%(sizeof(valid_chars)-1)]));
	str[i] = 0;
	return i;
}


int max_pages = 100;
pthread_mutex_t total_bytes_mutex = PTHREAD_MUTEX_INITIALIZER;

volatile unsigned long int total_bytes_written;
volatile unsigned long int total_bytes_read;


void 
raw_reader_cleanup(void *arg)
{
	int fd=(int)arg;
	close(fd);
}
void*
raw_reader(void *arg)
{
	char *hd_name = (char *)arg;
	char buf[4096];	
	int rnd_seek_pos, seek_mode;
	int fd = 0;
	int ret;

	fd = open(hd_name, O_RDONLY);
	if(fd<0)
	{
		perror("open");
		pthread_exit(0);
	}
	pthread_cleanup_push(raw_reader_cleanup, (void *)fd);

	while(1)
	{
		rnd_seek_pos = random();
		seek_mode = rnd_seek_pos & 1 ? SEEK_SET:SEEK_END;
		lseek(fd, rnd_seek_pos, seek_mode);
		if((ret = read(fd, buf, sizeof(buf)))<0)
		{
			perror("read");
			break;
		}
		total_bytes_read+=ret;
		pthread_testcancel();
	}

	close(fd);
	pthread_cleanup_pop(0);
	pthread_exit(0);
	
}
void
slam_cleanup(void *arg)
{
	remove((char*)arg);
}
void*
slam_junk(void *data)
{
	int fd, fd_rand;
	int pages, bytes_wrote, ret;
	char fname[100];
	char buff[4096];

	gen_random_filename(fname, sizeof(fname));
	pthread_cleanup_push(slam_cleanup, fname);
	while(1)
	{
		// O_SYNC tried
		fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXO);
		if(fd < 0)
		{
			printf("open:%s:\n",fname);
			perror("open:");
			pthread_exit(NULL);
		}
		fd_rand = open("/dev/zero", O_RDONLY);
		if(fd_rand < 0)
		{
			perror("open:/dev/zero:");
			close(fd);
			pthread_exit(NULL);
		}
		//pages = random()%max_pages + 1;
		pages = max_pages;
		bytes_wrote = 0;
		while(pages--)
		{
			if((ret = read(fd_rand, buff, sizeof(buff)))<0)
			{	
				perror("read:");
				close(fd_rand);
				close(fd);
				pthread_exit(NULL);
			}
			if((ret = write(fd, buff, ret))<0)
			{
				perror("write:");
				close(fd_rand);
				close(fd);
				pthread_exit(NULL);
				
			}
			bytes_wrote+=ret;
			total_bytes_written += bytes_wrote;
		}
		
		//lets see..whether it hangs
		fsync(fd);
		close(fd);
		close(fd_rand);

		fd = open(fname, O_RDONLY);
		while(read(fd, buff, sizeof(buff)));
		close(fd);

		//printf("%d bytes written\n", bytes_wrote);
		//pthread_mutex_lock(&total_bytes_mutex);
		//pthread_mutex_unlock(&total_bytes_mutex);		

			
		pthread_testcancel();
	}
	pthread_cleanup_pop(0);
	pthread_exit(NULL);
}
		
void 
termination_hdlr(int signum)
{
	printf("Quiting\n");
}
int 
main(int argc, char *argv[])
{
	int ret;
	int i;

	if(argc < 6)
	{
		printf("Usage: fs_slammer <hddisk> <dir> <no. of files> <no-of-pages> <time in secs> <rw>\n");
		return -1;
	}
	
	time_t tt;
	time(&tt);
	srandom(tt);

	char fs_path[256];
	char *hd_name;
	struct stat stat_buf;
	char **argp =++argv;

	hd_name = (char *)malloc(10);
	strcpy(hd_name, *argp++);
	if((ret = stat(hd_name, &stat_buf))<0)
	{
		perror("stat:");
		return ret;
	}
	
	strcpy(fs_path, *argp++);
	if((ret = stat(fs_path, &stat_buf))<0)
	{
		perror("stat:");
		return ret;
	}
	
	if((ret=chdir(fs_path))<0)
	{
		printf("%s:\n", fs_path);
		perror("chdir:");
		return ret;
	}
	
	int no_threads = 0;

	sscanf(*argp++, "%d", &no_threads);

	sscanf(*argp++, "%d", &max_pages);

	int slam_time;
	sscanf(*argp++, "%d", &slam_time);

	char rw[4];
	sscanf(*argp++, "%s", rw);

	pthread_t *read_threads;
	pthread_t *slam_threads;

	if(strchr(rw, 'w'))
	{
		slam_threads = (pthread_t*)malloc(sizeof(pthread_t)*no_threads);

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


		for(i = 0; i < no_threads; i++)
		{
			if((ret = pthread_create(slam_threads+i, &attr, slam_junk, NULL)))
			{
				perror("pthread_create:");
				return ret;
			}
		}
	}
	
	if(strchr(rw, 'r'))
	{
		read_threads = (pthread_t*)malloc(sizeof(pthread_t)*no_threads);

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


		for(i = 0; i < no_threads; i++)
		{
			if((ret = pthread_create(read_threads+i, &attr, raw_reader, hd_name)))
			{
				perror("pthread_create:");
				return ret;
			}
		}
	}
	

	signal(SIGINT, termination_hdlr);
	signal(SIGTERM, termination_hdlr);
	signal(SIGQUIT, termination_hdlr);
	if(slam_time <0)
		pause();
	else
		sleep(slam_time);

	for(i = 0; i < no_threads; i++)
	{
		if(strchr(rw, 'w'))
			pthread_cancel(slam_threads[i]);
		if(strchr(rw, 'r'))
			pthread_cancel(read_threads[i]);
	}

	printf("Total bytes written = %lu\n", total_bytes_written);
	printf("Total bytes raw read = %lu\n", total_bytes_read);
	pthread_exit(NULL);

}

