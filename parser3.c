#include "./lexer.h"

int get_prec(symbols op){
  switch (op) {
    case TOKEN_MUL:
    case TOKEN_DIV:
       return 2; break;
    case TOKEN_PLUS:
    case TOKEN_MINUS:
      return 1; break;
    default: return 0;
  }
}
// parse

bool is_left_asc(symbols op){
  switch (op) {
    case TOKEN_MUL:
    case TOKEN_DIV:
    case TOKEN_PLUS:
    case TOKEN_MINUS:
      return true; break;
    default: return false;
  }
}

void build_rpn();





int main(void){
  const char ts[] = "\"hello\" hi + 2"; 
  const char math[] = "((1+2)*6)/18"; // = 1
  Token tk = tokenize_all(math);
  for (size_t i=0; i<tk.size; ++i){
    printf("TokenNum: %zu Type: %s Value: %s\n", i, tk.tktype[i], tk.text[i]);
  }
  // printf("token count: %zu\n", tk.size);
}
