#include "./bytecode.c"
#define FG_IMPLEMENTATION
#include "flag.h"
#include "vm.h"

VM global_vm = {0};

int main(int argc, char **argv) {
    char* buf = malloc(sizeof(char*) * bytecode_count);

    buf = nb_append_null(bytecode, bytecode_count);

    Token tk = tokenize_all(buf);
    SymbolTable table = {0};
    symbol_table_init(&table, 32);

    Token rpn = build_rpn(&tk, &table);

    size_t prog_size = 0;
    instruct *prog = rpn_to_bytecode(&rpn, &prog_size);

    VM vm = {
        .program = prog,
        .program_size = prog_size,
        .inst_p = 0,
        .st_p = 0,
        .running = true,
    };
    global_vm = vm;

    vm_run(&vm);
    return 0;
}
