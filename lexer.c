#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>

typedef struct{
  char* mstr;
} mstring;

typedef struct{
  int mint;
} mint;

typedef struct{
  float myfloat;
} mfloat;

int str_to_int(char *strint){
  int new_int = atoi(strint);
  return new_int;
}

float str_to_float(char *strif){
  char *fptr;
  float new_int = strtof(strif, &fptr);
  return new_int;
}



typedef enum{
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_INTEGER,
  TOKEN_FLOAT,
  TOKEN_SPACE,
  TOKEN_STRING,
  TOKEN_MUL,
  TOKEN_DIV,
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

// since I now have tokenize all I dont really need previous_token. I can just ast walk it without each individual token carrying all data

typedef struct{
  Token* unit;
  size_t size;
  size_t capacity;
} TokenArr;
// maybe should add cursor even for TokenArr to use C's printf % to add whitespace in order to move something like this
// input = 1 + 323 + =-=-
//                   ^
//                   |- Unknown Token 


typedef struct{
  char *content;
  // size_t cursor;
  // size_t line;
} Lexer;


// will not nesseccarilly use AST. just could be useful in the future.

typedef enum{
  AST_NUMBER,
  AST_BINARY_OP,
} ASTNodeType;

typedef struct{
  ASTNodeType type;
  union {
    struct {
      double value;
    } number;
    struct {
      char op;
      struct ASTNode* left;
      struct ASTNode* right;
      // x = 5 + 3 parsed into 
      //    =
      //  /   \
      // x   / \
      //   5   3
    } binary;
  };
} ASTNode;

typedef struct{
  Token* tokens;
  size_t cursor;
} parser;
// tokenArr to token*

// Lexer 
void lexer_new(char *content, size_t content_len){
  (void) content;
  (void) content_len;
}
// Token
void lexer_next(Lexer *mylexer){
  (void) mylexer;
}

Token parser_peek(parser* p){
  return p->tokens[p->cursor];
}

Token parser_advance(parser* p){
  return p->tokens[p->cursor++];
}

bool parser_match(parser* p, symbols tokent){
  if (parser_peek(p).type == tokent){
    parser_advance(p);
    return true;
  } else {
    return false;
  }
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
        mytoks.text = strdup(buf);
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
        mytoks.text = strdup(buf);
        mytoks.text_len = i;
    }
    
    else {
       buf[0] = text[cursor];
       buf[1] = '\0';
       
       switch (text[cursor]){
         case '+':
           mytoks.type = TOKEN_PLUS;
         // asigning text is not really needed unless for debug. could however be useful for codegen later. NOW IT BECAME A MUST LOL
           mytoks.text = strdup("+");
           mytoks.behaviour = BHV_STACK;
                      break;
         case '-':
           mytoks.type = TOKEN_MINUS;
           mytoks.text = strdup("-");
           mytoks.behaviour = BHV_STACK; 
           break;
         case ' ':
           mytoks.type = TOKEN_SPACE;
           mytoks.text = strdup("space");
           break;
         case '*':
          mytoks.type = TOKEN_MUL;
          mytoks.text = strdup("*");
          mytoks.behaviour = BHV_STACK;
          break;
         case '/':
          mytoks.type = TOKEN_DIV;
          mytoks.text = strdup("/");
          mytoks.behaviour = BHV_STACK;
          break;
         default:
           mytoks.type = TOKEN_UNKNOWN;
           mytoks.behaviour = BHV_UNDEFINED;
           mytoks.text = strdup(buf);
           
    } 
  }
  return mytoks;
}


void tokenarr_push(TokenArr* arr, Token tok) {
    if (arr->size >= arr->capacity) {
        arr->capacity = arr->capacity ? arr->capacity * 2 : 8;
        arr->unit = realloc(arr->unit, arr->capacity * sizeof(Token));
        assert(arr->unit != NULL);
    }
    arr->unit[arr->size++] = tok;
}

