#ifndef __listar_h
#define __listar_h

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

typedef struct lista {
  int pid;
  int numeroTarefa;
  char* tarefa;
  struct lista *prox;
} *Lista;


Lista adiciona(int pid, int numeroTarefa, char* tarefa, Lista l);
Lista removeTarefa(int numeroTarefa, Lista l);
void printLista(Lista l, int fd);
int getPidFromNumeroTarefa(int numeroTarefa, Lista l);
Lista removePid(int pid, Lista l);
char* linhaHistorico(int pid,Lista l,int type);
int containsNum(int numero,Lista l);
char* itoa(int i);
int getNumerofromPid(int pid, Lista l);

#endif
