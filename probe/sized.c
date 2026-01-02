#define NB_IMPLEMENTATION
#include "nb.h"

char* nb_append_null(char* buf, size_t len){
  char *newbuf = malloc(sizeof(char*) * len);
  newbuf = buf;
  newbuf[len] = '\0';
  return newbuf;
}

int main(void){
  char test[5] = {'h', 'e', 'l', 'l', 'o'};
  printf("%s\n", nb_append_null(test, 5));
}