TokenArr tokenize_all(const char* input) {
    TokenArr arr = {NULL, 0, 0};
    size_t i = 0;
    size_t len = strlen(input);
    while (i < len) {
        Token tok = read_from_tok((char*)input, i);
        tokenarr_push(&arr, tok);
        i += tok.cursor_skip;
    }
    return arr;
}




// Token* c

void token_parser(Token mytok, char* input){
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

char* token_type_to_string(symbols type) {
    switch (type) {
        case TOKEN_PLUS:     return "TOKEN_PLUS";
        case TOKEN_MINUS:    return "TOKEN_MINUS";
        case TOKEN_INTEGER:  return "TOKEN_INTEGER";
        case TOKEN_FLOAT:    return "TOKEN_FLOAT";
        case TOKEN_SPACE:    return "TOKEN_SPACE";
        case TOKEN_STRING:   return "TOKEN_STRING";
        case intdef:         return "intdef";
        case TOKEN_UNKNOWN:  return "TOKEN_UNKNOWN";
        default:             return "UNKNOWN_SYMBOL";
    }
}

void main2() {
    char* input = "323.23 + Hello world 102102";
    int length1 = strlen(input);
    int i = 0;
    printf("input: %s\n\n", input);
    while (i < length1) {
        Token result = read_from_tok(input, i); 
        printf("text: %s\ntype: %u (%s)\n\n", result.text, result.type, token_type_to_string(result.type));
        i += result.cursor_skip;  
    }
}



void mathparser(const char* input) {  
    TokenArr stack = tokenize_all(input);  
    float result = 0;
    float current = 0;  
    float sign = 1;
    float op = 0;  
    
    for (size_t i = 0; i < stack.size; ++i) {  
        switch (stack.unit[i].type) {  
            case TOKEN_INTEGER:
                {  
                float value = str_to_float(stack.unit[i].text);  
                if (op == 1) {  
                    current *= value;  
                    op = 0;  
                } else if (op == 2) { 
                    current /= value;  
                    op = 0;  
                } else {  
                    current = value;  
                }
                break;  
            }

            case TOKEN_FLOAT:
                {  
                float value = str_to_float(stack.unit[i].text);  
                if (op == 1) {  
                    current *= value;  
                    op = 0;  
                } else if (op == 2) { 
                    current /= value;  
                    op = 0;  
                } else {  
                    current = value;  
                }
                break;  
            }
            case TOKEN_PLUS:  
                result += sign * current;  
                sign = 1;  
                op = 0;  
                break;  
            case TOKEN_MINUS:  
                result += sign * current;  
                sign = -1;  
                op = 0;  
                break;  
            case TOKEN_MUL:  
                op = 1;  
                break;  
            case TOKEN_DIV:  
                op = 2;  
                break;  
            default:  
                break;  
        }
    }
    result += sign * current;  
    printf("%f\n", result);  
    for (size_t j = 0; j < stack.size; ++j) {  
        free(stack.unit[j].text);  
    }
    free(stack.unit);  
}


int main4() {
    char* input = "print(5) hello";
    printf("input: %s\n\n", input);

    TokenArr arr = tokenize_all(input);
        
    for (size_t j = 0; j < arr.size; ++j) {
        Token* result = &arr.unit[j];
        printf("text: %s\ntype: %u (%s)\n\n", result->text, result->type, token_type_to_string(result->type));
    }
    
    printf("================ Tokenized =================\n");
    
    for (size_t j = 0; j < arr.size; ++j) {
        Token* result = &arr.unit[j];
        printf("text: %s, type: %u (%s) || ", result->text, result->type, token_type_to_string(result->type));
    }
    printf("\n");
    for (size_t j = 0; j < arr.size; ++j) {
      free(arr.unit[j].text);
    }
    free(arr.unit);
    return 0;
}





int main(){
    char* input = "40/2.3 * 10 + 400";
    printf("input: %s\n", input);
    mathparser(input);  
}
