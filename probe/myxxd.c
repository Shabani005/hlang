#define NB_IMPLEMENTATION
#include "nb.h"

// check file type and deal with it automatically for user convenience

int main(int argc, char** argv){
  if (argc != 3){
    fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
    return 1;
  }

  // char* buf = nb_read_file(argv[1]);
  
  nb_xxd_info info = {0};
  char *buf = nb_xxd(argv[1], &info, argv[2]);

  char fname[32];

  sprintf(fname, "%s.c", argv[2]);
  fname[32-1] = '\0';
  nb_write_file(fname, buf);
  return 0;
}
