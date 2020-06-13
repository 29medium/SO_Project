#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

typedef struct lista {
  int pid;
  char* numeroTarefa;
  char* tarefa;
  struct lista *prox;
} *Lista;


Lista adiciona(int pid, char* numeroTarefa, char* tarefa, Lista l);
Lista removeTarefa(char* numeroTarefa, Lista l);
void printLista(Lista l, int fd);
int getPidFromNumeroTarefa(char* numeroTarefa, Lista l);
Lista removePid(int pid, Lista l);
char* linhaHistorico(int pid,Lista l,int type);
int containsNum(char *numero,Lista l);
