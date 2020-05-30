#include <string.h>
#include <stdio.h>

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    int shifter = i;

    do{
        p++;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}


int main(){
  char b[5];
  itoa(145,b);

  printf("%s\n", b);

  return 0;
}
