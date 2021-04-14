#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "list.h"

#define BUF_MAX 100

struct cursor {
  int linie;
  int pozitie;
}
Cursor;

SNode * undo_stack = NULL;
SNode * redo_stack = NULL;

//Implementare comanda quit
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

/*Implementare comanda save, care copiaza nod cu nod dintr-o lista de lucru in
  cea salvata */
Node * save_command(Node * head, Node * saved_head) {
  Node * tmp1 = head;
  Node * tmp2 = saved_head;
  while (tmp1 && tmp2) {
    tmp2 -> data = (char * ) realloc(tmp2 -> data,
      (strlen(tmp1 -> data) + 1) * sizeof(char));
    strcpy((char * ) tmp2 -> data, (char * ) tmp1 -> data);
    tmp1 = tmp1 -> next;
    tmp2 = tmp2 -> next;
  }
  if (tmp1 == NULL && tmp2 != NULL) {
    while (tmp2) {
      Node * tmp = tmp2 -> next;
      saved_head = list_remove(saved_head, tmp2);
      tmp2 = tmp;
    }
  } else if (tmp1 != NULL && tmp2 == NULL) {
    while (tmp1) {
      Node * tmp = tmp1 -> next;
      saved_head = list_append(saved_head, tmp1 -> data);
      tmp1 = tmp;
    }
  }
  delete_stack( & redo_stack);
  delete_stack( & undo_stack);
  return saved_head;
}

/*Implementare comanda de stergere, care sterge primul caracter de dinainte 
  cursor*/
Node * delete_command(Node * head, int nr_characters) {

  Node * tmp = head;
  int i;
  for (i = 1; i < Cursor.linie; i++) {
    tmp = tmp -> next;
  }

  char * string = (char * ) calloc(strlen(tmp -> data) - nr_characters + 1,
    sizeof(char));
  strncpy(string, tmp -> data, Cursor.pozitie);
  strcat(string, tmp -> data + (Cursor.pozitie + nr_characters));
  tmp -> data = (char * ) realloc(tmp -> data,
    (strlen(string) + 1) * sizeof(char));
  strcpy(tmp -> data, string);

  free(string);
  return head;
}

//Implementare comanda de backspace, care sterge primul caracter de dupa cursor
Node * backspace_command(Node * head) {
  Node * tmp = head;
  int i;
  for (i = 1; i < Cursor.linie; i++) {
    tmp = tmp -> next;
  }

  char * string = (char * ) calloc(strlen(tmp -> data) + 1, sizeof(char));
  strncpy(string, tmp -> data, Cursor.pozitie - 1);
  strcat(string, tmp -> data + (Cursor.pozitie));
  tmp -> data = (char * ) realloc(tmp -> data,
    (strlen(string) + 1) * sizeof(char));
  strcpy(tmp -> data, string);

  free(string);

  Cursor.pozitie--;
  return head;
}

//Implementare comanda stergere linie, care sterge nodurile specifice liniei
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

/*Implementare comanda "go to char", care schimba valorile Cursorului cu 
  linia si pozitia noua*/
void go_to_char(Node * head, char * buffer) {
  strtok(buffer, " ");
  Cursor.pozitie = atoi(strtok(NULL, " "));
  char * x = strtok(NULL, " ");
  if (x != NULL) {
    Cursor.linie = atoi(x);
  }
}

/*Implementare comanda "go to line", care schimba valorile Cursorului cu 
  linia si pozitia 0*/
void go_to_line(Node * head, char * buffer) {
  int x = atoi(buffer + 3);
  Cursor.linie = x;
  Cursor.pozitie = 0;
}

/*Implementare comanda de undo, in care se face pop la ultimul nod din stiva
  de undo si push pe stiva de redo, facand operatia specifica stringului din
  acesta*/
