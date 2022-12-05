#include <stdio.h>
#include "stdlib.h"
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>


typedef enum {
  TK_RESERVED, // symbols
  TK_NUM, // integer tokens
  TK_EOF, // end of file
} TokenKind;

typedef struct Token Token;

// token structure
struct Token{
  TokenKind kind; // token type
  Token * next; // next token
  int val; // value if TokneKind == TK_NUM
  char *str; // token string
};

int main(int argc, char **argv) {
  if (argc != 2){
    fprintf(stderr, "not correct number of arguments");
    return 1;
  }

  char *p = argv[1];

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  printf("\tmov rax, %ld\n", strtol(p, &p, 10));

  while(*p){
    if(*p == '+'){
      p++;
      printf("\tadd rax, %ld\n", strtol(p, &p, 10));
      continue;
    }

    if(*p == '-'){
      p++;
      printf("\tsub rax, %ld\n", strtol(p, &p, 10));
      continue;
    }

    fprintf(stderr, "unexpexcted character: '%c'\n", *p);
    return 1;
  }

  printf("\tret\n");
  return 0;
  
}

