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

void sig_handler(int signum){
  unlink("pipeClienteServidor");
  unlink("pipeServidorCliente");

  write(1,"\nServer off!\n",15);
  _exit(1);
}

void ajuda(int fdwr) {
    char help[] = "\ntempo-inactividade segs\ntempo-execucao segs\nexecutar p1 | p2 ... | pn\nlistar\nterminar #num\nhistorico\najuda\n\n";
    write(fdwr, help, sizeof(help));
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

void separaString(char* buffer,char comand[2][100]){
  int i = 0,arg = 0,c = 0,exec = 0;
  for(;buffer[i] != '\n' && exec != 2 && buffer[i] != '\0';i++){
    if(arg == 1){
      if(buffer[i] != 39){
        comand[1][c] = buffer[i];
        c++;
      }
      else if(exec == 0)
          exec = 1;
      else
        exec = 2;

    } else if(buffer[i] == ' '){
        comand[0][i] = '\0';
        arg = 1;

    } else
        comand[0][i] = buffer[i];
  }

  if(arg == 0)
    comand[arg][i] = '\0';
  else
    comand[arg][c] = '\0';
}

int hasChar(char* buffer,char c){
  int i = 0;
    for(;buffer[i] != '\0';i++){
      if(buffer[i] == c) return i;
    }

    return -1;
}

//void addhist(char** historico,char );

int main(){
  int fdrd = -1,fdwr = -1, log_fd = -1,log_rd = -1;

  if(mkfifo("pipeClienteServidor",0666)<0){
		perror("pipe1");
		return 1;
	}

  if(mkfifo("pipeServidorCliente",0666)<0){
		perror("pipe2");
		return 1;
	}

  if((log_fd = open("log",O_WRONLY | O_TRUNC | O_CREAT,0666))< 0){
    perror("open log");
    return 1;
  }

  if((log_rd = open("log",O_RDONLY,0666))< 0){
    perror("open log");
    return 1;
  }

  if(signal(SIGINT,sig_handler) == SIG_ERR){
    perror("error");
    _exit(1);
  }

  int n[2],r = 1,i = 0;
  char c = '\0';
  char numero[10];
  char comand[2][100];
  char *buffer = malloc(sizeof(char) * 100);
  char historico[100];
  //char *s;

  close(fdrd);
  close(fdwr);

  while(r){
    if((fdrd = open("pipeClienteServidor",O_RDONLY))< 0){
      perror("open pipe1");
      return 1;
    }
    if((fdwr = open("pipeServidorCliente",O_WRONLY))< 0){
      perror("open pipe2");
      return 1;
    }

    while(r && (n[0] = read(fdrd,buffer,sizeof(char) * 100))>0){
        write(1,"Received: ",12);
        write(1,buffer,100);

        separaString(buffer,comand);
        write(1,comand[0],strlen(comand[0]));
        write(1,comand[1],strlen(comand[1]));
        printf("%lu %lu\n", strlen(comand[0]),strlen(comand[1]));
  /*      if((n[1] = hasChar(buffer,' '))>= 0){
            s = strsep(&buffer, " ");
            strcpy(comand[0],s);

            if(hasChar(buffer,39) >= 0){
              s = strsep(&buffer, "'");
              s = strsep(&buffer, "'");
            }
            strcpy(comand[1],s);
        } else {
          strcpy(comand[0],buffer);
          comand[0][n[1]] = '\0';
        }*/

        if((!strcmp(comand[0], "executar")) ||
           (!strcmp(comand[0], "-e"))) {
          r = itoa(tarefa,numero);
          write(log_fd,numero,r);
          write(fdwr,"nova tarefa #",13);
          write(fdwr,numero,r);
          tarefa++;

          if(!fork()){
            i = executar(comand[1],maxTime);
            write(log_fd,"*",1);
            write(log_fd,comand[1],strlen(comand[1]));
            write(log_fd,"\n",1);
            _exit(1);
          }
        }

        else if((!strcmp(comand[0], "ajuda")) ||
                (!strcmp(comand[0], "-h"))){
                  printf("...\n" );
          if(!fork()){
            ajuda(fdwr);
            _exit(1);
          }
        }

        else if((!strcmp(comand[0], "tempo-execucao")) ||
                (!strcmp(comand[0], "-m"))){
            maxTime = atoi(comand[1]);
            printf("%d\n", maxTime);
        }

        else if((!strcmp(comand[0], "historico")) ||
                (!strcmp(comand[0], "-r"))){
          lseek(log_rd,0,SEEK_SET);
          i = 1;
          historico[0] = '#';
          
          while(read(log_rd,&c,sizeof(char)) > 0){
            if(c == '*'){
              historico[i] = '\n';
              strcat(historico,", concluida: ");
              i = strlen(historico);
            }
            else if (c == '\n'){
              historico[i] = '\n';
              historico[i + 1] = '\0';
              write(1,historico,strlen(historico));
              i = 1;
              historico[0] = '#';
            }
            else {
              historico[i] = c;
              i++;
            }
          }
        }

        else if(strcmp(comand[0], "sair") == 0)
          r=0;

        else
          write(1,"Comando inválido\n",18);

        memset(buffer, 0, 100);
        memset(comand[0], 0, 100);
        memset(comand[1], 0, 100);
    }
    close(fdrd);
    close(fdwr);
  }
}
