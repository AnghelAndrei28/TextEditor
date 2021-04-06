#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define BUF_MAX 100

struct cursor {
  int linie;
  int pozitie;
}
Cursor;

SNode* undo_stack = NULL;
SNode* redo_stack = NULL;

typedef struct node {
  char * data;
  struct node * next;
  struct node * prev;
}
Node;

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

Node* list_prepend(Node* head, char* data)
{
    Node* new_node = init_node(data);

    if (head == NULL)
    {
        return new_node;
    }
    else
    {
        head->prev = new_node;
        new_node->next = head;
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

  if (head == del) {
    head = del -> next;
  }

  if (del -> next != NULL) {
    del -> next -> prev = del -> prev;
  }

  if (del -> prev != NULL) {
    del -> prev -> next = del -> next;
  }

  free(del -> data);
  free(del);

  return head;
}

Node* introduce_nod(Node* head, int nr_nod, char* data) {
  Node * new_node = init_node(data);
  if (head == NULL)
    return new_node;
  else {
    Node* tmp = head;
    int i = 1;
    while (i < nr_nod - 1 && tmp != NULL)
    {
      tmp = tmp->next;
      i++;
    }
    if(nr_nod == 1) {
      head = list_prepend(head, data);
      return head;
    } else if(tmp->next == NULL) {
      head = list_append(head, data);
      return head;
    } else if(tmp != NULL) {
      new_node->next = tmp->next;
      new_node->prev = tmp;
      if(tmp->next != NULL) {
        tmp->next->prev = new_node;
      }
      tmp->next = new_node;
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

void quit_command(FILE * outputFile, Node * head) {
  if (head == NULL) {
    return;
  } else if (head -> next == NULL) {
    fprintf(outputFile, "%s", head -> data);
    return;
  } else {
    fprintf(outputFile, "%s", head -> data);
    quit_command(outputFile, head -> next);
  }
}

Node * save_command(Node * head, Node * saved_head) {
  Node * tmp1 = head;
  Node * tmp2 = saved_head;
  while (tmp1 && tmp2) {
    tmp2 -> data = (char * ) realloc(tmp2 -> data, (strlen(tmp1 -> data) + 1) * sizeof(char));
    strcpy((char * ) tmp2 -> data, (char * ) tmp1 -> data);
    tmp1 = tmp1 -> next;
    tmp2 = tmp2 -> next;
  }
  if (tmp1 == NULL && tmp2 != NULL) {
    while (tmp2) {
      saved_head = list_remove(saved_head, tmp2);
      tmp2 = tmp2 -> next;
    }
  } else if (tmp1 != NULL && tmp2 == NULL) {
    while (tmp1) {
      saved_head = list_append(saved_head, tmp1 -> data);
      tmp1 = tmp1 -> next;
    }
  }

  return saved_head;
}

Node * delete_command(Node * head, int nr_characters) {

  Node * tmp = head;
  int i;
  for (i = 1; i < Cursor.linie; i++) {
    tmp = tmp -> next;
  }

  char * string = (char * ) calloc(strlen(tmp -> data) - nr_characters + 1, sizeof(char));
  strncpy(string, tmp -> data, Cursor.pozitie);
  strcat(string, tmp -> data + (Cursor.pozitie + nr_characters));
  tmp -> data = (char * ) realloc(tmp -> data, (strlen(string) + 1) * sizeof(char));
  strcpy(tmp -> data, string);

  free(string);
  return head;
}

Node * backspace_command(Node * head) {
  Node * tmp = head;
  int i;
  for (i = 1; i < Cursor.linie; i++) {
    tmp = tmp -> next;
  }

  char * string = (char * ) calloc(strlen(tmp -> data) + 1, sizeof(char));
  strncpy(string, tmp -> data, Cursor.pozitie - 1);
  strcat(string, tmp -> data + (Cursor.pozitie));
  tmp -> data = (char * ) realloc(tmp -> data, (strlen(string) + 1) * sizeof(char));
  strcpy(tmp -> data, string);

  free(string);

  Cursor.pozitie--;

  return head;
}

Node * delete_line(Node * head, char * buffer) {
  int i;
  if (buffer[2] == '\n') {
    Node * tmp = head;
    for (i = 1; i < Cursor.linie; i++) {
      tmp = tmp -> next;
    }
    head = list_remove(head, tmp);
  } else {
    Node * tmp = head;
    int x = atoi(buffer + 3);
    Cursor.linie = x;
    Cursor.pozitie = 0;
    for (i = 1; i < Cursor.linie; i++) {
      tmp = tmp -> next;
    }
    head = list_remove(head, tmp);
  }
  return head;
}

void go_to_char(Node * head, char * buffer) {
  strtok(buffer, " ");
  Cursor.pozitie = atoi(strtok(NULL, " "));
  char * x = strtok(NULL, " ");
  if (x != NULL) {
    Cursor.linie = atoi(x);
  }
}

void go_to_line(Node * head, char * buffer) {
  int x = atoi(buffer + 3);
  Cursor.linie = x;
  Cursor.pozitie = 0;
}

int list_length(Node* head) {
  int nr = 0;
  while(head) {
    head = head->next;
    nr++;
  }
  return nr;
}

int main() {

  FILE * entryFile;
  FILE * outputFile;

  int command_enabled = 0;
  char * buffer = (char * ) calloc(BUF_MAX, sizeof(char));
  char * nod_dels = (char *)calloc(7, sizeof(char));
  char nod[4];
  strcpy(nod_dels, "Ndel 1\0");
  Cursor.linie = 1;
  Cursor.pozitie = 0;

  entryFile = fopen("editor.in", "r");
  if (entryFile == NULL)
    printf("Nu s-a putut deschide fisierul entry");
  outputFile = fopen("editor.out", "w");
  if (outputFile == NULL)
    printf("Nu s-a putut deschide fisierul output");
  fgets(buffer, BUF_MAX, entryFile);
  Node * head = init_node(buffer);
  Node * saved_head = init_node(buffer);
  while (fgets(buffer, BUF_MAX, entryFile) != NULL) {
    if (command_enabled) {
      if (strcmp(buffer, "::i\n") == 0 || strcmp(buffer, "::i \n") == 0) {
        nod_dels = (char *) realloc(nod_dels, 5 * sizeof(char));
        strcpy(nod_dels, "Ndel\0");
        Cursor.linie = list_length(head);
        command_enabled = 0;
      } else if (strcmp(buffer, "q\n") == 0) {
        quit_command(outputFile, saved_head);
      } else if (strcmp(buffer, "r\n") == 0) {
        if (!is_empty(redo_stack)) {
          if(strncmp(top(redo_stack), "text", 4) == 0) {
            printf("--------------");
            char* nr_nod;
            char* text;
            char* redo = top(redo_stack);
            printf("%s\n", strtok(redo, " "));
            nr_nod = strtok(NULL, " ");
            printf("%s\n", nr_nod);
            text = strtok(NULL, " ");
            printf("%s\n", text);
            while (nr_nod != NULL)
            {
              head = introduce_nod(head, atoi(nr_nod), text);
              nr_nod = strtok(NULL, " ");
              text = strtok(NULL, " ");
            }
            
          }
          redo_stack = pop(redo_stack);
        }
        printf("Redo\n");
      } else if (strcmp(buffer, "u\n") == 0) {
        if (!is_empty(undo_stack))
        {
          char* redo;
          if(strncmp(top(undo_stack), "Ndel", 4) == 0) {
            redo = (char *)calloc(5, sizeof(char));
            strcpy(redo, "text\0");
            printf("Am ajuns aici si e bine\n");
            char* n;
            char* nodes = top(undo_stack);
            printf("%s\n", nodes);
            printf("%s\n", strtok(nodes, " "));
            n = strtok(NULL, " ");
            printf("%s\n", n);
            while (n != NULL) {
              printf("%s\n", n);
              Node* tmp = head;
              for (int i = 1; i <= list_length(head); i++){
                if(i == atoi(n)) {
                  redo = (char *)realloc(redo, (strlen(redo) + strlen(n) + strlen(tmp->data) + 3));
                  strcat(redo, " ");
                  strcat(redo, n);
                  strcat(redo, " ");
                  strcat(redo, tmp->data);
                  head = list_remove(head, tmp);
                  break;
                }
                tmp = tmp->next;
              }

              n = strtok(NULL, " ");
            }

          }
        redo_stack = push(redo_stack, redo);
        undo_stack = pop(undo_stack);
        free(redo);
        }
        printf("Undo\n");
      } else if (strcmp(buffer, "s\n") == 0) {
        saved_head = save_command(head, saved_head);
      } else if (strncmp(buffer, "dl", 2) == 0) {
        head = delete_line(head, buffer);
      } else if (strncmp(buffer, "gc", 2) == 0) {
        go_to_char(head, buffer);
      } else if (strncmp(buffer, "gl", 2) == 0) {
        go_to_line(head, buffer);
      } else if (strcmp(strtok(buffer, " "), "d") == 0 || strcmp(buffer, "d\n") == 0) {
        if (strcmp(buffer, "d\n") == 0) {
          head = delete_command(head, 1);
        } else {
          head = delete_command(head, atoi(strtok(NULL, " ")));
        }
      } else if (strcmp(buffer, "b\n") == 0) {
        head = backspace_command(head);
      } else {
        //printf("Unhandled case\n");
      }
    } else if (!command_enabled) {
      if (strcmp(buffer, "::i\n") == 0 || strcmp(buffer, "::i \n") == 0) {
        command_enabled = 1;
        undo_stack = push(undo_stack, nod_dels);
      } else {
        head = list_append(head, buffer);
        Cursor.linie++;
        Cursor.pozitie = 0;
        sprintf(nod, " %d", Cursor.linie);
        nod_dels = (char*)realloc(nod_dels, (strlen(nod_dels) + strlen(nod) + 1) * sizeof (char));
        strcat(nod_dels, nod);
      }
    }
  }
  print_stack(undo_stack);
  print_stack(redo_stack);
  // printf("%s", top(undo_stack));
  delete_list( & saved_head);
  delete_list( & head);
  delete_stack(& undo_stack);
  delete_stack(& redo_stack);
  free(buffer);
  free(nod_dels);
  fclose(entryFile);
  fclose(outputFile);

  return 0;
}