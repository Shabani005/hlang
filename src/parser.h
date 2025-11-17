#include "./lexer.h"
#define NB_IMPLEMENTATION
#include "../nb.h"

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

Token *global_tok = NULL;

typedef enum {
  SYM_VAR,
  SYM_FUNC,
} SymbolKind;

typedef struct {
  const char* name;
  size_t ret_count;
  size_t arg_count;
  symbols arg_types[16];
  symbols ret_type;
  SymbolKind symbol_kind;
  bool builtin;
} Symbol;


// static Symbol builtins[] = {
//     { "print", 1, 1, { TOKEN_UNKNOWN }, TOKEN_EOF, SYM_FUNC, true },
// };


typedef struct {
  Symbol *symbols;
  size_t size;
  size_t capacity;
} SymbolTable; 

typedef struct {
    Token* statements;
    size_t size;
    size_t capacity;
} Block;

void block_init(Block *b, size_t initial_cap){    
    b->statements = malloc(sizeof(Token) * initial_cap);
    b->capacity = initial_cap;
    b->size = 0;
}

void block_append(Block *b, Token t){
    // if (b->capacity == 0) {
    //     b->capacity = 192;
    //     b->size = 0;
    //     b->statements = (Token*)malloc(sizeof(Token)*b->capacity);
    // }
    if (b->size >= b->capacity) {
        b->capacity *=2;
        b = (Block*)realloc(b, sizeof(Token)*b->capacity);
    }
    b->statements[b->size] = t; // probably wrong
    b->size++;
}


// static int builtin_num = sizeof(builtins)/sizeof(builtins[0]);

// static SymbolTable global_env = {
//   .size = sizeof(builtins)/sizeof(builtins[0]),
//   .capacity = sizeof(builtins)/sizeof(builtins[0]),
//   .symbols = builtins};


Symbol *symbol_lookup(SymbolTable *table, const char *n){
  for (size_t i=0; i<table->size; ++i){
    if(strcmp(n, table->symbols[i].name) == 0){
      return &table->symbols[i];
    }
  }
  return NULL;
}

// fn add(x: int, y: int) int {
//   return x+y;
// }


void symbol_table_init(SymbolTable *table, size_t initial_capacity) {
    table->symbols = (Symbol*)malloc(sizeof(Symbol) * initial_capacity);
    if (!table->symbols) {
        fprintf(stderr, "symbol_table_init: malloc failed\n"); // should not happen
        exit(1);
    }
    table->size = 0;
    table->capacity = initial_capacity;
}

void symbol_table_add(SymbolTable *table, Symbol sym) {
    if (table->size >= table->capacity) {
        table->capacity = (table->capacity == 0) ? 8 : table->capacity * 2;
        table->symbols = (Symbol*)realloc(table->symbols, sizeof(Symbol) * table->capacity);
        if (!table->symbols) {
            fprintf(stderr, "symbol_table_add: realloc failed\n");
            exit(1);
        }
    }
    table->symbols[table->size++] = sym;
}


void symbol_table_free(SymbolTable *table) {
    free(table->symbols);
    table->symbols = NULL;
    table->size = 0;
    table->capacity = 0;
}


