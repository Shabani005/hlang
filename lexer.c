#include <assert.h>
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
  TOKEN_FLOAT,
  TOKEN_SPACE,
  TOKEN_STRING,
  intdef,
  TOKEN_UNKNOWN,
} symbols;

typedef enum{
  BHV_STACK,
  BHV_UNDEFINED,
  BHV_NUMBER,
  BHV_STRING,
  BHV_FLOAT,
} symbol_bhv;

typedef struct{
  symbols type;
  char* text;
  size_t text_len;
  symbol_bhv behaviour;
  uint cursor_skip;
  symbols previous_token;
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

// will implement a stack for arithmetic later. do I want a compiler or interpreter? since this is a learning experience im gonna do the easier thing first

Token read_from_tok(char* text, uint cursor){ 
    Token mytoks;
    
    static char buf[64];
    size_t i = 0;
    mytoks.cursor_skip = 1;

    // integer logic. will have to somehow detect "." for floats but it will be hard to do because the way I wrote this code is shit
    // ie: checking for . depends on the switch statement. so I will have to maybe add previous_token to the token struct. Actually a feasible idea.
    // will I need to set previous_token to the current token? maybe.
    if (isdigit(text[cursor])) {
        size_t start = cursor;
        int dots_seen = 0;
        while ( isdigit(text[cursor]) || text[cursor] == '.') {
          if (text[cursor] == '.') {
            dots_seen +=1;
            assert(dots_seen < 2);
          }
          buf[i++] = text[cursor++];
        }
        
        // recheck this assert later
          

        buf[i] = '\0';

        if (!dots_seen){
          mytoks.type = TOKEN_INTEGER;
          mytoks.behaviour = BHV_NUMBER;
        } else {
          mytoks.type = TOKEN_FLOAT;
          mytoks.behaviour = BHV_FLOAT;
        }

       
        mytoks.cursor_skip = cursor - start;
        mytoks.text = buf;
        mytoks.text_len = i;
    } 
    // string logic 
    else if (isalpha(text[cursor])){
        size_t start = cursor;
        while (isalpha(text[cursor])) {
            buf[i++] = text[cursor++];
        }
        buf[i] = '\0';
        mytoks.type = TOKEN_STRING;
        mytoks.behaviour = BHV_STRING; 
        mytoks.cursor_skip = cursor - start;
        mytoks.text = buf;
        mytoks.text_len = i;
    }
    
    else {
       buf[0] = text[cursor];
       buf[1] = '\0';
       mytoks.text = buf;
       
       switch (text[cursor]){
         case '+':
           mytoks.type = TOKEN_PLUS;
         // asigning text is not really needed unless for debug. could however be useful for codegen later.
           mytoks.text = "+";
           mytoks.behaviour = BHV_STACK;
                      break;
         case '-':
           mytoks.type = TOKEN_MINUS;
           mytoks.text = "-";
           mytoks.behaviour = BHV_STACK; 
           break;
         case ' ':
           mytoks.type = TOKEN_SPACE;
           mytoks.text = "space";
           break;
         default:
           mytoks.type = TOKEN_UNKNOWN;
           mytoks.behaviour = BHV_UNDEFINED;
           
    } 
  }
  return mytoks;
}


// Token* c

void parser(Token mytok, char* input){
  int length1 = strlen(input);
  int i=0;



  while (i < length1) {
  mytok = read_from_tok(input, i);
  
  printf("Text: %s\n", mytok.text);
  printf("Behaviour: %d\n", mytok.behaviour);
      if (mytok.behaviour == BHV_STACK){
         printf("this is stack lil bro\n");
      }
    i++;
  }
}


// operators accepted in int/digit or whatever type def only when they have a digit before AND after them 


/*
int main(){
  Token newtok;
  char* input = "8";

  parser(newtok, input);
}
*/

int main(){
    Token newtok;
    char* input = "323.23 + Hello world";
    int length1 = strlen(input);
    int i = 0;
    while (i < length1) {
        Token result = read_from_tok(input, i);
        printf("text: %s\ntype: %u\n\n", result.text, result.type);
        i += result.cursor_skip; 
    }
}
