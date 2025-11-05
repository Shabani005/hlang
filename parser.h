#include "./lexer.h"
#define NB_IMPLEMENTATION
#include "./nb.h"

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


static Symbol builtins[] = {
    { "print", 1, 1, { TOKEN_UNKNOWN }, TOKEN_EOF, SYM_FUNC, true },
};


typedef struct {
  Symbol *symbols;
  size_t size;
  size_t capacity;
} SymbolTable; 


static int builtin_num = sizeof(builtins)/sizeof(builtins[0]);

static SymbolTable global_env = {
  .size = sizeof(builtins)/sizeof(builtins[0]),
  .capacity = sizeof(builtins)/sizeof(builtins[0]),
  .symbols = builtins};


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
    table->symbols = malloc(sizeof(Symbol) * initial_capacity);
    if (!table->symbols) {
        fprintf(stderr, "symbol_table_init: malloc failed\n");
        exit(1);
    }
    table->size = 0;
    table->capacity = initial_capacity;
}

void symbol_table_add(SymbolTable *table, Symbol sym) {
    if (table->size >= table->capacity) {
        table->capacity = (table->capacity == 0) ? 8 : table->capacity * 2;
        table->symbols = realloc(table->symbols, sizeof(Symbol) * table->capacity);
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
