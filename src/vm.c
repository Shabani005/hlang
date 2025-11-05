#include "parser.h"
#include <string.h>

typedef enum {
  OP_PUSH_INT,
  OP_PUSH_FLOAT,
  OP_PUSH_STRING,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_PRINT,
  OP_HALT
} OPcode;

typedef struct {
  OPcode op;
  double num;
  char *strlit;
} instruct;

typedef enum {
    VAL_INT,
    VAL_FLOAT,
    VAL_STRING,
} ValueType;

typedef struct {
    ValueType type;
    union {
        long i;
        double f;
        char *s;
    };
} Value;

typedef struct {
  instruct *program;
  size_t inst_p;
  size_t program_size;
  Value stack[256];
  size_t st_p;
  bool running;
} VM;

instruct *rpn_to_bytecode(Token *rpn, size_t *out){
  size_t cap = 64;
  size_t size = 0;

  instruct *prog = malloc(sizeof(instruct) * cap);

  for (size_t i=0; i<rpn->size; ++i){
    symbols t = rpn->type[i];
    const char *text = rpn->text[i];

    instruct ins = {0};

    switch (t){
      case TOKEN_INTEGER: ins.op = OP_PUSH_INT; ins.num = atof(text); break;
      case TOKEN_FLOAT: ins.op = OP_PUSH_FLOAT; ins.num = atof(text); break;
      case TOKEN_STRING: ins.op = OP_PUSH_STRING; ins.strlit = strdup(text); break;
      case TOKEN_PLUS: ins.op = OP_ADD; break;
      case TOKEN_MINUS: ins.op = OP_SUB; break;
      case TOKEN_MUL: ins.op = OP_MUL; break;
      case TOKEN_DIV: ins.op = OP_DIV; break;

      case TOKEN_IDENTIFIER:
        if (strcmp(text, "print") == 0) {
            ins.op = OP_PRINT;
        } else {
            printf("[WARNING] Uknown Identifier '%s'\n", text);
        }
        break; //TODO: unhardcode this
      case TOKEN_EOF: ins.op = OP_HALT; break;
      default: continue;
    }
    if (size >= cap){
      cap*=2;
      prog = realloc(prog, sizeof(instruct)*cap);
    }
    prog[size++] = ins;
  }
  *out = size;
  return prog;
}

void vm_run(VM *vm) {
    vm->running = true;
    vm->inst_p = 0;
    vm->st_p = 0;

    while (vm->running && vm->inst_p < vm->program_size) {
        instruct ins = vm->program[vm->inst_p++];

        switch (ins.op) {
        case OP_PUSH_INT: {
            Value v = { .type = VAL_INT, .i = ins.num };
            vm->stack[vm->st_p++] = v;
        } break;

        case OP_PUSH_FLOAT: {
            Value v = { .type = VAL_FLOAT, .f = ins.num };
            vm->stack[vm->st_p++] = v;
        } break;

        case OP_PUSH_STRING: {
            Value v = { .type = VAL_STRING, .s = strdup(ins.strlit) };
            vm->stack[vm->st_p++] = v;
        } break;

        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV: {
            if (vm->st_p < 2) {
                fprintf(stderr, "not enough values on stack.\n");
                vm->running = false;
                break;
            }

            Value b = vm->stack[--vm->st_p];
            Value a = vm->stack[--vm->st_p];

            double av = (a.type == VAL_INT) ? a.i : a.f;
            double bv = (b.type == VAL_INT) ? b.i : b.f;
            double result = 0;

            switch (ins.op) {
            case OP_ADD: result = av + bv; break;
            case OP_SUB: result = av - bv; break;
            case OP_MUL: result = av * bv; break;
            case OP_DIV:
                if (bv == 0) {
                    fprintf(stderr, "division by zero.\n");
                    vm->running = false;
                } else result = av / bv;
                break;
            default: break;
            }

            Value v = { .type = VAL_FLOAT, .f = result };
            vm->stack[vm->st_p++] = v;
        } break;

        case OP_PRINT: {
            if (vm->st_p == 0) {
                fprintf(stderr, "cant print an empty stack\n");
                vm->running = false;
                break;
            }

            Value v = vm->stack[--vm->st_p];
            switch (v.type) {
            case VAL_INT:   printf("%ld\n", v.i); break;
            case VAL_FLOAT: printf("%g\n", v.f); break;
            case VAL_STRING:
                printf("%s\n", v.s);
                free(v.s);
                break;
            }
        } break;

        case OP_HALT:
            vm->running = false;
            break;

        default:
            fprintf(stderr, "unknown opcode %d\n", ins.op);
            vm->running = false;
            break;
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source file>\n", argv[0]);
        return 1;
    }

    char* read = nb_read_file(argv[1]);
    //printf("INPUT: %s\n", read);

    Token tk = tokenize_all(read);
    SymbolTable table = {0};
    symbol_table_init(&table, 32);

    Token rpn = build_rpn(&tk, &table);
    //print_token(&rpn);

    size_t prog_size = 0;
    instruct *prog = rpn_to_bytecode(&rpn, &prog_size);
    VM vm = {
        .program = prog,
        .program_size = prog_size,
        .inst_p = 0,
        .st_p = 0,
        .running = true,
    };

    vm_run(&vm);

    return 0;
}
