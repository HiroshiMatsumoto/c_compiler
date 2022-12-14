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

// current token
Token *token;


// error function
void error(char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// return true if next token is op otherwise false
bool consume(char op){
  if(token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

//  
void expect(char op){
  if(token->kind != TK_RESERVED || token->str[0] != op)
    error("'%c' is expected", op);
  token = token->next;  
}

int expect_number(){
  if(token->kind != TK_NUM)
    error("not numeric");
  int val = token->val;
  token = token->next;
  return val;
}


bool at_eof(){
  return token->kind == TK_EOF;
}

// generate new token and connect to cur
Token *new_token(TokenKind kind, Token *cur, char*str){
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str =str;
  cur->next = tok;
  return tok;
  
}

// 
Token *tokenize(char *p){
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*p){
    // skip whitespace
    if(isspace(*p)){
      p++;
      continue;
    }

    if(*p=='+' || *p =='-'){
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if(isdigit(*p)){
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("not tokenizable");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}


char *user_input;

void error_at(char* loc, char* fmt, ...){
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  fprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
  
}


typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // INT
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
};


Node* new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;  
  return node;
}

Node* new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node* expr(){
  Node* node = mul();
  for(;;){
    if(consume('+'))
      node = new_node(ND_ADD, node, mul());
    else if(consume('-'))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}


Node* mul(){
  Node *node = primary();
  for(;;){
    if(consume('*'))
      node = new_node(ND_MUL, node, primary());
    else if(consume('/'))
      node = new_node(ND_DIV, node, primary());
    else
      return node;
  }
}


Node* primary(){
  if(consume('(')){
    Node *node = expr();
    expect(')');
    return node;
  }
  return new_node_num(expect_number());
}

int main(int argc, char **argv) {
  if (argc != 2){
    fprintf(stderr, "not correct number of arguments");
    return 1;
  }

  // char *p = argv[1];
  token = tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  printf("\tmov rax, %d\n", expect_number());

  // assembly output
  while(!at_eof()){
    if(consume('+')){
      printf("\tadd rax, %d\n", expect_number());
      continue;
    }
    expect('-');
    printf("\tsub rax, %d\n", expect_number());
  }
  printf("\tret\n");
  return 0;
  
}

