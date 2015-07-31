#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <glib.h>

GSList *alloc_string_table;
GHashTable *alloc_track_table;
GHashTable *alloc_stat_table;

struct alloc_data {
	char *alloc_string;
	void *ptr;
	int size;
};

struct alloc_stat {
	uint64_t nr_allocs;
	uint64_t nr_frees;
	char *alloc_string;
};


void
debug_alloc(void *ptr, int size, char *s) {
	
	struct alloc_stat *as;

	if ((as = g_hash_table_lookup(alloc_stat_table, s))) {
		as->nr_allocs++;
	} else {
		/* this will remain forever */
		as = malloc(sizeof(struct alloc_stat));
		as->nr_allocs = 0;
		as->nr_frees = 0;
		as->nr_allocs++;
		as->alloc_string = strdup(s);
		g_hash_table_insert(alloc_stat_table, as->alloc_string, as);
		alloc_string_table = g_slist_append(alloc_string_table, strdup(s));
	}

	/* free this track data at debug_free */
	struct alloc_data *ad = malloc(sizeof(struct alloc_data));
	ad->ptr = ptr;
	ad->size = size;
	ad->alloc_string = as->alloc_string;
	g_hash_table_insert(alloc_track_table, ptr, ad);

}

void 
print_stats() {
	int i;
	char *alloc_string;
	struct alloc_stat *as;
	GSList *item;
	for (item = alloc_string_table; item;
			item = item->next) {
		alloc_string = item->data;
		if ((as = g_hash_table_lookup(alloc_stat_table, alloc_string))) {
			printf("%-32s %08d %08d %08d\n", 
					alloc_string, as->nr_allocs, as->nr_frees,
					as->nr_allocs - as->nr_frees);
		}

	}
}

void 
debug_free(void *ptr) {
	struct alloc_data *ad;
	struct alloc_stat *as;
	char *alloc_string;
	if ((ad = g_hash_table_lookup(alloc_track_table, ptr))) {
		if ((as = g_hash_table_lookup(alloc_stat_table, 
						ad->alloc_string))) {
			as->nr_frees++;
		}
		free(ad);
		g_hash_table_remove(alloc_track_table, ptr);
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
	alloc_stat_table = g_hash_table_new(g_str_hash, g_str_equal);
	alloc_track_table = g_hash_table_new(g_direct_hash, g_direct_equal);
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

	printf("Starting\n");
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
