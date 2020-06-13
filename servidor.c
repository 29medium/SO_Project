#include "servidor.h"

Lista tarefasExecucao = NULL;
int* pids;
char** historico;
int histSize = 0;
int maxTime = -1;
int maxInactivity = -1;
int numeroTarefa = 1;

void sig_handler(int signum){
  unlink("pipeClienteServidor");
  unlink("pipeServidorCliente");

  write(1,"\nServer off!\n",13);
  _exit(1);
}

void sigChild_handler(int signum) {
  int r;
  int pid = wait(&r);
  r = WEXITSTATUS(r);

  histSize++;
  historico = realloc(historico,sizeof(char*) * (histSize));
  historico[histSize - 1] = linhaHistorico(pid,tarefasExecucao,r);

  if (pid > 0) {
    tarefasExecucao = removePid(pid, tarefasExecucao);
    printf("%d morreu!\n",pid);
  }
}

void ajuda(int fdwr) {
    char help[] = "\ntempo-inactividade segs\ntempo-execucao segs\nexecutar p1 | p2 ... | pn\nlistar\nterminar #num\nhistorico\najuda\n\n";
    write(fdwr, help, sizeof(help));
}

char* itoa(int i){
    char const digit[] = "0123456789";
    int p = 0,size;
    char *b = malloc(sizeof(char) * 10);
    int shifter = i;

    do{
        p++;
        shifter = shifter/10;
    }while(shifter);

    b[p] = '\0';
    size = p + 1;

    do{
        p--;
        b[p] = digit[i%10];
        i = i/10;
    }while(i);

    return b;
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

int hasChar(char* buffer,char c){
  int i = 0;
    for(;buffer[i] != '\0';i++){
      if(buffer[i] == c) return i;
    }

    return -1;
}

int main(){
  int fdrd = -1,fdwr = -1, log_wr = -1,log_rd = -1,logID_rd = -1, logID_wr = -1;
  int r = 1,i = 0,pid,n;
  char *numero;
  char comand[2][100];
  char *buffer = malloc(sizeof(char) * 2048);

  pids = malloc(sizeof(int));

  mkfifo("pipeClienteServidor",0666);
  mkfifo("pipeServidorCliente",0666);

  log_wr = open("log",O_WRONLY | O_TRUNC | O_CREAT,0666);;
  log_rd = open("log",O_RDONLY,0666);

  logID_wr = open("log.idx",O_WRONLY | O_TRUNC | O_CREAT,0666);;
  logID_rd = open("log.idx",O_RDONLY,0666);

  signal(SIGINT,sig_handler);
  signal(SIGCHLD,sigChild_handler);

  if(log_rd < 0 || log_wr < 0 || logID_rd < 0 || logID_wr < 0)
    perror("erro open");

  historico = (char**) malloc(sizeof(char*));

  close(fdrd);
  close(fdwr);

  write(1,"Server On!\n",11);

  while(r){
    fdrd = open("pipeClienteServidor",O_RDONLY);
    fdwr = open("pipeServidorCliente",O_WRONLY);

    while(r && (n = read(fdrd,buffer,sizeof(char) * 100))>0){
        write(1,"Received: ",11);
        write(1,buffer,100);
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

            strcat(numero,"\n");
            write(logID_wr,numero,8);
            _exit(0);
          }
          else{
            pids = realloc(pids, sizeof(int) * (numeroTarefa - 1));
            pids[numeroTarefa - 1] = pid;
            tarefasExecucao = adiciona(pid, numero, comand[1], tarefasExecucao);
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
                for(int i = 0; i< histSize; i ++)
                  write(fdwr,historico[i],strlen(historico[i]));
        }

        else if((!strcmp(comand[0], "listar")) ||
                (!strcmp(comand[0], "-l")))
                printLista(tarefasExecucao, fdwr);

        else if((!strcmp(comand[0], "terminar")) ||
                (!strcmp(comand[0], "-t"))){

                kill(getPidFromNumeroTarefa(comand[1], tarefasExecucao), SIGUSR2);
              }

       else if((!strcmp(comand[0], "output")) ||
              (!strcmp(comand[0], "-o"))){
                size_t pos,pos2;
                i = atoi(comand[1]);

                if(i == 1)
                  pos = 0;
                else{
                  lseek(logID_rd,(i - 2) * 8,SEEK_SET);
                  read(logID_rd,buffer,8);
                  pos = atoi(buffer);
                  memset(buffer, 0, 2048);
                }

                lseek(logID_rd,(i - 1) * 8,SEEK_SET);
                read(logID_rd,buffer,8);
                pos2 = atoi(buffer);
                memset(buffer, 0, 2048);

                lseek(log_rd,pos,SEEK_SET);
                pos = read(log_rd,buffer,pos2 - pos);
                write(fdwr,buffer,pos);
              }

        else if(strcmp(comand[0], "sair") == 0)
          r=0;

        else
          write(1,"Comando inválido\n",18);

        memset(buffer, 0, 2048);
        memset(comand[0], 0, 100);
        memset(comand[1], 0, 100);
    }
    close(fdrd);
    close(fdwr);
  }
}
