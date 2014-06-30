typedef struct list {
	struct list *next;
	void *data;
} list_t;

void
list_add(list_t **head, list_t *x) {
	x->next = *head;
	*head = x;
}

void 
list_add_tail(list_t **head, list_t *x) {
	list_t *tail;
	
}
int main(){

	list_t *head;
	
}
