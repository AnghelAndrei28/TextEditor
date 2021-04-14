#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef list
#define list

typedef struct node {
  char * data;
  struct node * next;
  struct node * prev;
}
Node;

Node * init_node(char * );
Node * list_append(Node * , char * );
Node * list_prepend(Node * , char * );
void list_print_forward(Node * );
Node * list_remove(Node * , Node * );
Node * introduce_nod(Node * , int, char * );
void delete_list(Node ** );
int list_length(Node * );

#endif