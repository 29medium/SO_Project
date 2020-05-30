#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc,char* argv[]){
  int r = 1,n,fdwr,fdrd;
  char c;
  char* argus = "argus$ ", *s;
  char* buffer = malloc(sizeof(char) * 256);

  if((fdwr = open("pipeClienteServidor",O_WRONLY))< 0){
    perror("fifo");
    return 1;
  }

  if((fdrd = open("pipeServidorCliente",O_RDONLY))< 0){
    perror("fifo");
    return 1;
  }

  //dup2(fdwr,1);
  if(!fork()){
    while((n = read(fdrd,buffer,256 * sizeof(char))) > 0){
      write(1,buffer,n * sizeof(char));
      write(1,"\n",sizeof(char));
    }

    _exit(1);
  }

  while(r) {
      write(1, argus, sizeof(argus));
      while((n = read(0, &c, sizeof(char)) && c != '\n')>0){
        write(fdwr,&c,sizeof(char));
      }
      write(fdwr,"\0",sizeof(char));

      sleep(1);

      //while ((n = read(fdrd,&c,sizeof(char))) > 0 || c != '\0' | c != '\n')
      /*n = read(fdrd,buffer,256 * sizeof(char));
        write(1,buffer,n * sizeof(char));

      write(1,"\n",sizeof(char));*/

      //n = read(fdwr,buffer,256*sizeof(char));
      //write(1,buffer,n * sizeof(char));
      //buffer[i] = '\0';
      //printf("%s\n", buffer);
      //write(fdwr,buffer,(i + 1) * sizeof(char));
  }
}
