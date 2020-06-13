#include "executar.h"

#define SS 20
int* pidsFilho;
int numpids = 0;
int ppid = 0;

void sigalarm_handler(int signum){
  if(ppid != 0){
    kill(ppid, SIGUSR1);
    _exit(2);
  }
  else
    kill(getpid(), SIGUSR1);
}

void sigusr1_handler(int signum) {
  for (int i = 0; i< numpids; i++) {
    kill(pidsFilho[i], SIGKILL);
  }
  _exit(3);
}

void sigusr2_handler(int signum) {
    kill(ppid, SIGUSR1);
    _exit(1);
}

void sigchld_handler(int signum) {
  int r;
  wait(&r);
  r = WEXITSTATUS(r);

  if(r == 3)
    _exit(3);
}

int parse(char*** comands, char* line){
    int arg=0, c=0,i = 0,i2 = 0;

    for(;line[i] != '\0';i++){
      if(i2 == 0){
        comands[arg] = realloc(comands[arg],sizeof(char*) * (c + 1));
        comands[arg][c] = malloc(sizeof(char) * SS);
      }

      if(line[i] == '|'){
        comands[arg][c] = NULL;
        arg++;i++;
        i2 = 0;c = 0;
        comands = realloc(comands,sizeof(char**) * (arg + 1));
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

    return arg + 1;
}

int executar(char *line,int maxtime,int log_fd,int inactivity){
  char***comands = malloc(sizeof(char*));
  int arg = parse(comands, line);
  int pipe_fd[arg][2];
  int pid = getpid();
  int pidFilho;

  signal(SIGALRM,sigalarm_handler);
  signal(SIGUSR1, sigusr1_handler);

  if(maxtime != -1)
    alarm(maxtime);

  dup2(log_fd,1);

  if(!(pid = fork())) {
    if(arg == 1){
      pidsFilho = malloc(sizeof(int));
      pidsFilho[numpids++] = getpid();
      execvp(comands[0][0],comands[0]);
      _exit(0);
    }
    else {
      pidsFilho = malloc(sizeof(int) * arg);

      for(int i = 0;i < arg;i++) {
        if(pipe(pipe_fd[i])<0)
          {perror("pipe");_exit(1);}

        if(!(pidFilho = fork())) {
          if(i == arg)
            execvp(comands[i][0],comands[i]);

          close(pipe_fd[i][0]);
          dup2(pipe_fd[i][1],1);
          close(pipe_fd[i][1]);

          execvp(comands[i][0],comands[i]);
        }
        if(inactivity != -1)
          alarm(inactivity);

        close(pipe_fd[i][1]);
        dup2(pipe_fd[i][0],0);
        close(pipe_fd[i][0]);

        pidsFilho[numpids++] = pidFilho;
        //wait(NULL);
      }
    }
    for(int i = 0; i<numpids; i++)
      wait(NULL);

  } else{
    signal(SIGUSR2, sigusr2_handler);
    signal(SIGCHLD, sigchld_handler);
    ppid = pid;
    wait(NULL);
  }

  return 0;
}
