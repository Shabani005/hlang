#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef enum {
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

typedef enum {
  BHV_STACK,
  BHV_UNDEFINED,
  BHV_NUMBER,
  BHV_STRING,
  BHV_FLOAT,
} symbol_bhv;


typedef struct {
  symbols *type;
  char **text;
  size_t *text_len;
  symbol_bhv *behaviour;
  unsigned int *cursor_skip;
  symbols *previous_token;
  size_t capacity;
  size_t size;
} Token;


void token_init(Token *tok, size_t capacity) {
  tok->capacity = capacity;
  tok->size = 0;

  tok->type = malloc(sizeof(symbols) * capacity);
  tok->text = malloc(sizeof(char *) * capacity);
  tok->text_len = malloc(sizeof(size_t) * capacity);
  tok->behaviour = malloc(sizeof(symbol_bhv) * capacity);
  tok->cursor_skip = malloc(sizeof(unsigned int) * capacity);
  tok->previous_token = malloc(sizeof(symbols) * capacity);

  assert(tok->type && tok->text && tok->text_len &&
         tok->behaviour && tok->cursor_skip && tok->previous_token);
}

void token_grow(Token *tok) {
  size_t new_capacity = (tok->capacity == 0 ? 8 : tok->capacity * 2);

  tok->type = realloc(tok->type, new_capacity * sizeof(symbols));
  tok->text = realloc(tok->text, new_capacity * sizeof(char *));
  tok->text_len = realloc(tok->text_len, new_capacity * sizeof(size_t));
  tok->behaviour = realloc(tok->behaviour, new_capacity * sizeof(symbol_bhv));
  tok->cursor_skip = realloc(tok->cursor_skip, new_capacity * sizeof(unsigned int));
  tok->previous_token = realloc(tok->previous_token, new_capacity * sizeof(symbols));

  assert(tok->type && tok->text && tok->text_len &&
         tok->behaviour && tok->cursor_skip && tok->previous_token);

  tok->capacity = new_capacity;
}

void token_push(Token *tok, symbols type, const char *text,
                symbol_bhv behaviour, size_t cursor_skip) {
  if (tok->size >= tok->capacity) {
    token_grow(tok);
  }

  size_t i = tok->size;

  tok->type[i] = type;
  tok->text[i] = strdup(text);
  tok->text_len[i] = strlen(text);
  tok->behaviour[i] = behaviour;
  tok->cursor_skip[i] = cursor_skip;

  if (i > 0)
    tok->previous_token[i] = tok->type[i - 1];
  else
    tok->previous_token[i] = TOKEN_UNKNOWN;

  tok->size++;
}

void token_free(Token *tok) {
  for (size_t i = 0; i < tok->size; i++) {
    free(tok->text[i]);
  }
  free(tok->type);
  free(tok->text);
  free(tok->text_len);
  free(tok->behaviour);
  free(tok->cursor_skip);
  free(tok->previous_token);
}


int str_to_int(char *strint) { return atoi(strint); }
float str_to_float(char *strif) { return strtof(strif, NULL); }

char *token_type_to_string(symbols type) {
  switch (type) {
    case TOKEN_PLUS: return "TOKEN_PLUS";
    case TOKEN_MINUS: return "TOKEN_MINUS";
    case TOKEN_INTEGER: return "TOKEN_INTEGER";
    case TOKEN_FLOAT: return "TOKEN_FLOAT";
    case TOKEN_SPACE: return "TOKEN_SPACE";
    case TOKEN_STRING: return "TOKEN_STRING";
    case TOKEN_MUL: return "TOKEN_MUL";
    case TOKEN_DIV: return "TOKEN_DIV";
    case TOKEN_LPAREN: return "TOKEN_LPAREN";
    case TOKEN_RPAREN: return "TOKEN_RPAREN";
    case TOKEN_COMMA: return "TOKEN_COMMA";
    case TOKEN_EOF: return "TOKEN_EOF";
    case TOKEN_NEWLINE: return "TOKEN_NEWLINE";
    case TOKEN_UNKNOWN: return "TOKEN_UNKNOWN";
    default: return "UNKNOWN_SYMBOL";
  }
}


size_t read_from_tok(Token *tok, const char *input, size_t cursor) {
  char buf[64];
  size_t start = cursor;
  size_t i = 0;

  if (isdigit(input[cursor])) {
    int dots_seen = 0;
    while (isdigit(input[cursor]) || input[cursor] == '.') {
      if (input[cursor] == '.') dots_seen++;
      buf[i++] = input[cursor++];
    }
    buf[i] = '\0';
    if (dots_seen == 0) {
      token_push(tok, TOKEN_INTEGER, buf, BHV_NUMBER, cursor - start);
    } else {
      token_push(tok, TOKEN_FLOAT, buf, BHV_FLOAT, cursor - start);
    }
  } else if (isalpha(input[cursor])) {
    while (isalpha(input[cursor])) {
      buf[i++] = input[cursor++];
    }
    buf[i] = '\0';
    token_push(tok, TOKEN_STRING, buf, BHV_STRING, cursor - start); 
    //refactor into separate function to use in parsing functions and definitions  
  } else {
    buf[0] = input[cursor];
    buf[1] = '\0';
    switch (input[cursor]) {
      case '+': token_push(tok, TOKEN_PLUS, "+", BHV_STACK, 1); break;
      case '-': token_push(tok, TOKEN_MINUS, "-", BHV_STACK, 1); break;
      case '*': token_push(tok, TOKEN_MUL, "*", BHV_STACK, 1); break;
      case '/': token_push(tok, TOKEN_DIV, "/", BHV_STACK, 1); break;
      case ' ': token_push(tok, TOKEN_SPACE, " ", BHV_UNDEFINED, 1); break;
      case '\n': token_push(tok, TOKEN_NEWLINE, "\\n", BHV_UNDEFINED, 1); break;
      case '(': token_push(tok, TOKEN_LPAREN, "(", BHV_STACK, 1); break;
      case ')': token_push(tok, TOKEN_RPAREN, ")", BHV_STACK, 1); break;
      case ',': token_push(tok, TOKEN_COMMA, ",", BHV_STACK, 1); break;
      default: token_push(tok, TOKEN_UNKNOWN, buf, BHV_UNDEFINED, 1); break;
    }
    cursor++;
  }

  return cursor - start;
}

Token tokenize_all(const char *input) {
  Token tok;
  token_init(&tok, 8);

  size_t i = 0;
  size_t length = strlen(input);

  while (i < length) {
    i += read_from_tok(&tok, input, i);
  }

  token_push(&tok, TOKEN_EOF, "EOF", BHV_UNDEFINED, 0);
  return tok;
}



