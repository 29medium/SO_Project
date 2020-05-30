#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "executar.h"

int maxTime = -1;
int tarefa = 1;

void ajuda() {
    char help[] = "\ntempo-inactividade segs\ntempo-execucao segs\nexecutar p1 | p2 ... | pn\nlistar\nterminar #num\nhistorico\najuda\n\n";
    write(1, help, sizeof(help));
}

int itoa(int i, char b[]){
    char const digit[] = "0123456789";
    int p = 0,size;
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
    return size;
}

int main(){
  int fdrd,fdwr, n,r = 1,i = 0,arg = 0,exec = 0;
  char c = '\0';
  char numero[10];
  char comand[5][15];

  if(mkfifo("pipeClienteServidor",0666)<0){
		perror("pipe1");
		return 1;
	}

  if(mkfifo("pipeServidorCliente",0666)<0){
		perror("pipe2");
		return 1;
	}

  if((fdrd = open("pipeClienteServidor",O_RDONLY))< 0){
    perror("open pipe1");
    return 1;
  }

  if((fdwr = open("pipeServidorCliente",O_WRONLY))< 0){
    perror("open pipe2");
    return 1;
  }

  dup2(fdrd,0);
  dup2(fdwr,1);
  close(fdrd);
  close(fdwr);

    while((n = read(0,&c,sizeof(char)))>0 && r){
      if(c == '\0'){
        comand[arg][i] = '\0';

        if(strcmp(comand[0], "executar") == 0) {
          r = itoa(tarefa,numero);
          write(1,"nova tarefa #",13);
          write(1,numero,r);
          tarefa++;

          if(!fork()){
            executar(comand[1],maxTime);
            _exit(1);
          }
        }

        else if(strcmp(comand[0], "ajuda") == 0){
          if(!fork()){
            ajuda();
            _exit(1);
          }
        }

        else if(strcmp(comand[0], "tempo-execucao") == 0){
            maxTime = atoi(comand[1]);
        }

        else if(strcmp(comand[0], "sair") == 0)
          r=0;

        arg = 0;
        i = 0;
        exec = 0;
      }

      else if(c == 39)
        exec = 1;

      else if(exec == 1 || c != ' '){
        comand[arg][i] = c;
        i++;
      }

      else if(c == ' '){
        comand[arg][i] = '\0';
        arg++;
        i = 0;
      }
    }
  unlink("pipeClienteServidor");
  unlink("pipeServidorCliente");
}