Token build_rpn(Token *inp, SymbolTable *symtab) {
    Token output;
    Token stack;

    token_init(&output, 16);
    token_init(&stack, 16);

    for (size_t i = 0; i < inp->size; ++i) {
        symbols type = inp->type[i];
        const char *text = inp->text[i];

        if (type == TOKEN_IDENTIFIER && i + 1 < inp->size && inp->type[i + 1] == TOKEN_LPAREN) {
            Symbol *found = symbol_lookup(symtab, text);
            if (!found) {
                Symbol sym = {
                    .name = strdup(text),
                    .arg_count = 0,
                    .ret_type = TOKEN_EOF,
                    .symbol_kind = SYM_FUNC,
                    .builtin = false
                };
                symbol_table_add(symtab, sym);
            }
            token_push(&stack, type, text, inp->behaviour[i], 0);
        } else if (type == TOKEN_IDENTIFIER) {
            Symbol *found = symbol_lookup(symtab, text);
            if (!found) {
                Symbol sym = {
                    .name = strdup(text),
                    .arg_count = 0,
                    .ret_type = TOKEN_UNKNOWN,
                    .symbol_kind = SYM_VAR,
                    .builtin = false
                };
                symbol_table_add(symtab, sym);
            }
            token_push(&output, type, text, inp->behaviour[i], 0);
        } else if (type == TOKEN_LPAREN) {
            token_push(&stack, type, text, inp->behaviour[i], 0);
        } else if (type == TOKEN_RPAREN) {
            while (stack.size > 0 && stack.type[stack.size - 1] != TOKEN_LPAREN) {
                token_push(&output, stack.type[stack.size - 1],
                           stack.text[stack.size - 1],
                           stack.behaviour[stack.size - 1], 0);
                stack.size--;
            }
            if (stack.size > 0 && stack.type[stack.size - 1] == TOKEN_LPAREN)
                stack.size--;
            if (stack.size > 0 && stack.type[stack.size - 1] == TOKEN_IDENTIFIER) {
                token_push(&output, stack.type[stack.size - 1],
                           stack.text[stack.size - 1],
                           stack.behaviour[stack.size - 1], 0);
                stack.size--;
            }
        } else if (type == TOKEN_INTEGER || type == TOKEN_FLOAT || type == TOKEN_STRING) {
            token_push(&output, type, text, inp->behaviour[i], 0);
        } else if (is_left_asc(type)) {
            while (stack.size > 0 && stack.type[stack.size - 1] != TOKEN_LPAREN &&
                   (get_prec(stack.type[stack.size - 1]) > get_prec(type) ||
                    get_prec(stack.type[stack.size - 1]) == get_prec(type)) &&
                   is_left_asc(type)) {
                token_push(&output, stack.type[stack.size - 1],
                           stack.text[stack.size - 1],
                           stack.behaviour[stack.size - 1], 0);
                stack.size--;
            }
            token_push(&stack, type, text, inp->behaviour[i], 0);
        }
    }

    while (stack.size > 0) {
        token_push(&output, stack.type[stack.size - 1],
                   stack.text[stack.size - 1],
                   stack.behaviour[stack.size - 1], 0);
        stack.size--;
    }

    token_push(&output, TOKEN_EOF, "EOF", BHV_UNDEFINED, 0);
    return output;
}

void print_token(Token *tk){
  for (size_t i=0; i<tk->size; ++i){
    printf("TokenNum: %zu Type: %s Value: %s\n", i, tk->tktype[i], tk->text[i]);
  }
}


void skip_space(Token *inp, size_t *idx){
  while (inp->type[*idx] == TOKEN_SPACE || inp->type[*idx] == TOKEN_NEWLINE) (*idx)++;
}


Token slice_token(Token *inp, size_t a, size_t z){ // probably should be implemented in lexer but not bothered
  Token t = {0};
  token_init(&t, z-a+1);
  for (size_t i=a; i<z; ++i){
    token_push(&t, inp->type[i], inp->text[i], inp->behaviour[i], inp->cursor_skip[i]);
  }
  return t;
}

Token parse_statement(Token *inp, size_t *idx, SymbolTable *sym){
  skip_space(inp, idx);

  if (inp->type[*idx] == TOKEN_LET){
    (*idx)++;
    skip_space(inp, idx);

    if (inp->type[*idx] != TOKEN_IDENTIFIER){
      fprintf(stderr, "Expected Identifier after 'let'");
      exit(1);
    }

    char *var_name = inp->text[*idx];
    (*idx)++;
    skip_space(inp, idx);

    if (inp->type[*idx] != TOKEN_EQU){
      fprintf(stderr, "Expected '=' after identifier");
      exit(1);
    }
    (*idx)++;
    skip_space(inp, idx);

    size_t expr_start = *idx;
    while (inp->type[*idx] != TOKEN_SEMI && inp->type[*idx] != TOKEN_EOF){
      (*idx)++;
    }

    size_t expr_end = *idx;
    Token expr = slice_token(inp, expr_start, expr_end);
    Token rpn = build_rpn(&expr, sym);


    Symbol exprn = 
    {
      .name=strdup(var_name),
      .symbol_kind = SYM_VAR,
      .builtin = false,
      .ret_type = TOKEN_UNKNOWN
    }; 
    symbol_table_add(sym, exprn);

    skip_space(inp, idx);
    if (inp->type[*idx] == TOKEN_SEMI) {
      (*idx)++;
      skip_space(inp, idx);
      return rpn;
    }
  } else if (inp->type[*idx] == TOKEN_RETURN) {
      (*idx)++;
      skip_space(inp, idx);

      size_t expr_start = *idx;
      while (inp->type[*idx] != TOKEN_SEMI && inp->type[*idx] != TOKEN_EOF){
          (*idx)++;
      }
      size_t expr_end = *idx;

      Token expr = slice_token(inp, expr_start, expr_end);
      Token rpn = build_rpn(&expr, sym);
      (*idx)++;
      if (inp->type[*idx] == TOKEN_SEMI) {
        (*idx)++;
        skip_space(inp, idx);
        return rpn;
      }
  } else {
      fprintf(stderr, "Unexpected statement '%s\n'", inp->text[*idx]);
      exit(1);
  }
}

