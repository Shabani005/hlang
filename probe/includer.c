#define NB_IMPLEMENTATION
#include "nb.h"

char* nb_sprintf(char* format, ...){
  
}


// TODO: use rand to not have to do includername manually, but maybe its fine to overwrite because its not user facing
void includer(char* inputfile, char* header, char* includername){
  nb_file file = nb_read_file_c(inputfile);  

  char* buf = malloc(sizeof(char*) * file.filesize * 30);
  char* p = buf;
}

int main(void){
  return 0;
}
