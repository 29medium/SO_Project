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

  for (; aux->prox; aux = aux->prox);

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

    for (; aux->prox && (strcmp(numeroTarefa,aux->numeroTarefa) != 0); aux = aux->prox) {
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
    write(fd, "Não há tarefas em execução\n", 30);

  else{
    Lista aux = l;
    char buffer[100];
    for (; aux; aux = aux->prox) {
      strcpy(buffer,"\n#");
      strcat(buffer,aux->numeroTarefa);
      strcat(buffer,": ");
      strcat(buffer,aux->tarefa);
      write(fd,buffer,strlen(buffer));
    }
  }
}

char* linhaHistorico(int pid,Lista l,int type){
  Lista aux = l;
  char *buffer = malloc(sizeof(char) * 100);

  if(l == NULL){
    write(1,"Sem historico",15);
    strcpy(buffer,"Sem historico");
  }

  else{
    for(;aux && aux -> pid != pid;aux = aux -> prox);
    if(aux != NULL){
      strcpy(buffer,"#");
      strcat(buffer,aux->numeroTarefa);
      if(type == 1)
        strcat(buffer,", terminada: ");
      else if(type == 0)
        strcat(buffer,", concluida: ");
      else if(type == 2)
        strcat(buffer,", max execução: ");
      else
        strcat(buffer,", max inactividade: ");

      strcat(buffer,aux->tarefa);
      strcat(buffer,"\n");
    }
  }
  return buffer;
}


int getPidFromNumeroTarefa(char* numeroTarefa, Lista l) {
  if(!l)
    return -1;

  Lista aux = l;
  for(; aux; aux = aux->prox) {

    if (strcmp(aux->numeroTarefa,numeroTarefa) == 0) {
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

    for (; aux->prox && aux->pid != pid; aux = aux->prox) {
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
