#include "src/parser.h"


int main(int argc, char** argv){
  // if (argc < 2){
  //   fprintf(stderr, "No file provided. %s <file>\n", argv[0]);
  //   return 1;
  // }
  Token tk = tokenize_all("fn add(x: int, y: int) int {\n let z = x+y; return z;\n }");
  size_t i=0;
  
  Block final = {0};
  block_init(&final, 9191);
  
  SymbolTable tb = {0};
  symbol_table_init(&tb, 1212);

  printf("===================\n");
  for (size_t j = 0; j < tk.size; ++j) {
    printf("[%zu] type=%s text='%s'\n", j, token_type_to_string(tk.type[j]), tk.text[j]);
  }
  printf("===================\n");
  
  while (i<tk.size && tk.type[i] != TOKEN_EOF){
    skip_space(&tk, &i);
    if (tk.type[i] == TOKEN_FN){
      final = *parse_func_def(&tk, &i, &tb);
    } else break;
  }
  asm("int3");
  return 0;
}
