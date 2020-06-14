#include "argus.h"

#define BUFFERSIZE 4096

Lista tarefasExecucao = NULL;
char** historico;
int histSize = 0;
int maxTime = -1;
int maxInactivity = -1;
int numeroTarefa = 1;
int logID_wr = -1;
int log_wr = -1;
int ordemIDX[50];
int ordIDX = 0;

void sigChild_handler(int signum) {
  int r;
  int pid = wait(&r);
  r = WEXITSTATUS(r);

  histSize++;
  historico = realloc(historico,sizeof(char*) * (histSize));
  historico[histSize - 1] = linhaHistorico(pid,tarefasExecucao,r);

  if (pid > 0) {
    int numero = getNumerofromPid(pid,tarefasExecucao);
    ordemIDX[ordIDX++] = numero;
    tarefasExecucao = removePid(pid, tarefasExecucao);

    char *buffer;
    buffer = itoa(lseek(log_wr,0,SEEK_CUR));
    strcat(buffer,"\n");
    write(logID_wr,buffer,8);
  }
}

void ajuda(int fdwr) {
    char help[] = "\ntempo-inactividade segs\ntempo-execucao segs\nexecutar p1 | p2 ... | pn\nlistar\nterminar #num\nhistorico\najuda\n\n";
    write(fdwr, help, sizeof(help));
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
  else {
    comand[arg][c] = '\0';
  }
}

void output(int numero,int log_rd,int logID_rd,int fdwr){
  char posBuf[10];

  if(numero < numeroTarefa && containsNum(numero,tarefasExecucao) == 0){
    size_t pos,pos2;
    int linha,i = 0;

    for(;i < ordIDX; i++){
      if(ordemIDX[i] == numero)
        linha = i;
    }

    if(linha == 0)
      pos = 0;
    else{
      lseek(logID_rd,(linha - 1) * 8,SEEK_SET);
      read(logID_rd,posBuf,8);
      pos = atoi(posBuf);
      memset(posBuf, 0, 10);
    }
    lseek(logID_rd,linha * 8,SEEK_SET);
    read(logID_rd,posBuf,8);
    pos2 = atoi(posBuf);
    memset(posBuf, 0, 10);

    lseek(log_rd,pos,SEEK_SET);
    char buffer[pos2 - pos];
    pos = read(log_rd,buffer,pos2 - pos);
    write(fdwr,buffer,pos);
  }
}

int main(){
  int fdrd = -1,fdwr = -1,log_rd = -1,logID_rd = -1;
  int r = 1,i = 0,pid,n;
  char *numero;
  char comand[2][100];
  char *buffer = malloc(BUFFERSIZE * sizeof(char));
  historico = (char**) malloc(sizeof(char*));

  log_wr = open("log",O_WRONLY | O_TRUNC | O_CREAT,0666);;
  log_rd = open("log",O_RDONLY,0666);

  logID_wr = open("log.idx",O_WRONLY | O_TRUNC | O_CREAT,0666);;
  logID_rd = open("log.idx",O_RDONLY,0666);

  signal(SIGCHLD,sigChild_handler);

  if(log_rd < 0 || log_wr < 0 || logID_rd < 0 || logID_wr < 0){
    perror("erro open log");
    return 1;
  }

  while(r){
    fdrd = open("pipeClienteServidor",O_RDONLY);
    fdwr = open("pipeServidorCliente",O_WRONLY);

    if(fdwr < 0 || fdrd < 0){
      perror("erro fifo");
      return 1;
    }

    while(r && (n = read(fdrd,buffer,sizeof(char) * 100))>0){
        separaString(buffer,comand);

        if((!strcmp(comand[0], "executar")) ||
           (!strcmp(comand[0], "-e"))) {
          numero = itoa(numeroTarefa);
          strcpy(buffer,"nova tarefa #");
          strcat(buffer,numero);
          write(fdwr,buffer,strlen(buffer));

          if(!(pid = fork())){
            signal(SIGCHLD, SIG_DFL);
            i = executar(comand[1],maxTime,log_wr,maxInactivity);
            _exit(0);
          }
          else{
            tarefasExecucao = adiciona(pid, numeroTarefa, comand[1], tarefasExecucao);
            numeroTarefa++;
          }
        }

        else if((!strcmp(comand[0], "ajuda")) ||
                (!strcmp(comand[0], "-h")))
                ajuda(fdwr);

        else if((!strcmp(comand[0], "tempo-execucao")) ||
                (!strcmp(comand[0], "-m")))
                maxTime = atoi(comand[1]);

        else if((!strcmp(comand[0], "tempo-inactividade")) ||
                (!strcmp(comand[0], "-i")))
                maxInactivity = atoi(comand[1]);

        else if((!strcmp(comand[0], "historico")) ||
                (!strcmp(comand[0], "-r"))){
                  char buffer[4096];
                  buffer[0] = '\0';

                  for(int i = 0; i< histSize; i ++)
                    strcat(buffer,historico[i]);

                  write(fdwr,buffer,strlen(buffer));
        }

        else if((!strcmp(comand[0], "listar")) ||
                (!strcmp(comand[0], "-l")))
                printLista(tarefasExecucao, fdwr);

        else if((!strcmp(comand[0], "terminar")) ||
                (!strcmp(comand[0], "-t"))){
                if(containsNum(atoi(comand[1]),tarefasExecucao) == 1)
                  kill(getPidFromNumeroTarefa(atoi(comand[1]), tarefasExecucao), SIGUSR2);
        }

        else if((!strcmp(comand[0], "output")) ||
                (!strcmp(comand[0], "-o")))
                output(atoi(comand[1]),log_rd,logID_rd,fdwr);

        else
          write(fdwr,"Comando inválido\n",18);

        memset(buffer, 0, BUFFERSIZE);
        memset(comand[0], 0, 100);
        memset(comand[1], 0, 100);
    }
    close(fdrd);
    close(fdwr);
  }
}
