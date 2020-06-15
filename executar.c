#include "executar.h"

#define SS 20
int pidsFilho[2];
int numpids = 0;
int principal = 0;

void sigalarm_handler(int signum){
  if(principal != 0){
    kill(pidsFilho[0], SIGUSR1);
    _exit(2);
  }
  else
    kill(getpid(), SIGUSR1);
}

void sigusr1_handler(int signum) {
  for (int i = 0; i< numpids; i++) {
      kill(pidsFilho[i], SIGUSR1);
  }
  _exit(3);
}

void sigusr2_handler(int signum) {
    if(numpids == 1)
      kill(pidsFilho[0], SIGUSR1);
    _exit(1);
}

void sigchld_handler(int signum) {
  int r;
  wait(&r);
  r = WEXITSTATUS(r);

  if(r == 3)
    _exit(3);
}

int parse(char* comands[15], char* line){
    int i = 0,arg = 0,i2 = 0;
    for(i = 0;line[i] != '\0';i++){
      if(i2 == 0)
        comands[arg] = malloc(sizeof(char) * 50);

      if(line[i] == '|' ){
        comands[arg][i2 - 1] = '\0';
        arg++;i++;
        i2 = 0;
      } else {
        comands[arg][i2] = line[i];
        i2++;
      }
    }
    comands[arg][i2] = '\0';
    return arg;
}

void executarComando(char *comand){
  char* buffer[10];
  int i = 0,arg = 0,i2 = 0;

  for(;comand[i] != '\0';i++){
    if(i2 == 0)
      buffer[arg] = malloc(sizeof(char) * 30);

    if(comand[i] == ' '){
      buffer[arg][i2] = '\0';

      i2 = 0;arg++;
    } else {
      buffer[arg][i2] = comand[i];
      i2++;
    }
  }
  buffer[arg][i2] = '\0';

  buffer[arg + 1] = NULL;

  execvp(buffer[0],buffer);
}

int executar(char *line,int maxtime,int log_fd,int inactivity){
  char*comands[15];
  int arg = parse(comands, line);
  int pipe_fd[arg][2];
  int pid = getpid();
  int pidFilho;

  signal(SIGALRM,sigalarm_handler);
  signal(SIGUSR1, sigusr1_handler);
  signal(SIGCHLD, sigchld_handler);

  if(maxtime != -1)
    alarm(maxtime);

  dup2(log_fd,1);

  if(!(pid = fork())) {
    if(arg == 0){
      executarComando(comands[0]);
      _exit(1);
    }
    else {
      for(int i = 0;i < arg;i++) {
        if(pipe(pipe_fd[i])<0){perror("pipe");_exit(1);}

        if(!(pidFilho = fork())){
          numpids = 0;

          close(pipe_fd[i][1]);
          dup2(pipe_fd[i][0],0);
          close(pipe_fd[i][0]);

          if(i + 1 == arg){
            if(inactivity != -1)
              alarm(inactivity);
            executarComando(comands[i+1]);
            _exit(1);
          }

        } else {
            pidsFilho[numpids++] = pidFilho;
            close(pipe_fd[i][0]);

            if(inactivity != -1)
              alarm(inactivity);

            if(!(pidFilho = fork())){
              numpids = 0;

              dup2(pipe_fd[i][1],1);
              close(pipe_fd[i][1]);
              executarComando(comands[i]);
              _exit(1);
            }
            pidsFilho[numpids++] = pidFilho;

            close(pipe_fd[i][1]);
            wait(NULL);
            alarm(0);
            wait(NULL);
            _exit(1);
        }
      }
    }
  }
  else {
    principal = 1;
    pidsFilho[numpids++] = pid;
    signal(SIGUSR2, sigusr2_handler);
    wait(NULL);
  }

  return 0;
}
