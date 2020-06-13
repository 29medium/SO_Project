#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

void sig_handler(int signum){
  write(1,"\nA encerrar!\n",12);
  _exit(1);
}

int main(int argc,char* argv[]){
  int fdwr,fdrd;

  fdwr = open("pipeClienteServidor",O_WRONLY);
  fdrd = open("pipeServidorCliente",O_RDONLY);
  signal(SIGINT,sig_handler);

  if(fdrd < 0 || fdwr < 0)
    perror("erro open");

  char c;
  char* argus = "argus$ ", *s;
  char* buffer = malloc(sizeof(char) * 100);
  int r = 1,n;

  if(!fork()){
    while((n = read(fdrd,buffer,256 * sizeof(char))) > 0){
      write(1,buffer,n * sizeof(char));
      write(1,"\n",sizeof(char));
    }
    _exit(1);
  }

  if(argc > 1){
    if(argc == 3){
      strcpy(buffer,argv[1]);
      strcat(buffer," ");
      strcat(buffer,argv[2]);
    }
    else
      strcpy(buffer,argv[1]);

    write(fdwr,buffer,sizeof(char)*strlen(buffer));

    return 0;
  }

  while(r) {
      write(1, argus, sizeof(argus));

      n = read(0, buffer, sizeof(char)*100);
      write(fdwr,buffer,sizeof(char)*n);
      memset(buffer, 0, 100);

      sleep(1);
  }
  return 0;
}
