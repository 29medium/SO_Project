#include "listar.h"

Lista adiciona(int pid, char* numeroTarefa, char* tarefa,Lista l) {
  Lista aux = l;
  Lista new = malloc(sizeof(struct lista));
  new->pid = pid;
  new->numeroTarefa = malloc(sizeof(char) * strlen(numeroTarefa));
  strcpy(new->numeroTarefa,numeroTarefa);
  new->tarefa = malloc(sizeof(char) * strlen(tarefa));
  strcpy(new->tarefa,tarefa);
  new->prox = NULL;

  if (!l) {
    return new;
  }

  for (aux; aux->prox; aux = aux->prox);

  aux->prox = new;

  return l;
}

Lista removeTarefa(char* numeroTarefa, Lista l) {
  if (!l) {
    return NULL;
  }
  Lista aux = l;
  if(strcmp(numeroTarefa,aux->numeroTarefa) == 0){
    l = l->prox;
    free(aux);
  }

  else {
    Lista ant = l;
    aux = aux->prox;

    if(!aux) {
      return l;
    }

    for (aux; aux->prox && (strcmp(numeroTarefa,aux->numeroTarefa) != 0); aux = aux->prox) {
      ant = ant->prox;
    }

    if (aux->prox) {
      ant->prox = aux->prox;
      aux->prox = NULL;
      free(aux);
    }
    else {
      if (strcmp(numeroTarefa,aux->numeroTarefa) == 0) {
        ant->prox = NULL;
        free(aux);
      }
    }
  }

  return l;
}

void printLista(Lista l, int fd) {
  if(!l)
    write(fd, "Não há tarefas em execução\n", 27);

  Lista aux = l;

  for (aux; aux; aux = aux->prox) {
    write(fd,"\n#" , 2);
    write(fd, aux->numeroTarefa, strlen(aux->numeroTarefa));
    write(fd, ": ", 2);
    write(fd, aux->tarefa, strlen(aux->tarefa));
  }
}

int getPidFromNumeroTarefa(char* numeroTarefa, Lista l) {
  if(!l)
    return -1;

  Lista aux = l;
  for(aux; aux; aux = aux->prox) {
    if (strcmp(numeroTarefa,aux->numeroTarefa) == 0) {
      return aux->pid;
    }
  }

  return -1;
}

Lista removePid(int pid, Lista l) {
  if (!l) {
    return NULL;
  }
  Lista aux = l;
  if(aux->pid == pid){
    l = l->prox;
    free(aux);
  }

  else {
    Lista ant = l;
    aux = aux->prox;

    if(!aux) {
      return l;
    }

    for (aux; aux->prox && aux->pid != pid; aux = aux->prox) {
      ant = ant->prox;
    }

    if (aux->prox) {
      ant->prox = aux->prox;
      aux->prox = NULL;
      free(aux);
    }
    else {
      if (aux->pid == pid) {
        ant->prox = NULL;
        free(aux);
      }
    }
  }

  return l;
}
