#include "executar.h"

#define SS 20
int* pidsFilho;
int numpids = 0;

void sigalarm_handler(int signum){
  printf("O tempo acabou!\n");
  _exit(2);
}

void sigusr1_handler(int signum) {
  for (int i = 0; i< numpids; i++) {
    kill(pidsFilho[i], SIGKILL);
  }
  _exit(1);
}

int parse(char* comands[15][10], char* line){
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

int executar(char *line,int time,int log_fd){
  char* comands[15][10];
  int arg = parse(comands, line);
  int pipe_fd[arg][2];
  int pid = getpid();
  int pidFilho, pid1, pid2;

  signal(SIGALRM,sigalarm_handler);
  signal(SIGUSR1, sigusr1_handler);

  if(time != -1)
    alarm(time);

  dup2(log_fd,1);
  if(arg == 0){
    pidsFilho = malloc(sizeof(int));
    if(!(pidFilho = fork()))
      execvp(comands[0][0],comands[0]);
    else {
      pidsFilho[numpids++] = pidFilho;
    }
  }

  else {
    pidsFilho = malloc(sizeof(int) * arg);
    for(int i = 0;i < arg;i++) {
      if(pipe(pipe_fd[i])<0){perror("pipe");_exit(1);}
      if(!(pid1 = fork())){
        close(pipe_fd[i][1]);
        dup2(pipe_fd[i][0],0);
        close(pipe_fd[i][0]);

        if(i + 1 == arg){
          execvp(comands[i + 1][0],comands[i + 1]);
          _exit(1);
        }

      } else {
          pidsFilho[numpids++] = pid1;
          if(!(pid2 = fork())) {
            close(pipe_fd[i][0]);
            dup2(pipe_fd[i][1],1);
            close(pipe_fd[i][1]);
            execvp(comands[i][0],comands[i]);
            _exit(1);
          }
          pidsFilho[numpids++] = pid2;
          close(pipe_fd[i][0]);
          close(pipe_fd[i][1]);
          wait(NULL);
          wait(NULL);

          if(getpid() != pid)
            _exit(1);
        }
    }
  }

  for(int i = 0; i<numpids; i++) {
    wait(NULL);
  }

  return 0;
}
