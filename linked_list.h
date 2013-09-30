#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#define INVALID_DATA -1

struct node
{
   void *data;
   struct node *next;
   struct node *previous;
};

struct linked_list
{
   struct node *head;
   struct node *tail;
   int size;
};


void init_linked_list(struct linked_list *list);
void add_node_end(struct node *node,struct linked_list *list);
void remove_node(struct node *node,struct linked_list *list);
int get_linked_list_size(struct linked_list *list);
struct node* get_node(struct linked_list *list,int position);

#endif
