#include "executar.h"

#define SS 20

void sigalarm_handler(int signum){
  printf("O tempo acabou!\n");
  _exit(1);
}

int parse(char* comands[10][10], char* line){
    int arg=0, c=0,i = 0,i2 = 0;

    for(;line[i] != '\0';i++){
      if(i2 == 0)
        comands[arg][c] = malloc(sizeof(char) * SS);

      if(line[i] == '|'){
        comands[arg][c] = NULL;
        arg++;
        i2 = 0;
        c = 0;
        i++;
      }
      else if(line[i] == ' '){
        comands[arg][c][i2] = '\0';
        c++;
        i2 = 0;
      }
      else{
        comands[arg][c][i2] = line[i];
        i2++;
      }
    }

    comands[arg][c][i2] = '\0';
    comands[arg][c + 1] = NULL;

    return arg;
}

int executar(char *line,int time){
  if(time != -1){
    if(signal(SIGALRM,sigalarm_handler) == SIG_ERR){
      perror("error");
      _exit(1);
    }
    alarm(time);
  }

  printf("%d\n", time);

  //char*** comands = malloc(sizeof(char**));
  char* comands[10][10];
  int arg = parse(comands, line);

  int pipe_fd[arg][2];
  int pid = getpid(),log_fd;

  if((log_fd = open("log.txt",O_WRONLY | O_TRUNC | O_CREAT,0666))< 0){
    perror("open log");
    return 1;
  }

  //dup2(log_fd,1);

  if(arg == 0){
    if(!fork())
      execvp(comands[0][0],comands[0]);
  }

  else {
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

          if(getpid() != pid)
            _exit(1);
        }
    }
  }

  return 0;
}
