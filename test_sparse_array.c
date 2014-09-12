#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

typedef struct {
    void **objs;
    int size;
    int *indices;
    int nr_objs;
} sparse_array_t;

#ifndef NDEBUG
#define debug_printf(fmt,...) printf("%s:" fmt, __FUNCTION__, ##__VA_ARGS__)
#else
#define debug_printf(...)
#endif

static int
binary_search(int *array, int data, int length) {
    
    int low = 0;
    int high = low + length;
    int mid = 0;
        
    if (length == 0)
        return (~0);
    debug_printf("start: low %d mid %d high %d arr %d\n", low, mid, high, array[mid]);
    while (low < high) {
        debug_printf("inside: low %d mid %d high %d arr %d\n", low, mid, high, array[mid]);
        mid = low + (high - low)/2;
        if (data == array[mid])
            break;
        if (data < array[mid]) {
            high = mid;
        } else {
           low = mid + 1;  
        }

    }

    debug_printf("end: low %d mid %d high %d arr %d data %d\n", low, mid, high, array[mid], data);
    if (array[mid] == data) {
        debug_printf("bsearch found %d at %d\n", data, high);
        return mid;
    }
    else
    if (data > array[mid])
        return ~(mid+1);
    else
        return ~mid;
}

#define SPARSE_ARRAY_INIT_LEN (64)
sparse_array_t*
sparse_array_create() {

    sparse_array_t *sa;

    sa = malloc(sizeof(*sa));
    if (!sa)
        return NULL;
    sa->objs = malloc(sizeof(sa->objs[0])*SPARSE_ARRAY_INIT_LEN);
    sa->indices = malloc(sizeof(sa->indices[0])*SPARSE_ARRAY_INIT_LEN);
    sa->size = SPARSE_ARRAY_INIT_LEN;
    sa->nr_objs = 0;
    
    return sa;
}

void
sparse_array_destroy(sparse_array_t *sa) {
    free(sa->objs);
    free(sa->indices);
    free(sa);
}

int
sparse_array_set(sparse_array_t *sa, int index, void *obj) {
    
    int ret;
    int *indices_arr;
    void **obj_arr;
    ret = binary_search(sa->indices, index, sa->nr_objs);
    if (ret < 0) {
        ret = ~ret;
        debug_printf("index %d was not found ret = %d nr_objs %d size %d\n", 
                     index, ret, sa->nr_objs, sa->size);
        if (sa->nr_objs == sa->size) {
            debug_printf("size overflow %d, realloc.\n", sa->size);
            indices_arr = realloc(sa->indices, sizeof(sa->indices[0])*sa->size*2);
            if (!indices_arr) {
                debug_printf("realloc failed with err %d\n", errno);
                return -errno;
            }
            sa->indices = indices_arr;
            obj_arr = realloc(sa->objs, sizeof(sa->objs[0])*sa->size*2);
            if(!obj_arr) {
                debug_printf("realloc failed with err %d\n", errno);
                return -errno;
            }
            sa->objs = obj_arr;
            sa->size *= 2;
        }
        if (ret < sa->size) {
            memmove(&sa->indices[ret+1], &sa->indices[ret], 
                    (sa->nr_objs - ret)*sizeof(sa->indices[0]));
            memmove(&sa->objs[ret+1], &sa->objs[ret], 
                    (sa->nr_objs - ret)*sizeof(sa->objs[0]));
            sa->indices[ret] = index;
            sa->objs[ret] = obj;
        } else {
            sa->indices = indices_arr;
            sa->indices[ret]  = index;
            sa->objs[ret] = obj;
        }
        sa->nr_objs++;
        debug_printf("inserting at internal position %d (%d, %d) size %d nr_objs %d\n", 
                      ret, index, (int)obj, sa->size, sa->nr_objs);
    } else {
        debug_printf("index was found at %d setting new obj\n", ret);
        sa->objs[ret] = obj;
    }

    return 0;
}

int
sparse_array_get(sparse_array_t *sa, int index, void **obj) {
    int ret;
    ret = binary_search(sa->indices, index, sa->nr_objs);
    if (ret >= 0) {
        debug_printf("index %d was found\n", ret);
        *obj = sa->objs[ret];
        return 0;
    }
    debug_printf("index was not found ret = %d nr_objs %d size %d\n", ret, sa->nr_objs, sa->size);
    return -1;
}


void
test_failed(char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    abort();
}


int
run_test1() {

    sparse_array_t *sa;
    int *objp, rnd_data, rnd_i;
    int i, ret;

    sa = sparse_array_create();
    if (!sa) {
        test_failed("sparse_array_create failed\n");
    }

#define MAX_NUM (20000)
#define RND_NUM (random()%MAX_NUM)

    for (i = 0; i<10000; i++) {
        rnd_i = RND_NUM;
        rnd_data = RND_NUM;
        debug_printf("insert arr[%d] = %d\n", rnd_i, rnd_data);
        ret = sparse_array_set(sa, rnd_i, (void *)rnd_data);
        if (ret < 0) {
            test_failed("sparse_array_set failed with ret = %d\n", ret);
        }
        ret = sparse_array_get(sa, rnd_i, (void **)&objp);
        if (ret < 0) {
            test_failed("sparse_array_get failed with ret = %d\n", ret);
        }
        if ((int)objp != rnd_data) {
            test_failed("sparse array insert verify failed "
                        "index %d set val = %d get val = %d\n", rnd_i, rnd_data, objp);
        }
    }

    sparse_array_destroy(sa);
    
    return 0;
}


int
main() {
    run_test1();
    printf("All tests passed successfully.\n");
    return 0;
}
