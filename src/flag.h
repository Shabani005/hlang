#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

typedef struct {
  char**    label;
  char**    desc;
  void      (**func_ptr)(void);
  size_t    count;
  size_t    capacity;
} fg_flags;

typedef struct {
  size_t current;
  size_t total;
} fg_progressbar;

#define fg_index fg_naive_index
#define FG_PB_COUNT 33

void   fg_append(fg_flags *flags, char *label, char *desc);
size_t fg_naive_index(fg_flags *flags, const char* value);
void   fg_run(fg_flags *flags, int argc, char** argv);
void   fg_append_ptr(fg_flags *flags, char *label, void (*func)(void));

#ifdef FG_IMPLEMENTATION
void fg_append(fg_flags *flags, char *label, char *desc){
  if (flags->capacity == 0){
    flags->capacity = 128;
    flags->label   = (char**)malloc(sizeof(char*)*flags->capacity);
    flags->desc    = (char**)malloc(sizeof(char*)*flags->capacity); 
  } if (flags->count >= flags->capacity){
    flags->capacity *=2;
    flags->label = (char**)realloc(flags->label, sizeof(char*) * flags->capacity);
    flags->desc  = (char**)realloc(flags->desc, sizeof(char*) * flags->capacity); 
  }
  flags->label[flags->count] = strdup(label);
  flags->desc[flags->count] = strdup(desc);
  flags->count++;
}

void fg_append_ptr(fg_flags *flags, char *label, void(*func)(void)){
  if (flags->capacity == 0){
    flags->capacity = 128;
    flags->label   = (char**)malloc(sizeof(char*)*flags->capacity);
    flags->desc    = (char**)malloc(sizeof(char*)*flags->capacity);
    flags->func_ptr = (void(**) (void)) malloc(sizeof(void(*)(void))*flags->capacity);
  } if (flags->count >= flags->capacity){
    flags->capacity *=2;
    flags->label    = (char**)realloc(flags->label, sizeof(char*) * flags->capacity);
    flags->desc     = (char**)realloc(flags->desc, sizeof(char*) * flags->capacity);
    flags->func_ptr = (void(**) (void)) realloc(flags->func_ptr, sizeof(void(*)(void)) * flags->capacity);
  }
  flags->label[flags->count] = strdup(label);
  flags->desc[flags->count] = "\0";
  flags->func_ptr[flags->count] = (void(*)(void))func;
  // TODO: remove desc null terminator and fix in terms on fg_run to not print \n
  //flags->func_ptr = func;
  // func();
  flags->count++;
}

size_t fg_naive_index(fg_flags *flags, const char* value){
  bool found;
  for(size_t i=0; i<flags->count; ++i){
    if (strcmp(flags->label[i], value) == 0){
      return i;
      found = true;
    }
  }
  if (!found){
    printf("error: unrecognized command-line option ‘%s’.\n", value); // TODO: did you mean %s?
    return -1;
  }
}

void fg_run(fg_flags *flags, int argc, char** argv){
  bool called = false;
  if (flags->count > 0){
    if (argc < 2){
      printf("All commands:\n");
      for (size_t i=0; i<flags->count; ++i){
        if (!flags->desc[i]) flags->desc[i] = "no given desc"; // impossible case
          printf("    --%s, -%s  %s\n", flags->label[i], flags->label[i], flags->desc[i]); //use strlen comp to align by longest string
      }
    } else {
      // here add logic for parsing flags
      for (size_t i=0; i<argc; ++i){
        //printf("%zu\n", i);
        if (argv[i][0] == '-' && argv[i][1] != '-'){ // && argv[i][1] == '-'          //printf("found {-} in %s\n", argv[i]);
          memmove(argv[i], argv[i]+1, strlen(argv[i]));
          if (fg_index(flags, argv[i]) == -1) exit(-1);
          if (flags->func_ptr[fg_index(flags, argv[i])] != NULL && !called){
            flags->func_ptr[fg_index(flags, argv[i])]();
            called = true;
          } else printf("%s\n", flags->desc[fg_index(flags, argv[i])]);
          // in python it would be if flags->labels[argv[i]] != null
       }
       if (argv[i][0] == '-' && argv[i][1] == '-'){
         memmove(argv[i], argv[i]+2, strlen(argv[i]));
         if (fg_index(flags, argv[i]) == -1) exit(-1);
         if (flags->func_ptr[fg_index(flags, argv[i])] != NULL && !called){
           flags->func_ptr[fg_index(flags, argv[i])]();
           called = true;
           
         } else printf("%s\n", flags->desc[fg_index(flags, argv[i])]);
       }
       // TODO: run from func pointer if not NULL
      }  
    }
  }     
}

void fg_progress_update(fg_progressbar *pb, size_t total) {
    pb->total = total;
    pb->current++;

    if (pb->current > total)
        pb->current = total;

    float progress = (float)pb->current / total;
    size_t filled = (size_t)(progress * FG_PB_COUNT);

    char buf[FG_PB_COUNT + 1];
    for (size_t i = 0; i < FG_PB_COUNT; ++i)
        buf[i] = (i < filled) ? '#' : ' ';
    buf[FG_PB_COUNT] = '\0';

    printf("\r[%s] %3.0f%%", buf, progress * 100);
    fflush(stdout);
}
#endif


