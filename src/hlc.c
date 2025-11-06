#define FG_IMPLEMENTATION
#include "flag.h"
#include "vm.h"


VM global_vm = {0};

void emit_bytecode_call(){
  printf("[debug] emit_bytecode_call() triggered\n");
  emit_bytecode(&global_vm);
}

void flag_handling(int argc, char** argv){
  fg_flags flags = {0};
  fg_append_ptr(&flags, "emit-bytecode", emit_bytecode_call);
  fg_run(&flags, argc, argv);
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
    global_vm = vm;
        
    
    flag_handling(argc, argv);
    vm_run(&vm);    
    return 0;
}
