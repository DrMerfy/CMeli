#ifndef TREE_H
#define TREE_H

#include "errors.h"

/* <------- LANGUAGE SPECIFIC -------> */
#define MAX_LINKS 4
#define MAX_VARIABLES 128
#define MAX_INT_SIZE 1073741823 // Maximum size of integer: 2^30 - 1
#define MAX_VARIABLE_LENGTH 81

/* <------- GLOBALS -------> */
typedef int bool;
enum { false, true };

int line_number = 1; // The current line on the input file

/* <------- SYNTACTICAL ANALYSIS CONSTS -------> */
#define VProgram 1
#define VDecls 2
#define VDecl 3
#define VVars 4
#define VType 5
#define VStmts 6
#define VStmt 7
#define VSimp 8
#define VControl 9
#define VAsop 10
#define VExp 11
#define VBlock 12
#define VElseBlock 13
#define VUnop 14
#define VBinop 15

#define AssigEQ 16
#define AssigPLE 17
#define AssigMNE 18
#define AssigMLE 19
#define AssigSBE 20
#define AssigMDE 21

#define BiPLUS 22
#define BiMINUS 23
#define BiMULT 24
#define BiSUB 25
#define BiDIV 26
#define BiLS 27
#define BiLE 28
#define BiGT 29
#define BiGE 30
#define BiEQ 31
#define BiNE 32
#define BiAND 33
#define BiOR 34

#define OpNOT 35
#define OpNEG 36

/* <------- SYMBOL -------> */

typedef struct symbol_struct {
  char* name;
  int value;
  int type;

  bool isType;
  bool tmp;
} symbol;

symbol* create_symbol_constant(int value) {
  if (value > MAX_INT_SIZE) {
    printf("Integer value extended!\n");
    value = 0;
  }

  symbol* sym = (symbol*) malloc(sizeof(symbol));

  sym->name = NULL;
  sym->value = value;

  sym->tmp = true;

  return sym;
}

symbol* create_symbol_variable(char* name) {
  symbol* sym = (symbol*) malloc(sizeof(symbol));

  sym->name = malloc(MAX_VARIABLE_LENGTH*sizeof(char));
  strcpy(sym->name, name);

  sym->isType = false;
  sym->tmp = false;

  return sym;
}

symbol* create_symbol_type(int type) {
  symbol* sym = (symbol*) malloc(sizeof(symbol));

  sym->name = NULL;
  sym->value = type;

  sym->isType = true;
  sym->tmp = true;

  return sym;
}

/* <------- SYMBOLS TABLE -------> */

symbol* symbols_table[MAX_VARIABLES];
symbol* active_symbols[MAX_VARIABLES];
char used_symbols[MAX_VARIABLES];

int symbol_table_index = 0;
int active_symbols_index = 0;

void push_symbol(symbol* sym) {
  if (symbol_table_index < MAX_VARIABLES) {
    symbols_table[symbol_table_index] = sym;
    symbol_table_index++;
  }else {
    printf("MAX VARIABLES OVERFLOW\n");
  }
}

void pop_symbols() {
  printf("------PRINTING VARIABLES-------\n");
  while (symbol_table_index > 0) {
    symbol_table_index--;
    printf("%s\n", symbols_table[symbol_table_index]->name);
  }
}

symbol* find_symbol(char* name) {
  for (int i = 0; i < symbol_table_index; i++) {
    if (strcmp(symbols_table[i]->name, name) == 0) {
      return symbols_table[i];
    }
  }
  return NULL;
}

/* <------- NODE -------> */

typedef struct node_struct {
  symbol* sym;

  int children_count;
  struct node_struct* children[MAX_LINKS];

  int line_number;
} node;

node* root; // The root of the syntax tree

node* add_nodes(int num, node* n1, node* n2, node* n3, node* n4) {
  node* n = (node*) malloc(sizeof(node));

  n->sym = NULL;

  if (n1) {
    n->line_number = n1->line_number;
  } else {
    n->line_number = line_number;
  }

  n->children_count = num;
  n->children[0] = n1;
  n->children[1] = n2;
  n->children[2] = n3;
  n->children[3] = n4;

  return n;
}

node* create_node(symbol* sym) {
  int i;
  node* n = (node*) malloc(sizeof(node));

  n->sym = sym;
  n->line_number = line_number;

  n->children_count = 0;

  for (i = 0; i < MAX_LINKS; i++) {
    n->children[i] = NULL;
  }

  return n;
}

/* <---- |------- WRAPPERS FOR BASIC NODE FUNCTIONS -------> */

node* add_empty_node() {
  return add_nodes(0, NULL, NULL, NULL, NULL);
}

node* add_node(node* n) {
  return add_nodes(1, n, NULL, NULL, NULL);
}

node* add_two_nodes(node* n1, node* n2) {
  return add_nodes(2, n1, n2, NULL, NULL);
}

node* add_three_nodes(node* n1, node* n2, node* n3) {
  return add_nodes(3, n1, n2, n3, NULL);
}

node* add_four_nodes(node* n1, node* n2, node* n3, node* n4) {
  return add_nodes(4, n1, n2, n3, n4);
}

node* create_node_constant(int value) {
  return create_node(create_symbol_constant(value));
}

node* create_node_variable(char* name) {

  // Check for already defined variable
  symbol* sym = find_symbol(name);

  if (sym) {
    error(VARDEFINED, line_number, name);
    return create_node(sym);
  }

  sym = create_symbol_variable(name);
  push_symbol(sym);


  return create_node(sym);
}

node* create_node_existing_variable(char* name) {

  // Check for not defined variable
  symbol* sym = find_symbol(name);

  if (sym == 0) {
    error(VARNOTDEFINED, line_number, name);
  }
  return create_node(sym);
}

node* create_node_int() {
  return  create_node(create_symbol_type(VType));
}

node* create_node_operator(int type) {
  return create_node(create_symbol_type(type));
}

void __print_tree(node* n, int depth) {
  if (n == NULL)
    return;
  if (n->children_count == 0) {
    if (n->sym == NULL)
      return;
    for (int i = 0; i < depth; i++) {
      printf("-");
    }
    if (n->sym->tmp == false)
      printf("%s\n", n->sym->name);
    else if (n->sym->isType == false)
      printf("%d\n", n->sym->value);
    else
      printf("%d\n", n->sym->value);
    return;
  }
  depth++;
  for (int i = 0; i < n->children_count; i++) {
    __print_tree(n->children[i], depth);
  }
}

void print_tree(node* root) {
  printf("------PRINTING TREE------\n");
  __print_tree(root, 0);
}

#endif
