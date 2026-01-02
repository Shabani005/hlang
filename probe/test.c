#define NB_IMPLEMENTATION
#include "nb.h"

typedef struct {
  char* buf;
  size_t len;
} nb_xxd_info2;

// outname is an experimntal feature

char* nb_xxd2(char* filename, nb_xxd_info2 *info, char* outname){  
  if (!nb_does_file_exist(filename)){
    fprintf(stderr, "File: '%s' does not exist\n", filename);
    return NULL;
  }
  
  FILE *f = fopen(filename, "rb");
  fseek(f, 0, SEEK_END);
  size_t fsize = ftell(f);

  unsigned char *buf = malloc(fsize);

  fseek(f, 0, SEEK_SET);  
  fread(buf, 1, sizeof(char)*fsize, f);
  buf[fsize+1] = '\0';

  // TODO: unhardcode 

  char *newbuf = (char*)malloc(sizeof(char) * fsize * 20 + 1);
  char *p = newbuf;


  char* otherbuf = (char*)malloc(sizeof(char) * fsize * 30 + 1);
  char* p2 = otherbuf;  

  size_t count = 0; 
 
  // char test[2] = {0x31, 0x30}; 
 
  // should probably do <= to ignore last thing

  for (size_t i=1; i+1 < fsize; ++i){
    p += sprintf(p, "0x%02x, ", buf[i]);
    count++;
  }
  info->len = count;

  *p = '\0';
  newbuf[count*10] = '\0';

  // p should be null terminated?
  // 
  p2 += sprintf(p2, "char %s[%zu] = {", outname, count);
  p2 += sprintf(p2, newbuf); // only printf to -1 to ignore last comma
  p2 += sprintf(p2, "};\n");

  p2 += sprintf(p2, "int %s_count = %zu;", outname, count);
  
  // printf("%s\n", newbuf);
  // printf("%zu\n", count);
  // printf("count: %zu\n", count);
  // *p = '\0';
  *p2 = '\0';
  return otherbuf;

  fclose(f);
}



int main(void){
  nb_xxd_info2 info = {0};
  printf("%s\n", nb_xxd2("./main.hl", &info, "hi"));
  return 0;
}