Node * undo_command(Node * head) {
  int i;
  if (!is_empty(undo_stack)) {
    char * redo;
    if (strncmp(top(undo_stack), "Ndel", 4) == 0) {
      redo = (char * ) calloc(5, sizeof(char));
      strcpy(redo, "text\0");
      char * n;
      char * nodes = top(undo_stack);
      strtok(nodes, " ");
      n = strtok(NULL, " ");
      while (n != NULL) {
        Node * tmp = head;
        for (i = 1; i <= list_length(head); i++) {
          if (i == atoi(n)) {
            redo = (char * ) realloc(redo, (strlen(redo) + strlen(n) +
              strlen(tmp -> data) + 3));
            strcat(redo, " ");
            strcat(redo, n);
            strcat(redo, " ");
            strcat(redo, tmp -> data);
            head = list_remove(head, tmp);
            break;
          }
          tmp = tmp -> next;
        }

        n = strtok(NULL, " ");
      }

    }
    redo_stack = push(redo_stack, redo);
    undo_stack = pop(undo_stack);
    free(redo);
  }
  return head;
}

/*Implementare comanda de redo, in care se face pop la ultimul nod din stiva
  de redo si push pe stiva de undo, facand operatia specifica stringului din
  acesta*/
Node * redo_command(Node * head) {
  if (!is_empty(redo_stack)) {
    if (strncmp(top(redo_stack), "text", 4) == 0) {
      char * nr_nod;
      char * text;
      char * redo = top(redo_stack);
      strtok(redo, " ");
      nr_nod = strtok(NULL, " ");
      text = strtok(NULL, " ");
      while (nr_nod != NULL) {
        head = introduce_nod(head, atoi(nr_nod), text);
        nr_nod = strtok(NULL, " ");
        text = strtok(NULL, " ");
      }

    }
    redo_stack = pop(redo_stack);
  }
  return head;
}

//Functie care verifica numarul de linii de text
int nr_linii(Node * head) {
  int nr = 0;
  Node * tmp = head;
  while (tmp) {
    if (strstr(tmp -> data, "\n") != NULL) {
      nr++;
    }
    tmp = tmp -> next;
  }
  return nr;
}

//Functie care verifica fiecare nod daca este gol si in caz pozitiv il sterge
Node * check_empty_node(Node * head) {
  Node * tmp = head;
  while (tmp) {
    if (strcmp(tmp -> data, "\n") == 0 || tmp -> data == NULL) {
      Node * tmp2 = tmp -> next;
      head = list_remove(head, tmp);
      tmp = tmp2;
      continue;
    }
    tmp = tmp -> next;
  }
  return head;
}

//Functie care returneaza noul string pentru fiecare nod
char * replace_word(char * string, char * old_word, char * new_word) {
  char * new_string;
  int i, nr_aparitii = 0;
  int newlen = strlen(new_word);
  int oldlen = strlen(old_word);

  for (i = 0; string[i] != '\0'; i++) {
    if (strstr( & string[i], old_word) == & string[i]) {
      nr_aparitii++;
      i += oldlen - 1;
    }
  }

  new_string = (char * ) malloc(i + nr_aparitii * (newlen - oldlen) + 1);

  i = 0;
  while ( * string) {

    if (strstr(string, old_word) == string) {
      strcpy( & new_string[i], new_word);
      i += newlen;
      string += oldlen;
    } else
      new_string[i++] = * string++;
  }

  new_string[i] = '\0';
  return new_string;
}

/*Implementare comanda de stergere a unui cuvant, in care se cauta prima
  aparitie a unui cuvant, si este sters*/
Node * delete_word(Node * head, char * word) {
  char * new_string;
  Node * tmp = head;
  for (int i = 1; i < Cursor.linie; i++) {
    tmp = tmp -> next;
  }
  if (strstr( & tmp -> data[Cursor.pozitie], word) != NULL) {
    char * ptr = strstr( & tmp -> data[Cursor.pozitie], word);
    int x = ptr - tmp -> data;
    new_string = (char * ) calloc(strlen(tmp -> data) - strlen(word) + 1,
      sizeof(char));
    strncpy(new_string, tmp -> data, x);
    Cursor.pozitie = strlen(new_string);
    strcat(new_string, tmp -> data + x + strlen(word));
  }
  tmp -> data = (char * ) realloc(tmp -> data,
    (strlen(new_string) + 1) * sizeof(char));
  strcpy(tmp -> data, new_string);
  free(new_string);
  head = check_empty_node(head);
  return head;
}

