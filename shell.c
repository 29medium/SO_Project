#include "shell.h"

#define BUFFERSIZE 1024

void ajuda() {
    char help[] = "\ntempo-inactividade segs\ntempo-execucao segs\nexecutar p1 | p2 ... | pn\nlistar\nterminar #num\nhistorico\najuda\n\n";
    write(1, help, sizeof(help));
}

int shell() {
    int r=1;
    char* argus = "argus$ ", *s;
    char* buffer = malloc(sizeof(char) * BUFFERSIZE);

    while(r) {
        write(1, argus, sizeof(argus));

        read(0, buffer, BUFFERSIZE);

        s = strsep(&buffer, " ");

        if(strcmp(s, "executar") == 0) {
            s = strsep(&buffer, "'");
            s = strsep(&buffer, "'");
            executar(s);
        }

        else if(strcmp(s, "ajuda") == 0)
            ajuda();

        else if(strcmp(s, "sair") == 0)
            r=0;    
    }

    return 0;
}