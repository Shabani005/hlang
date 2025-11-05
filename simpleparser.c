#include "lexer.h"
#include <stddef.h>

typedef struct {
  char* left;
  char* right;
  symbols node;
  size_t cursor;
  size_t prec;
} ASTNode;

typedef struct {
  ASTNode* nodes;
  size_t size;
  size_t capacity;
} ASTTree;

void tree_init(ASTTree* a){
  if (a->capacity == 0) a->capacity = 128;
  a->nodes = malloc(sizeof(*a->nodes)*a->capacity);
}

void construct_nodes(ASTTree* a, Token t){
  if (a->capacity == 0) tree_init(a);
  if (a->size >= a->capacity) {
    a->capacity *=2;
    a->nodes = realloc(a->nodes, sizeof(*a->nodes)*a->capacity);
  }
  size_t nc = 0;
  for (size_t i=0; i<t.size; ++i){
    switch (t.type[i]){
      case TOKEN_PLUS:
        assert(i >  0 && i < t.size - 1);
        a->nodes[nc].node = TOKEN_PLUS;
        a->nodes[nc].left = t.text[i-1];
        a->nodes[nc].right = t.text[i+1];
        a->nodes[nc].prec = 1;
        a->nodes[nc].cursor = nc;
        nc++;
        break;
      case TOKEN_MINUS:
        assert(i >  0 && i < t.size - 1);
        a->nodes[nc].node = TOKEN_MINUS;
        a->nodes[nc].left = t.text[i-1];
        a->nodes[nc].right = t.text[i+1];
        a->nodes[nc].prec = 1;
        a->nodes[nc].cursor = nc;
        nc++;
        break;
      case TOKEN_DIV:
        assert(i >  0 && i < t.size - 1);
        a->nodes[nc].node = TOKEN_DIV;
        a->nodes[nc].left = t.text[i-1];
        a->nodes[nc].right = t.text[i+1];
        a->nodes[nc].prec = 2;
        a->nodes[nc].cursor = nc;
        nc++;
        break;
      case TOKEN_MUL:
        assert(i >  0 && i < t.size - 1);
        a->nodes[nc].node = TOKEN_MUL;
        a->nodes[nc].left = t.text[i-1];
        a->nodes[nc].right = t.text[i+1];
        a->nodes[nc].prec = 2;
        a->nodes[nc].cursor = nc;
        nc++;
        break;
      default:
        break;
    }
  }
  a->size = nc;
}


// void eval(ASTTree *tree){
//   for (size_t i=0; i<tree->size; ++i){
//   ASTNode curr = tree->nodes[i];
//   size_t n = tree->size;
//   float total = 0.0f;
//   switch (curr.node){
//       case TOKEN_PLUS:
//         if (tree->size > 1){
//           for (size_t i=0; i<tree->size; ++i){
//             total += atof(tree->nodes[i].left) + atof(tree->nodes[i].right);
//           }
//           // for (size_t i=0; i<n; i++ && n--){
//           //   total -= 
//           // }
//         }
//         for (size_t i=0; i<tree->size && (tree->size%i != 0); ++i){
//           // total -= atof(tree->nodes[i].right); 
//           printf("%zu\n", i);
//         }
        
//         printf("%f\n", total);
//         break;
//       default:
//         break;
//     }
//   }
// }


void eval(ASTTree* tree) {
    if (!tree || !tree->nodes || tree->size == 0) {
        fprintf(stderr, "Invalid or empty ASTTree\n");
        return;
    }

    float total = 0.0f;
    int initialized = 0;

    for (size_t i = 0; i < tree->size; ++i) {
        ASTNode* curr = &tree->nodes[i];

        switch (curr->node) {
            case TOKEN_PLUS:
                if (!initialized && curr->left) {
                    total = atof(curr->left);
                    initialized = 1;
                }
                if (curr->right)
                    total += atof(curr->right);
                break;


            default:
                fprintf(stderr, "Unknown token at node %zu\n", i);
                break;
        }
    }

    printf("Result: %.2f\n", total);
}


int main(int argc, char** argv){
  // Token tokens = tokenize_all("1+2 3-4 1/2 2*7"); //invalid syntax
  Token tokens = tokenize_all("1+2+3+4"); //invalid syntax
  ASTTree tree = {0};
  construct_nodes(&tree, tokens);
  eval(&tree);
  // for (size_t i=0; i<tree.size; ++i){
  //   ASTNode curr = tree.nodes[i];
  //   printf("token: %s\n", token_type_to_string(curr.node));
  //   printf("left: %s right: %s\n", curr.left, curr.right);
  // }
}
  
