#define NB_IMPLEMENTATION
#include "nb.h"

int main(int argc, char **argv){
  nb_rebuild(argc, argv);
  
  nb_arr cmd;

  nb_append(&cmd, "gcc");
  // nb_append(&cmd, "-Wall -Wextra");
  nb_append(&cmd, "-static");
  nb_append(&cmd, "./src/hlc.c");
  nb_append(&cmd, "-o hlc");
  nb_cmd(&cmd); 


  nb_append(&cmd, "./hlc");
  for (int i=1; i<argc; ++i){
    nb_append(&cmd, argv[i]);
  }
  nb_cmd(&cmd);
}
