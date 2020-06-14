#include "listar.h"

char* itoa(int i){
    char const digit[] = "0123456789";
    int p = 0,size;
    char *b = malloc(sizeof(char) * 10);
    int shifter = i;

    do{
        p++;
        shifter = shifter/10;
    }while(shifter);

    b[p] = '\0';
    size = p + 1;

    do{
        p--;
        b[p] = digit[i%10];
        i = i/10;
    }while(i);

    return b;
}

Lista adiciona(int pid, int numeroTarefa, char* tarefa,Lista l) {
  Lista aux = l;
  Lista new = malloc(sizeof(struct lista));
  new->pid = pid;
  new->numeroTarefa = numeroTarefa;
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

Lista removeTarefa(int numeroTarefa, Lista l) {
  if (!l) {
    return NULL;
  }
  Lista aux = l;
  if(aux->numeroTarefa == numeroTarefa){
    l = l->prox;
    free(aux);
  }

  else {
    Lista ant = l;
    aux = aux->prox;

    if(!aux) {
      return l;
    }

    for (; aux->prox && aux->numeroTarefa != numeroTarefa; aux = aux->prox) {
      ant = ant->prox;
    }

    if (aux->prox) {
      ant->prox = aux->prox;
      aux->prox = NULL;
      free(aux);
    }
    else {
      if (numeroTarefa == aux->numeroTarefa) {
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
      strcat(buffer,itoa(aux->numeroTarefa));
      strcat(buffer,": ");
      strcat(buffer,aux->tarefa);
      write(fd,buffer,strlen(buffer));
    }
  }
}

char* linhaHistorico(int pid,Lista l,int type){
  Lista aux = l;
  char *buffer = malloc(sizeof(char) * 100);

  for(;aux && aux -> pid != pid;aux = aux -> prox);
  if(aux != NULL){
    strcpy(buffer,"#");
    strcat(buffer,itoa(aux->numeroTarefa));
    if(type == 0)
      strcat(buffer,", concluida: ");
    else if(type == 1)
      strcat(buffer,", terminada: ");
    else if(type == 2)
      strcat(buffer,", max execução: ");
    else
      strcat(buffer,", max inactividade: ");

    strcat(buffer,aux->tarefa);
    strcat(buffer,"\n");
  }

  return buffer;
}


int getPidFromNumeroTarefa(int numeroTarefa, Lista l) {
  if(!l)
    return -1;

  Lista aux = l;
  for(; aux; aux = aux->prox) {

    if (aux->numeroTarefa == numeroTarefa) {
      return aux->pid;
    }
  }

  return -1;
}

int getNumerofromPid(int pid, Lista l) {
  if(!l)
    return -1;

  Lista aux = l;
  for(; aux; aux = aux->prox) {

    if (aux->pid == pid) {
      return aux->numeroTarefa;
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

int containsNum(int numero,Lista l){
  Lista aux = l;

  for(;aux;aux = aux -> prox)
    if(numero == aux->numeroTarefa){
      return 1;
    }

  return 0;
}
