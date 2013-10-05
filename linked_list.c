#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

struct linked_list* init_linked_list()
{
   struct linked_list *list = (struct linked_list *) malloc(sizeof(struct linked_list));
   list->head = NULL;
   list->tail = NULL;
   list->size = 0; 

   return list;
}

int add_node_end(struct node *node,struct linked_list *list)
{
   if(list == NULL) {
       printf("[!] List is not properly initialized.\n");
       return -1;
   }

   if(node == NULL) {
       printf("[!] Node is empty.\n");
       return -1;
   }

   if(list->head == NULL)
   {
       node->next = NULL;
       node->previous = NULL;
       list->head = node;
       list->tail = node;
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

   return 0;
}

int remove_node(struct linked_list *list,int position)
{
   if(list == NULL) {
       printf("[!] Cannot remove element on empty list.\n");
       return -1;
   }

   if(position < 0 || position > list->size-1)
   {
       printf("[!] Invalid position.\n");
       return -1;
   }

   if(list->size == 1) 
   {
       free(list->head);
       list->head = NULL;
       list->tail = NULL;
   }
   else
   {
       int i;
       struct node *tmp = list->head;

       for(i=0;i<list->size;i++)
       {
           if(i == position)
               break;

           if(tmp == NULL) {
               printf("[!] Inconsistent data, position should be valid.");
               return -1;
           }
           tmp = tmp->next;
       }

       if(tmp == list->head)
           list->head = tmp->next;

       if(tmp->previous != NULL)
           tmp->previous->next = tmp->next;

       free(tmp);
   }

   list->size--;

   return 0;
}

int get_linked_list_size(struct linked_list *list)
{
    if(list == NULL)
        return 0;

    return list->size;
}

struct node* get_node(struct linked_list *list,int position)
{
   if(list == NULL || list->head == NULL)
   {
       printf("[!] Cannot get node from an empty node.\n");
       return NULL; 
   }

   if(position < 0 || position > list->size-1)
   {
       printf("[!] Invalid position.\n");
       return NULL;
   }


    struct node* tmp = list->head;
    int i = 0;

    while(tmp != NULL) {
       if(i == position)
            break;

       if(tmp->next != NULL)
            tmp = tmp->next;

       i++;
    }

    return tmp;
}
