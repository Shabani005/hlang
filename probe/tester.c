#define NB_IMPLEMENTATION
#include "nb.h"

int main(void){
  nb_xxd_info info = {0};
  printf("%s\n", nb_xxd("./text.txt", &info, "hi"));
  return 0;
}
