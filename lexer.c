#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#define NB_IMPLEMENTATION
#include "nb.h"

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
  TOKEN_UNKNOWN,
  TOKEN_EOF,
  TOKEN_NEWLINE,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_COMMA
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
  Token* unit;
  size_t size;
  size_t capacity;
} TokenArr;

typedef struct{
  char *content;
  // size_t cursor;
  // size_t line;
} Lexer;

typedef enum{
  AST_NUMBER,
  AST_BINARY_OP,
} ASTNodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
  ASTNodeType type;
  union {
    struct { double value; } number;
    struct {
      char op;
      ASTNode* left;
      ASTNode* right;
    } binary;
    struct {
      char *name;
      ASTNode** args;
      size_t arg_count;
    } func_call;
  } data;
};

typedef struct{
  Token* tokens;
  size_t cursor;
} parser;

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

ASTNode* ast_new_number(double val){
  ASTNode* node = malloc(sizeof(ASTNode));
  node->type = AST_NUMBER;
  node->data.number.value = val;
  return node;
}

ASTNode* ast_new_binary(char op, ASTNode* l, ASTNode* r){
  ASTNode* node = malloc(sizeof(ASTNode));
  node->type = AST_BINARY_OP;
  node->data.binary.op = op;
  node->data.binary.left = l;
  node->data.binary.right = r;
  // maybe need to fix
  return node;
}

ASTNode* parse_factor(parser* p) {
  Token tok = parser_peek(p);
  if (tok.type == TOKEN_EOF){
    fprintf(stderr, "Unexpected end of input in factor\n");
    exit(EXIT_FAILURE);
  }
  if (tok.type == TOKEN_INTEGER || tok.type == TOKEN_FLOAT){
    parser_advance(p);
    double v = atof(tok.text);
    return ast_new_number(v);
  }
  // if (tok.type == TOKEN_STRING){
  //   parser_advance(p);
  //   char* func_name = tok.text;
  //   if (parser_match(p, TOKEN_LPAREN)){
  //     size_t argc_count = 0;
  //
  //   }
  // }
  fprintf(stderr, "Unexpected token '%s' in factor\n", tok.text);
  exit(EXIT_FAILURE);
}

ASTNode* parse_term(parser* p) {
    ASTNode* node = parse_factor(p);
    while (true) {
        Token tok = parser_peek(p);
        if (tok.type == TOKEN_MUL || tok.type == TOKEN_DIV) {
            parser_advance(p);
            ASTNode* right = parse_factor(p);
            node = ast_new_binary(tok.text[0], node, right);
        } else {
            break;
        }
    }
    return node;
}

ASTNode* parse_expression(parser* p) {
    ASTNode* node = parse_term(p);
    while (true) {
        Token tok = parser_peek(p);
        if (tok.type == TOKEN_PLUS || tok.type == TOKEN_MINUS) {
            parser_advance(p);
            ASTNode* right = parse_term(p);
            node = ast_new_binary(tok.text[0], node, right);
        } else {
            break;
        }
    }
    return node;
}

double eval_ast(ASTNode* node) {
    if (node->type == AST_NUMBER) {
        return node->data.number.value;
    }
    double L = eval_ast(node->data.binary.left);
    double R = eval_ast(node->data.binary.right);
    switch (node->data.binary.op) {
        case '+': return L + R;
        case '-': return L - R;
        case '*': return L * R;
        case '/': return L / R;
        default:
            fprintf(stderr, "Unknown op '%c'\n", node->data.binary.op);
            exit(EXIT_FAILURE);
    }
}

Token read_from_tok(char* text, uint cursor){ 
    Token mytoks;
    
    static char buf[64];
    size_t i = 0;
    mytoks.cursor_skip = 1;

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
       
       switch (text[cursor])
       {
         case '+':
           mytoks.type = TOKEN_PLUS;
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
         case '\n':
          mytoks.type = TOKEN_NEWLINE;
          mytoks.text = strdup("newline");
          mytoks.cursor_skip = 1;
          break;
         case '(':
          mytoks.type = TOKEN_LPAREN;
          mytoks.text = strdup("(");
          mytoks.behaviour = BHV_STACK;
          break;
         case ')':
          mytoks.type = TOKEN_RPAREN;
          mytoks.text = strdup(")");
          mytoks.behaviour = BHV_STACK;
          break;
         case ',':
          mytoks.type = TOKEN_COMMA;
          mytoks.text = strdup(",");
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
        i += tok.cursor_skip;
        if (tok.type == TOKEN_SPACE || tok.type == TOKEN_NEWLINE) {
            free(tok.text);
            continue;
        }
        tokenarr_push(&arr, tok);
    }
    Token eof = {0};
    eof.type = TOKEN_EOF;
    eof.text = strdup("EOF");
    eof.text_len = 3;
    eof.behaviour = BHV_UNDEFINED;
    eof.cursor_skip = 0;
    tokenarr_push(&arr, eof);
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
        case TOKEN_UNKNOWN:  return "TOKEN_UNKNOWN";
        default:             return "UNKNOWN_SYMBOL";
    }
}

// void main2() {
//     char* input = "323.23 + Hello world 102102";
//     int length1 = strlen(input);
//     int i = 0;
//     printf("input: %s\n\n", input);
//     while (i < length1) {
//         Token result = read_from_tok(input, i); 
//         printf("text: %s\ntype: %u (%s)\n\n", result.text, result.type, token_type_to_string(result.type));
//         i += result.cursor_skip;  
//     }
// }



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


// int main4() {
//     char* input = "print(5) hello";
//     printf("input: %s\n\n", input);
//
//     TokenArr arr = tokenize_all(input);
//
//     for (size_t j = 0; j < arr.size; ++j) {
//         Token* result = &arr.unit[j];
//         printf("text: %s\ntype: %u (%s)\n\n", result->text, result->type, token_type_to_string(result->type));
//     }
//
//     printf("================ Tokenized =================\n");
//
//     for (size_t j = 0; j < arr.size; ++j) {
//         Token* result = &arr.unit[j];
//         printf("text: %s, type: %u (%s) || ", result->text, result->type, token_type_to_string(result->type));
//     }
//     printf("\n");
//     for (size_t j = 0; j < arr.size; ++j) {
//       free(arr.unit[j].text);
//     }
//     free(arr.unit);
//     return 0;
// }





// int main5(){
//     char* input = "40/2.3 * 10 + 400";
//     printf("input: %s\n", input);
//     mathparser(input);
//     return 0;
// }


int main(int argc, char** argv) {
    if (argc > 1){
    char* input = nb_read_file(argv[1]);
    // printf("Input: %s\n", input);

    TokenArr toks = tokenize_all(input);

    parser p = { toks.unit, 0 };
    ASTNode* root = parse_expression(&p);

    double result = eval_ast(root);
    printf("%f\n", result);
  } else {
    printf("Usage: %s <file>\n", argv[0]);
  }

    return 0;
}
