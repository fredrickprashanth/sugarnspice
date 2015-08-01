#define __USE_GNU
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <search.h>
#include <assert.h>
#include <errno.h>


#define HTABLE_SZ 1024
struct alloc_string {
	char *string[HTABLE_SZ];
	int count;
} alloc_string_table;

struct alloc_data {
	char *alloc_string;
	void *ptr;
	int size;
};

struct alloc_stat {
	uint64_t nr_allocs;
	uint64_t nr_frees;
};

void *alloc_track_table = NULL;
struct hsearch_data alloc_stat_table;

int ad_compare(const void *a1, const void *a2) {
	
	void *p1, *p2;
	p1 = ((struct alloc_data *)a1)->ptr;
	p2 = ((struct alloc_data *)a2)->ptr;
	if ((uintptr_t)p1 < (uintptr_t)p2)
		return -1;
	if ((uintptr_t)p1 > (uintptr_t)p2)
		return 1;
	return 0;
}

void
debug_alloc(void *ptr, int size, char *str) {
	ENTRY item, *item_p = NULL;
	int ret;
	char *alloc_string = strdup(str);

	
	struct alloc_stat *as;
	item.key = alloc_string;
	ret = hsearch_r(item, FIND, &item_p, &alloc_stat_table);
	if (ret < 0) {
		fprintf(stderr, "Error(%d) finding %s in alloc_stat_table\n",
				errno, alloc_string);
		return;
	}
	if (item_p == NULL) {
		/* this will remain forever */
		as = malloc(sizeof(struct alloc_stat));
		as->nr_allocs = 1;
		as->nr_frees = 0;
		item.data = as;
		item.key = alloc_string;
		ret = hsearch_r(item, ENTER, &item_p, &alloc_stat_table);
		if (ret < 0) {
			fprintf(stderr, "Error(%d) adding %s to alloc_stat_table\n",
					errno, alloc_string);
			free(as);
			return;
		}
		alloc_string_table.string[alloc_string_table.count] =
			alloc_string;
		alloc_string_table.count++;
	} else {

		as = (struct alloc_stat *)item_p->data;
		as->nr_allocs++;
		free(alloc_string);
		alloc_string = item_p->key;
	}


	/* free this track data at debug_free */
	struct alloc_data *ad = malloc(sizeof(struct alloc_data));
	ad->ptr = ptr;
	ad->size = size;
	ad->alloc_string = alloc_string;
	if (NULL == tsearch(ad, &alloc_track_table, ad_compare)) {
		fprintf(stderr, "Error(%d) adding ptr %p %s to alloc_track_table\n",
				errno, ptr, alloc_string);
	}
}

void 
print_stats() {
	int i;
	char *alloc_string;
	struct alloc_stat *as;
	int ret;
	ENTRY item, *item_p = NULL;
	for (i = 0; i < alloc_string_table.count;
			i++) {
		alloc_string = alloc_string_table.string[i];
		item.key = alloc_string;
		ret = hsearch_r(item, FIND, &item_p, &alloc_stat_table);
		if (ret < 0) {
			fprintf(stderr, "Error(%d) in finding %s in alloc_stat_table\n",
					errno, alloc_string);
			return;
		}
		if (item_p != NULL) {
			as = (struct alloc_stat *)item_p->data;
			printf("%-32s %08lu %08lu %08lu\n", 
					alloc_string, as->nr_allocs, as->nr_frees,
					as->nr_allocs - as->nr_frees);
		} else {
			fprintf(stderr, "Unable to find string %s in alloc_stat_table\n", 
					alloc_string);
		}
	}
}

void 
debug_free(void *ptr) {
	int ret;
	ENTRY item, *item_p;
	struct alloc_data *ad, ad_key = {0}, **adp;
	struct alloc_stat *as;
	char *alloc_string;

	ad = &ad_key;
	ad->ptr = ptr;
	adp =  tfind(ad, &alloc_track_table, ad_compare);
	if (adp == NULL) {
		assert(0);
		fprintf(stderr, "Error(%d) finding alloc info for %p\n", 
				errno, ptr);
		return;
	}
	ad = *adp;

	alloc_string = ad->alloc_string;
	tdelete(ad, &alloc_track_table, ad_compare);
	free(ad);

	item.key = alloc_string;
	ret = hsearch_r(item, FIND, &item_p, &alloc_stat_table);
	if (ret < 0) {
		fprintf(stderr, "Error(%d) finding alloc stat info for %p %s\n", 
				errno, ptr, alloc_string);
		return;
	}
	if (item_p == NULL) {
		fprintf(stderr, "Could not find the free alloc_string [%s] for addr %p\n", 
				alloc_string, ptr);
	} else {
		as = (struct alloc_stat *)item_p->data;
		as->nr_frees++;
	}
}

void*
do_alloc(int size, char *s) {
    void *ptr = malloc(size);
    debug_alloc(ptr, size, s);
    return ptr;
}

void
do_free(void *ptr) {
    debug_free(ptr);
    free(ptr);
}

void
debug_init() {
	memset(&alloc_stat_table, 0, sizeof(alloc_stat_table));
	hcreate_r(HTABLE_SZ, &alloc_stat_table);
	alloc_string_table.count = 0;
}

#define NARGS 1
int
main(int argc, char *argv[]) {

	int i, j;
	char astr[64];
	int max_iter = 30;
	int max_alloc_types = 20;
	void **p;

	if (argc < (NARGS+1)) {
		printf("Usage: %s <max_iters>\n", argv[0]);
		return 0;
	}
	sscanf(argv[1], "%d", &max_iter);

	p = malloc(sizeof(void *)*max_iter);
	srandom(time(NULL));

	debug_init();

	for (i = 0; i<max_iter; i++) {
		j = random()%max_alloc_types;
		sprintf(astr, "alloc %d", j);
		p[i] = do_alloc(20, astr);
	}
	//print_stats();
	for (i = 0; i<max_iter; i++) {
		if (random()%2 == 0)
			do_free(p[i]);
	}
	print_stats();

	return 0;
}
