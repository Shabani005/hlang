#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#define NB_IMPLEMENTATION
#include "lexer.h"
#include "nb.h"

typedef struct {
  Token *left;
  Token *right;
  size_t prec;
  symbols op;
} ASTNode;

typedef struct {
  ASTNode *nodes;
  size_t size;
} ASTTree;

Token *copy_single_token(const Token *src, size_t i) {
    Token *t = calloc(1, sizeof(Token));
    assert(t);

    t->size = 1;
    t->type = malloc(sizeof(int));
    t->text = malloc(sizeof(char*));
    assert(t->type && t->text);

    t->type[0] = src->type[i];
    t->text[0] = strdup(src->text[i]);  

    return t;
}

ssize_t find_prev_token(const Token *tok, size_t start) {
    for (ssize_t i = (ssize_t)start; i >= 0; --i) {
        if (tok->type[i] != TOKEN_SPACE &&
            tok->type[i] != TOKEN_NEWLINE &&
            tok->type[i] != TOKEN_EOF) {
            return i;
        }
    }
    return -1;
}

ssize_t find_next_token(const Token *tok, size_t start) {
    for (size_t i = start; i < tok->size; ++i) {
        if (tok->type[i] != TOKEN_SPACE &&
            tok->type[i] != TOKEN_NEWLINE &&
            tok->type[i] != TOKEN_EOF) {
            return i;
        }
    }
    return -1;
}


size_t token_precedence(Token token, size_t idx){
  switch (token.type[idx]) {
    case TOKEN_PLUS:
      return 1;
      break;
    case TOKEN_MINUS:
      return 1;
      break;
    case TOKEN_MUL:
      return 2;
      break;
    case TOKEN_DIV:
      return 3;
      break;
    
    default:
      return 0;
      break;
  }
}

ASTTree ast_walk(Token token) {
    ASTTree ops = {0};

    ops.nodes = calloc(token.size, sizeof(ASTNode));
    assert(ops.nodes);

    for (size_t i = 0; i < token.size; ++i) {
        switch (token_precedence(token, i) > 0) {
          case true: {
              ssize_t l = find_prev_token(&token, i - 1);
              ssize_t r = find_next_token(&token, i + 1);
              assert(l >= 0 && r >= 0);

              ASTNode op = {0};
              op.left = copy_single_token(&token, l);
              op.right = copy_single_token(&token, r);
              op.prec = token_precedence(token, i);
              op.op = token.type[i];
              ops.nodes[ops.size++] = op;
              break;
          }
      }
    }

    return ops;
}

int main(int argc, char **argv){
  Token to_tokenize = {0};
  if (argc > 1) {
    to_tokenize = tokenize_all(nb_read_file(argv[1]));
  }
  for (size_t i=0; i<to_tokenize.size; ++i){
    printf("Type: %s\nText: %s\n\n", token_type_to_string(to_tokenize.type[i]), to_tokenize.text[i]);
  }
  ASTTree walked = ast_walk(to_tokenize);
  printf("op: %s, left: %s, right: %s, prec %zu\n", token_type_to_string(walked.nodes->op), walked.nodes->left->text[0], walked.nodes->right->text[0], walked.nodes->prec);
  return 0;
}
