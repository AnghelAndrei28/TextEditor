#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cursor {
  int linie;
  int pozitie;
}
Cursor;

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
    printf("%s", head->data);
    fprintf(outputFile, "%s", head -> data);
    return;
  } else {
    printf("%s", head->data);
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

#define BUF_MAX 100

int main() {

  FILE * entryFile;
  FILE * outputFile;

  int command_enabled = 0;
  char * buffer = (char * ) calloc(BUF_MAX, sizeof(char));

  Cursor.linie = 1;
  Cursor.pozitie = 0;

  entryFile = fopen("editor.in", "r");
  if (entryFile == NULL)
    printf("Nu s-a putut deschide fisierul entry");
  outputFile = fopen("editor.out", "w");
  if (outputFile == NULL)
    printf("Nu s-a putut deschide fisierul output");
  fgets(buffer, BUF_MAX + 2, entryFile);
  Node * head = init_node(buffer);
  Node * saved_head = init_node(buffer);
  while (fgets(buffer, BUF_MAX + 2, entryFile) != NULL) {
    if (command_enabled) {
      if (strcmp(buffer, "::i\n") == 0 || strcmp(buffer, "::i \n") == 0) {
        command_enabled = 0;
      } else if (strcmp(buffer, "q\n") == 0) {
        quit_command(outputFile, saved_head);
      } else if (strcmp(buffer, "r\n") == 0) {
        printf("Redo\n");
      } else if (strcmp(buffer, "u\n") == 0) {
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
      } else {
        head = list_append(head, buffer);

      }
    }
  }

  delete_list( & saved_head);
  delete_list( & head);
  free(buffer);
  fclose(entryFile);
  fclose(outputFile);

  return 0;
}