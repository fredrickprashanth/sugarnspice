
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/futex.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <string.h>

#include "xlock.h"
#include "xsem.h"
#include "mycommon.h"

#define _GNU_SOURCE
#include <sys/syscall.h>
#define futex(uaddr, op, val, timeout, uaddr2, val3)  \
		syscall(SYS_futex , uaddr, op, val, NULL, NULL, NULL)

typedef struct xbarrier {
	xsem_t *sem;
	xlock_t *lock
	int count;
	int barr_count;
} xbarrier_t;
xbarrier_t*
xbarrier_create_open(char *barr_name, int count, int create_flag) {
}

xbarrier_barrier(xbarrier_t *x, int token){
	xsem_down(x->sem);
	xlock_lock(x->lock);
	x->barr_count++;
	xlock_unlock(x->lock);
	futex(
}

xbarrier_min_barrier_start(xbarrier_t *x, int token) {
}
xbarrier_min_barrier_end(xbarrier_t *x, int token) {
}