//Implementare comanda de stergere a tuturor aparitiilor unui cuvant
Node * delete_all(Node * head, char * word) {
  char * new_string = (char * ) calloc(1, sizeof(char));
  Node * tmp = head;
  for (int i = 1; i < Cursor.linie; i++) {
    tmp = tmp -> next;
  }
  /*Se verifica randul la care se afla prima data cursorul, astfel incat
  sa nu fie verificat in totalitate la nevoie*/
  if (strstr( & tmp -> data[Cursor.pozitie], word) != NULL) {
    char * ptr = strstr( & tmp -> data[Cursor.pozitie], word);
    int x = ptr - tmp -> data;
    new_string = (char * ) realloc(new_string,
      (strlen(tmp -> data) - strlen(word) + 1) * sizeof(char));
    strncpy(new_string, tmp -> data, x);
    Cursor.pozitie = strlen(new_string);
    strcat(new_string, tmp -> data + x + strlen(word));
    tmp -> data = (char * ) realloc(tmp -> data,
      (strlen(new_string) + 1) * sizeof(char));
    strcpy(tmp -> data, new_string);
    Cursor.pozitie++;
  }
  //Sunt verificate celelalte linii
  tmp = tmp -> next;
  while (tmp) {
    char * string = replace_word(tmp -> data, word, "");
    tmp -> data = (char * ) realloc(tmp -> data,
      (strlen(string) + 1) * sizeof(char));
    strcpy(tmp -> data, string);
    tmp = tmp -> next;
    free(string);
  }
  head = check_empty_node(head);
  free(new_string);
  return head;
}

//Implementare comanda de inlocuire a tuturor aparitiilor unui cuvant
Node * replace_all(Node * head, char * new_word, char * old_word) {
  Node * tmp = head;
  for (int i = 1; i < Cursor.linie; i++) {
    tmp = tmp -> next;
  }
  /*Se verifica randul la care se afla prima data cursorul, astfel incat
  sa nu fie verificat in totalitate la nevoie*/
  if (strstr( & tmp -> data[Cursor.pozitie], old_word) != NULL) {
    char * new_string = (char * ) calloc(Cursor.pozitie + 1, sizeof(char));
    strncpy(new_string, tmp -> data, Cursor.pozitie);
    char * string = replace_word(tmp -> data + Cursor.pozitie,
      old_word, new_word);
    new_string = (char * ) realloc(new_string,
      (strlen(new_string) + strlen(string) + 1) * sizeof(char));
    strcat(new_string, string);
    tmp -> data = (char * ) realloc(tmp -> data,
      (strlen(new_string) + 1) * sizeof(char));
    strcpy(tmp -> data, new_string);
    free(string);
    free(new_string);
  }
  //Sunt verificate celelalte linii
  tmp = tmp -> next;
  while (tmp) {
    //Se verifica continutul fiecarui nod si se obtine noul continut
    char * string = replace_word(tmp -> data, old_word, new_word);
    tmp -> data = (char * ) realloc(tmp -> data,
      (strlen(string) + 1) * sizeof(char));
    strcpy(tmp -> data, string);
    tmp = tmp -> next;
    free(string);
  }
  return head;
}

//Implementare comanda de inlocuire a primei aparitii a unui cuvant
Node * replace_only_a_word(Node * head, char * old_word, char * new_word) {
  char * new_string;
  int length = strlen(old_word) - strlen(new_word);
  Node * tmp = head;
  for (int i = 1; i < Cursor.linie; i++) {
    tmp = tmp -> next;
  }
  if (strstr( & tmp -> data[Cursor.pozitie], old_word) == NULL) {
    tmp = tmp -> next;
    while (tmp) {
      Cursor.linie++;
      if (strstr(tmp -> data, old_word) != NULL) {
        char * ptr = strstr(tmp -> data, old_word);
        int x = ptr - tmp -> data;
        //Se verifica daca textul cautat se afla la inceputul stringului
        if (ptr - tmp -> data == 0) {
          new_string = (char * ) calloc(strlen(tmp -> data) + length + 1,
            sizeof(char));
          strcpy(new_string, new_word);
          Cursor.pozitie = strlen(new_string);
          strcat(new_string, tmp -> data + strlen(old_word));
        } else {
          new_string = (char * ) calloc(strlen(tmp -> data) + length + 1,
            sizeof(char));
          strncpy(new_string, tmp -> data, ptr - tmp -> data);
          strcat(new_string, new_word);
          Cursor.pozitie = strlen(new_string);
          strcat(new_string, tmp -> data + strlen(old_word) + x);
        }
        tmp -> data = (char * ) realloc(tmp -> data,
          (strlen(new_string) + 1) * sizeof(char));
        strcpy(tmp -> data, new_string);
        break;
      }
      tmp = tmp -> next;
    }
  }
  free(new_string);
  return head;
}

