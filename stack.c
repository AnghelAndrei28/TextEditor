#include "stack.h"

SNode *init_snode(char* comanda) {
	SNode *node = (SNode *)malloc(sizeof(SNode));
    node->comanda = (char* )malloc(100 * sizeof(char));
	strcpy(node->comanda, comanda);
	node->next = NULL;

	return node;
}

int is_empty(SNode* head) {
    if(head == NULL) {
        return 1;
    }
    else {
        return 0;
    }
}

SNode* push(SNode *head, char* comanda) {
	SNode *new_node = init_snode(comanda);

	if (is_empty(head)) {
		return new_node;
	}
	else {
		new_node->next = head;
	}

	return new_node;
}

SNode* pop(SNode *head) {
	if (is_empty(head))
		return NULL;
	else if (is_empty(head->next)) {
		return NULL;
	} else {
		SNode* to_be_removed = head;
		head = head->next;
        free(to_be_removed->comanda);
		free(to_be_removed);

		return head;
	}

	return head;
}

char* top(SNode* head) {
    return head->comanda;
}

void delete_stack(SNode ** head) {
  SNode * current;
  while ( * head) {
    current = * head;
    * head = ( * head) -> next;
    free(current -> comanda);
    free(current);
  }
}