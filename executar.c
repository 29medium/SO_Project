#include "executar.h"

#define SS 20

int parse(char*** comands, char* line){
    int arg=0, c=0;
    char* s;

    while((s = strsep(&line, " ")) != NULL){
        if(!strcmp(s,"|")) {
            comands[arg][c] = NULL;
            c = 0;
            arg++;
            comands = realloc(comands, sizeof(char**) * (arg+1));
        }
        else {
          if(c == 0)
            comands[arg] = malloc(sizeof(char*) * (c+1));

          comands[arg][c] = malloc(sizeof(char) * SS);
          strcpy(comands[arg][c], s);
          c++;
        }
        
    }
    comands[arg][c] = NULL;

    return arg;
}

int executar(char *line){
  char*** comands = NULL;

  int arg = parse(comands, line);
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
