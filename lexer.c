#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


typedef struct{
  char* mstr;
} mstring;

typedef struct{
  int mint;
} mint;

typedef struct{
  float myfloat;
} mfloat;


typedef enum{
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_INTEGER,
  TOKEN_SPACE,
  intdef,
  TOKEN_UNKNOWN,
} symbols;

typedef struct{
  symbols type;
  char* text;
  size_t text_len;
} Token;

typedef struct{
  char *content;
  // size_t cursor;
  // size_t line;
} Lexer;


// Lexer 
void lexer_new(char *content, size_t content_len){

}
// Token
void lexer_next(Lexer *mylexer){
}


Token read_from_tok(char* text, uint cursor){ 
    Token mytoks;
    static char buf[64];
    size_t i = 0;
      if (isdigit(text[cursor])) {
        // Start of an integer token
        size_t start = cursor;
        while (isdigit(text[cursor])) {
            buf[i++] = text[cursor++];
        }
        buf[i] = '\0';
        mytoks.type = TOKEN_INTEGER;
        mytoks.text = buf;
        mytoks.text_len = i;
    }else {
       buf[0] = text[cursor];
       buf[1] = '\0';
       mytoks.text = buf;
       
       switch (text[cursor]){
         case '+':
           mytoks.type = TOKEN_PLUS;
         // asigning text is not really needed unless for debug
           mytoks.text = "+";
           break;
         case '-':
           mytoks.type = TOKEN_MINUS;
           mytoks.text = "-";
           break;
         case ' ':
           mytoks.type = TOKEN_SPACE;
           mytoks.text = "space";
           break;
         default:
           mytoks.type = TOKEN_UNKNOWN;
    } 
  }
  return mytoks;
}


// Token* c



// operators accepted in int/digit or whatever type def only when they have a digit before AND after them 

int main(){
  Token newtok;
  char* input = "32323 + 232";
  int length1 = strlen(input);
  int i = 0;
  while (i < length1) {
    Token result = read_from_tok(input, i);
    printf("text: %s\ntype: %u\n\n", result.text, result.type);
    if (result.type == TOKEN_INTEGER) {
        i += result.text_len; // to skip the whole integer
    } else {
      i++;
    }
}
}
