#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct chain_hook_s {
	int key;
	//int valid;
	void *data;
} chain_hook_t;

typedef struct slot_s {
	int nr_chain_hooks;
	int next_chain_hook;
	chain_hook_t *chain_hooks;
} slot_t;

typedef struct hash_table_s {
	int nr_slots;
	slot_t *slots;
} hash_table_t;

static void
slot_init(slot_t *slt, int nr_chain_hooks) {

	slt->chain_hooks = malloc(sizeof(chain_hook_t)*nr_chain_hooks);
	if (!slt->chain_hooks) { 
		perror("malloc");
		assert(0);
	}
	slt->nr_chain_hooks = nr_chain_hooks;
	slt->next_chain_hook = 0;
}

hash_table_t*
hash_table_create(int nr_slots, int nr_chain_hooks) {
	
	hash_table_t *ht;
	slot_t *slt;
	int i;

	ht = malloc(sizeof(*ht));
	ht->slots = malloc(sizeof(slot_t)*nr_slots);
	if (!ht) {
		perror("malloc");
		assert(0);
	}
	ht->nr_slots = nr_slots;
	if(!ht) {
		perror("malloc");
		assert(0);
		return ht;
	}

	for (i=0; i<nr_slots; i++){
		slt = &ht->slots[i];
		slot_init(slt, nr_chain_hooks);
	}
	return ht;
}

int 
hash_table_usage(hash_table_t *ht) {

	int i;
	int usage = 0;
	slot_t *slt;
	for (i=0; i<ht->nr_slots; i++) {
		slt = &ht->slots[i];
		usage += (slt->next_chain_hook*100/slt->nr_chain_hooks);
	}
	usage /= ht->nr_slots;
	return usage;
}

int
hash_table_expand(hash_table_t *ht) {
	
	slot_t *new_slts, *old_slts, *slt;
	chain_hook_t *ch;
	int i,j;

	old_slts = ht->slots;
	new_slts = malloc(ht->nr_slots*2*sizeof(slot_t));
	if (!new_slts) {
		perror("malloc");
		return -1;
	}
	ht->slots = new_slts;
	ht->nr_slots *= 2;
	
	for (i=0; i<ht->nr_slots/2; i++) {
		slt = &old_slts[i];
		for (j=0; j<slt->next_chain_hook; j++) {
			ch = &slt->chain_hooks[j];
			hash_table_insert(ht, ch->key, ch->data);
		}
	}
	
	free(old_slts);

	return 0;
}

/*
int
hash_table_shrink(hash_table *ht) {
}
*/

static int
slot_chain_hooks_expand(slot_t *slt) {
	chain_hook_t *old_ch, *new_ch;

	new_ch = malloc(sizeof(chain_hook_t)*slt->nr_chain_hooks*2);
	if (!new_ch) {
		perror("malloc");
		return -1;
	}
	memcpy(new_ch, slt->chain_hooks, 
		slt->nr_chain_hooks*sizeof(chain_hook_t));
	free(slt->chain_hooks);
	slt->chain_hooks = new_ch;
	slt->nr_chain_hooks *= 2;
}

static int 
hash_key(int key, int nr_slots) {
#if 0
	return ((key>>8|key)&(nr_slots-1));
#endif
	return (key&(nr_slots-1));
}
int 
hash_table_insert(hash_table_t *ht, int key, void *data) {
	
	int slot_i;
	slot_t *slt;
	chain_hook_t *ch;

	slot_i = hash_key(key, ht->nr_slots);
	slt = &ht->slots[slot_i];
	
	if(slt->next_chain_hook == 
			slt->nr_chain_hooks) {
		
		if (hash_table_usage(ht) > 90) {
		
			printf("expanding table\n");
			hash_table_expand(ht);
		}
		else { 
			printf("expanding slot %d\n", slot_i);
			slot_chain_hooks_expand(slt);
		}

		hash_table_insert(ht, key, data);
		
	}
	else {
		//printf("inserting %d at slot %d hook %d\n", key,
				//slot_i, slt->next_chain_hook);
		ch = &slt->chain_hooks[slt->next_chain_hook++];
		ch->key = key;
		ch->data = data;
	}
	
	return 0;

}

#define NR_SLOTS 1024
#define NR_CHAIN_HOOKS 64
int 
main(int argc, char *argv[]){
	
	int nr_slots;
	int i;
	hash_table_t *ht;
	
	srand(time(NULL));

	ht = hash_table_create(NR_SLOTS, NR_CHAIN_HOOKS);

	for (i = 0; i < NR_SLOTS*NR_CHAIN_HOOKS; i++)
		hash_table_insert(ht, rand(), (void *)rand());

	printf("usage  = %d\n", hash_table_usage(ht));

	return 0;
}	
