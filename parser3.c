#include "./lexer.h"


int main(void){
  const char ts[] = "\"hello\" hi + 2"; 
  const char math[] = "((1+2)*6)/18"; // = 1
  Token tk = tokenize_all(math);
  for (size_t i=0; i<tk.size; ++i){
    printf("TokenNum: %zu Type: %s Value: %s\n", i, tk.tktype[i], tk.text[i]);
  }
  // printf("token count: %zu\n", tk.size);
}
