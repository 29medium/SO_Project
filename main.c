#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc,char *argv[]){
  int pid, c = 0,arg = 0;
  char* comands[6][10];

  for(int i = 1;i < argc;i++) {
    if(!strcmp(argv[i],",")) {
      comands[arg][c] = NULL;
      c = 0;arg++;
    }
    else {
      comands[arg][c] = argv[i];
      c++;
    }
  }
  comands[arg][c] = NULL;

  int pipe_fd[arg][2];

  for(int i = 0;i < arg;i++) {
    if(pipe(pipe_fd[i])<0){perror("pipe");_exit(1);}
    if(!fork()){
      close(pipe_fd[i][1]);
      dup2(pipe_fd[i][0],0);
      close(pipe_fd[i][0]);

      if(i + 1 == arg){
        execvp(comands[i + 1][0],comands[i + 1]);
        _exit(1);
      }

    } else {
        if(!fork()) {
          close(pipe_fd[i][0]);
          dup2(pipe_fd[i][1],1);
          close(pipe_fd[i][1]);
          execvp(comands[i][0],comands[i]);
          _exit(1);
        }
        close(pipe_fd[i][0]);
        close(pipe_fd[i][1]);
        wait(NULL);
        wait(NULL);
        _exit(1);
    }
  }
  return 0;
}