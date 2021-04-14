#include "list.h"

Node * init_node(char * data) {
  Node * node = (Node * ) malloc(1 * sizeof(Node));
  node -> data = (char * ) malloc((strlen(data) + 1) * sizeof(char));
  strcpy(node -> data, data);
  node -> next = NULL;
  node -> prev = NULL;
  return node;
}

Node * list_append(Node * head, char * data) {
  Node * new_node = init_node(data);

  if (head == NULL)
    return new_node;
  else {
    Node * tmp = head;
    while (tmp -> next != NULL) {
      tmp = tmp -> next;
    }
    tmp -> next = new_node;
    new_node -> prev = tmp;
  }

  return head;
}

Node * list_prepend(Node * head, char * data) {
  Node * new_node = init_node(data);

  if (head == NULL) {
    return new_node;
  } else {
    head -> prev = new_node;
    new_node -> next = head;
  }

  return new_node;
}

void list_print_forward(Node * head) {
  if (head == NULL)
    return;
  else if (head -> next == NULL) {
    printf("%s\n", head -> data);
    return;
  } else {
    printf("%s", head -> data);
    list_print_forward(head -> next);
  }
}

Node * list_remove(Node * head, Node * del) {
  if (head == NULL || del == NULL)
    return NULL;

  else if (head == del) {
    head = del -> next;
  } else if (del -> next == NULL) {
    del -> prev -> next = NULL;
  } else {
    del -> prev -> next = del -> next;
    del -> next -> prev = del -> prev;
  }

  free(del -> data);
  free(del);

  return head;
}

Node * introduce_nod(Node * head, int nr_nod, char * data) {
  Node * new_node = init_node(data);
  if (head == NULL)
    return new_node;
  else {
    Node * tmp = head;
    int i = 1;
    while (i < nr_nod - 1 && tmp != NULL) {
      tmp = tmp -> next;
      i++;
    }
    if (nr_nod == 1) {
      head = list_prepend(head, data);
      return head;
    } else if (tmp -> next == NULL) {
      tmp -> next = new_node;
      new_node -> prev = tmp;
      return head;
    } else {
      new_node -> next = tmp -> next;
      new_node -> prev = tmp;
      tmp -> next -> prev = new_node;
      tmp -> next = new_node;
      return head;
    }
  }
  return head;
}

void delete_list(Node ** head) {
  Node * current;
  while ( * head) {
    current = * head;
    * head = ( * head) -> next;
    free(current -> data);
    free(current);
  }
}

int list_length(Node * head) {
  int nr = 0;
  while (head) {
    head = head -> next;
    nr++;
  }
  return nr;
}