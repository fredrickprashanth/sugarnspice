#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int
datesort(const struct dirent **a, const struct dirent **b) {

    struct stat st_a, st_b;
    if (stat(a[0]->d_name, &st_a)) {
        perror("stat failed");
        return 0;
    }
    if (stat(b[0]->d_name, &st_b)) {
        perror("stat failed");
        return 0;
    }
    if (st_a.st_mtime > st_b.st_mtime)
        return 1;
    if (st_a.st_mtime < st_b.st_mtime)
        return -1;
    return 0;	
}

int selector(const struct dirent *ent) {
    if(ent->d_type == DT_REG)
        return 1;
    return 0;
}

int
main(int argc, char *argv[])
{
   struct dirent **namelist;
   int n;

   n = scandir(argv[1], &namelist, selector, datesort);
   if (n < 0)
       perror("scandir");
   else {
       while (n--) {
       printf("%s\n", namelist[n]->d_name);
       free(namelist[n]);
       }
       free(namelist);
   }
}
