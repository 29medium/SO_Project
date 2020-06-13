#ifndef __executar_h
#define __executar_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

int executar(char *line,int time,int fd_wr,int inactivity);

#endif
