#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef stack
#define stack

typedef struct SNode {
    char* comanda;
    struct SNode *next;
    struct SNode *prev;
} SNode;

SNode *init_node(char*);
SNode* push(SNode*, char*); 
SNode* pop(SNode*);
char* top(SNode*);
int is_empty(SNode*);
void delete_stack(SNode **);

#endif