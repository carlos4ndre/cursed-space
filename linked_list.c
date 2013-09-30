#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

void init_linked_list(struct linked_list *list)
{
   list = (struct linked_list*) malloc(sizeof(struct linked_list));
  
   list->head = NULL;
   list->tail = NULL;
   list->size = 0; 
}

void add_node_end(struct node *node,struct linked_list *list)
{
   if(list == NULL) {
       printf("[!] Cannot add element to empty list.");
       exit INVALID_DATA;
   }

   if(node == NULL) {
       printf("[!] Cannot add element to empty list.");
       exit INVALID_DATA;
   }

   if(list->head == NULL)
   {
       node->next = NULL;
       node->previous = NULL;
       list->head = node;
       list->tail = list->head;
   }
   else
   {
       struct node *tmp;

       tmp = list->tail;
       tmp->next = node;

       node->next = NULL;
       node->previous = tmp;
       list->tail = node;
   }

   list->size++;
}

void remove_node(struct node *node,struct linked_list *list)
{
   if(list == NULL) {
       printf("[!] Cannot remove element on empty list.");
       exit INVALID_DATA;
   }

   if(node == NULL) {
       printf("[!] Cannot remove element with empty node.");
       exit INVALID_DATA;
   }

   if(list->size == 1) 
   {
       free(list->head);
       list->head = NULL;
       list->tail = NULL;
   }
   else
   {
       struct node *tmp = node;
       node->previous->next = node->next;
       free(tmp);
   }

   list->size--;
}

int get_linked_list_size(struct linked_list *list)
{
    if(list == NULL)
        return 0;

    return list->size;
}

struct node* get_node(struct linked_list *list,int position)
{
    if(list == NULL && list->size == 0)
    {
       printf("[!] Cannot get node from an empty node.");
       exit INVALID_DATA; 
    }

    struct node* tmp = list->head;
    int i = 0;

    while(tmp->next != NULL) {
        if(i == position)
              return tmp;
        if(tmp->next != NULL)
              tmp = tmp->next;
        i++;
    }
}