int main() {

  FILE * entryFile;
  FILE * outputFile;

  //Contor daca se scriu comenzi sau text
  int command_enabled = 0;
  char * buffer = (char * ) calloc(BUF_MAX, sizeof(char));
  char * nod_dels = (char * ) calloc(7, sizeof(char));
  char nod[4];
  strcpy(nod_dels, "Ndel 1\0");
  //Initializare cursor
  Cursor.linie = 1;
  Cursor.pozitie = 0;

  entryFile = fopen("editor.in", "r");
  if (entryFile == NULL)
    printf("Nu s-a putut deschide fisierul entry");
  outputFile = fopen("editor.out", "w");
  if (outputFile == NULL)
    printf("Nu s-a putut deschide fisierul output");
  //Extrag prima linie din fisierul de input
  fgets(buffer, BUF_MAX, entryFile);
  //Initializare liste de lucru si save
  Node * head = init_node(buffer);
  Node * saved_head = init_node(buffer);
  Cursor.linie++;
  //Incep extragerea celorlalte linii din fisierul de input
  while (fgets(buffer, BUF_MAX, entryFile) != NULL) {
    if (command_enabled) {
      //Verific daca se schimba contorul pentru a scrie text
      if (strcmp(buffer, "::i\n") == 0 || strcmp(buffer, "::i \n") == 0) {
        nod_dels = (char * ) realloc(nod_dels, 5 * sizeof(char));
        //Se creeaza stringul care va fi trimis in nodul de undo
        strcpy(nod_dels, "Ndel\0");
        command_enabled = 0;
      }
      //Se verifica ce comanda se executa
      else if (strcmp(buffer, "q\n") == 0) {
        quit_command(outputFile, saved_head);
      } else if (strcmp(buffer, "r\n") == 0) {
        head = redo_command(head);
      } else if (strcmp(buffer, "u\n") == 0) {
        head = undo_command(head);
      } else if (strcmp(buffer, "s\n") == 0) {
        saved_head = save_command(head, saved_head);
      } else if (strncmp(buffer, "dl", 2) == 0) {
        head = delete_line(head, buffer);
      } else if (strncmp(buffer, "gc", 2) == 0) {
        go_to_char(head, buffer);
      } else if (strncmp(buffer, "gl", 2) == 0) {
        go_to_line(head, buffer);
      } else if (strcmp(strtok(buffer, " "), "d") == 0 ||
        strcmp(buffer, "d\n") == 0) {
        if (strcmp(buffer, "d\n") == 0) {
          head = delete_command(head, 1);
        } else {
          head = delete_command(head, atoi(strtok(NULL, " ")));
        }
        head = check_empty_node(head);
      } else if (strcmp(buffer, "b\n") == 0) {
        head = backspace_command(head);
        head = check_empty_node(head);
      } else if (strncmp(buffer, "re", 2) == 0) {
        char * old_word = strtok(NULL, " \n");
        char * new_word = strtok(NULL, " \n");
        head = replace_only_a_word(head, old_word, new_word);
      } else if (strncmp(buffer, "dw", 2) == 0) {
        char * word = strtok(NULL, " \n");
        head = delete_word(head, word);
      } else if (strncmp(buffer, "da", 2) == 0) {
        char * word = strtok(NULL, " \n");
        head = delete_all(head, word);
      } else if (strncmp(buffer, "ra", 2) == 0) {
        char * old_word = strtok(NULL, " \n");
        char * new_word = strtok(NULL, " \n");
        head = replace_all(head, new_word, old_word);
      }
    } else if (!command_enabled) {
      //Se verifica daca incep sa scriu comenzi
      if (strcmp(buffer, "::i\n") == 0 || strcmp(buffer, "::i \n") == 0) {
        command_enabled = 1;
        /*Se incheie perioada de scriere text si se trimit nodurile nou
        adaugate in stiva de undo*/
        undo_stack = push(undo_stack, nod_dels);
      } else {
        /*Daca cursorul se afla la finalul textului, se introduce nodul la
          finalul listei*/
        if (Cursor.linie == nr_linii(head) + 1) {
          head = list_append(head, buffer);
          Cursor.linie++;
          Cursor.pozitie = 0;
          sprintf(nod, " %d", Cursor.linie - 1);
          nod_dels = (char * ) realloc(nod_dels,
            (strlen(nod_dels) + strlen(nod) + 1) * sizeof(char));
          strcat(nod_dels, nod);
        } else {
          //Se verifica daca se adauga text la inceputul randului
          if (Cursor.pozitie == 0) {
            Node * tmp = head;
            int nr_line = 0;
            int nr_nod = 0;
            while (tmp) {
              nr_nod++;
              if (strstr(tmp -> data, "\n") != NULL) {
                nr_line++;
              }
              if (nr_line == Cursor.linie) {
                break;
              }
              tmp = tmp -> next;
            }
            char * buffer2 = (char * ) calloc(strlen(buffer), sizeof(char));
            strncpy(buffer2, buffer, strlen(buffer) - 1);
            head = introduce_nod(head, nr_nod + 1, buffer2);
            free(buffer2);
          } else {
            Node * tmp = head;
            int nr_line = 0;
            int nr_nod = 0;
            //Se cauta primul nod al liniei cursorului
            while (tmp && (nr_line < Cursor.linie)) {
              nr_nod++;
              if (strstr(tmp -> data, "\n") != NULL) {
                nr_line++;
              }
              tmp = tmp -> next;
            }
            tmp = tmp -> prev;
            /*Se verifica daca s-a ajuns la nodul in continutul caruia se
              adauga noul nod. In acest caz, nodul curent va fi impartit
              in 2 noduri si va fi adaugat un al treilea nod intre acestea*/
            if (Cursor.pozitie < strlen(tmp -> data)) {
              /*"data" este stringul de dinainte de Cursor, iar "data2"
                stringul de dupa cursor.*/
              char * data = (char * ) calloc(Cursor.pozitie + 1, sizeof(char));
              char * data2 = (char * ) calloc(strlen(tmp -> data) -
                Cursor.pozitie + 1, sizeof(char));
              strncpy(data, tmp -> data, Cursor.pozitie);
              strncpy(data2, tmp -> data + Cursor.pozitie,
                strlen(tmp -> data) - Cursor.pozitie);
              tmp -> data = (char * ) realloc(tmp -> data,
                (strlen(data) + 1) * sizeof(char));
              //Nodul actual va avea stringul de dinainte de cursor
              strcpy(tmp -> data, data);
              //"buffer2" este stringul adaugat fara caracterul "\n"
              char * buffer2 = (char * ) calloc(strlen(buffer), sizeof(char));
              strncpy(buffer2, buffer, strlen(buffer) - 1);
              /*Se introduc nodurile care contin noul text adaugat, respectiv
              stringul de dupa cursor*/
              head = introduce_nod(head, nr_nod + 1, buffer2);
              head = introduce_nod(head, nr_nod + 2, data2);
              free(data);
              free(data2);
              free(buffer2);
            }
          }
        }
      }
    }
  }
  //Dezalocare finala a memoriei ramase
  delete_list( & saved_head);
  delete_list( & head);
  delete_stack( & undo_stack);
  delete_stack( & redo_stack);
  free(buffer);
  free(nod_dels);
  fclose(entryFile);
  fclose(outputFile);

  return 0;
}