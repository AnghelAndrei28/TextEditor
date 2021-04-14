#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef stack
#define stack

typedef struct SNode {
  char * comanda;
  struct SNode * next;
}
SNode;

SNode * init_snode(char * );
SNode * push(SNode * , char * );
SNode * pop(SNode * );
char * top(SNode * );
int is_empty(SNode * );
void delete_stack(SNode ** );
void print_stack(SNode * );

#endif