#include "./lexer.h"


int main(void){
  const char ts[] = "\"hello\" hi"; // = 3
  Token tk = tokenize_all(ts);
  for (size_t i=0; i<tk.size; ++i){
    printf("TokenNum: %zu Type: %d Value: %s\n", i, tk.type[i], tk.text[i]);
  }
  // printf("token count: %zu\n", tk.size);
}
