#include "nb.h"

int main(void){
  nb_arr cmd;

  nb_append(&cmd, "gcc");
  nb_append(&cmd, "-Wall -Wextra");
  nb_append(&cmd, "lexer.c");
  nb_append(&cmd, "-o lex");
  
  nb_print_info(&cmd);
  
  nb_cmd(&cmd); 

  nb_free(&cmd);

  nb_append(&cmd, "./lex");

  nb_print_info(&cmd);

  nb_cmd(&cmd);
}
