#include "argus.h"

#define BUFFERSIZE 4096

int main(int argc,char* argv[]){
  int fdwr,fdrd;

  fdwr = open("pipeClienteServidor",O_WRONLY);
  fdrd = open("pipeServidorCliente",O_RDONLY);

  if(fdrd < 0 || fdwr < 0){
    perror("erro open");
    _exit(0);
  }

  char* argus = "argus$ ";
  char* buffer = malloc(sizeof(char) * BUFFERSIZE);
  int r = 1,n,pid;

  if(!(pid = fork())){
    while((n = read(fdrd,buffer,BUFFERSIZE * sizeof(char))) > 0){
      write(1,buffer,n * sizeof(char));
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
    kill(pid,SIGKILL);
    return 0;
  }

  while(r) {
      write(1, argus, sizeof(argus));

      n = read(0, buffer, sizeof(char)*100);
      write(fdwr,buffer,sizeof(char)*n);
      memset(buffer, 0, 100);

      sleep(1);
  }
  kill(pid,SIGKILL);

  return 0;
}
