#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

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


struct linked_list* init_linked_list();
int add_node_end(struct node *node,struct linked_list *list);
int get_linked_list_size(struct linked_list *list);
struct node* get_node(struct linked_list *list,int position);
int remove_node(struct linked_list *list,int position);

#endif
