#pragma once

typedef struct list_node
{
	void * value;
	struct list_node * next;
} node;

typedef struct 
{
	node * head;
} list;

list * ll_new();
void ll_cons(list * l, void * value);
void ll_remove(list * l, void * value);
int ll_count(list * l);
void ll_free(list * l);

/*
* Removes all nodes from list matching the expression.
*/
#define REMOVE_ALL_X(llist, type, expr)  {  			\
		node * prev_node_rm = NULL;         			\
		node * node_rm = llist->head;             		\
		while(node_rm != NULL) {              			\
			prev_node_rm = node_rm;						\
			node_rm = node_rm->next;					\
			type* x = prev_node_rm->value;         		\
			if(expr)                  					\
				ll_remove(llist, prev_node_rm->value);  \
	}}

#define FOREACH_X(llist, type)\
	node * node_it = llist->head;\
	type * x;         		\
		while(node_it != NULL && ((x = node_it->value) && ((node_it = node_it->next) || 1)))  //little hack