Block *parse_func_def(Token *inp, size_t *idx, SymbolTable *sym) {
    skip_space(inp, idx);
    if (inp->type[*idx] != TOKEN_FN) {
        fprintf(stderr, "Expected 'fn'\n");
        exit(1);
    }
    (*idx)++;
    skip_space(inp, idx);

    if (inp->type[*idx] != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Expected function name after 'fn'\n");
        exit(1);
    }

    const char *fname = inp->text[*idx];
    (*idx)++;
    skip_space(inp, idx);

    if (inp->type[*idx] != TOKEN_LPAREN) {
        fprintf(stderr, "Expected '('\n");
        exit(1);
    }
    (*idx)++;
    skip_space(inp, idx);

    Symbol func = {0};
    func.name = strdup(fname);
    func.symbol_kind = SYM_FUNC;
    func.ret_type = TOKEN_UNKNOWN;
    func.arg_count = 0;
    func.builtin = false;

    while (inp->type[*idx] != TOKEN_RPAREN) {
        skip_space(inp, idx);
        if (inp->type[*idx] != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Expected argument name\n");
            exit(1);
        }

        (*idx)++;
        skip_space(inp, idx);

        if (inp->type[*idx] != TOKEN_COLON) {
            fprintf(stderr, "Expected ':' after argument name\n");
            exit(1);
        }

        (*idx)++;
        skip_space(inp, idx);

        if (inp->type[*idx] != TOKEN_IDENT_INT) {
            fprintf(stderr, "Expected type after ':'\n");
            exit(1);
        }

        func.arg_types[func.arg_count++] = inp->type[*idx];
        (*idx)++;
        skip_space(inp, idx);

        if (inp->type[*idx] == TOKEN_COMMA) {
            (*idx)++;
            continue;
        } else if (inp->type[*idx] == TOKEN_RPAREN) {
            break;
        } else {
            fprintf(stderr, "Expected ',' or ')' after argument type\n");
            exit(1);
        }
    }

    (*idx)++;
    skip_space(inp, idx);

    if (inp->type[*idx] != TOKEN_IDENT_INT) {
        fprintf(stderr, "Expected return type after ')'\n");
        exit(1);
    }

    func.ret_type = inp->type[*idx];
    (*idx)++;
    skip_space(inp, idx);

    if (inp->type[*idx] != TOKEN_LCURLY) {
        fprintf(stderr, "Expected '{'\n");
        exit(1);
    }

    (*idx)++;
    skip_space(inp, idx);

    Block *block = (Block*)malloc(sizeof(Block));
    block_init(block, 55);
    Token statement = {0};
    
    while (inp->type[*idx] != TOKEN_RCURLY && inp->type[*idx] != TOKEN_EOF) {
        statement = parse_statement(inp, idx, sym);
        skip_space(inp, idx);
        block_append(block, statement);
    }

    if (inp->type[*idx] != TOKEN_RCURLY) {
        fprintf(stderr, "Expected '}' at end of function\n");
        exit(1);
    }

    (*idx)++;
    symbol_table_add(sym, func);
    return block; // TODO: return block aka multiple statements
}



// int main(int argc, char **argv){
//   if (argc < 2) return -1;
//   const char ts[] = "\"hello\" hi + 2 2.312"; 
//   const char math[] = "print(((1+2)*6)/18)"; // = 1
//   const char print[] = "print(\"hello\")";
//   const char simple[] = "1 + (  3 + 3  )/4+4*3";
  

//   char* read = nb_read_file(argv[1]);
//   Token tk = tokenize_all(read);
//   printf("INPUT: %s\n", read);
//   SymbolTable table = {0};
//   symbol_table_init(&table, 32);


//   Token rpn = build_rpn(&tk, &table);
//   print_token(&rpn); 
// }
