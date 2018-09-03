#ifndef TREE_H
#define TREE_H

#include "macros.h"
#include "out.h"

/* <------- LANGUAGE SPECIFIC -------> */
#define MAX_LINKS 4
#define MAX_VARIABLES 128
#define MAX_INT_SIZE 1073741823 // Maximum size of integer: 2^30 - 1
#define MAX_VARIABLE_LENGTH 80

/* <------- GLOBALS -------> */

int line_number = 1; // The current line on the input file

/* <------- SYMBOL -------> */

typedef struct symbol_struct {
  char* name;
  int value;

  char* assos_name; // Name assosiated with this symbol on the runtime
} symbol;

symbol* create_symbol_constant(int value) {
  if (value > MAX_INT_SIZE) {
    printf("Integer value extended!\n");
    value = 0;
  }

  symbol* sym = (symbol*) malloc(sizeof(symbol));

  sym->name = NULL;
  sym->value = value;

  return sym;
}

symbol* create_symbol_variable(char* name) {
  symbol* sym = (symbol*) malloc(sizeof(symbol));

  sym->name = malloc(MAX_VARIABLE_LENGTH*sizeof(char));
  strcpy(sym->name, name);

  return sym;
}

symbol* create_symbol_type(int type) {
  symbol* sym = (symbol*) malloc(sizeof(symbol));

  sym->name = NULL;
  sym->value = type;

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
    errors_no++;
  }
}

/* Pops a symbol from the symbols table and returns it */
symbol* pop_symbol() {
  if (symbol_table_index > 0) {
    symbol_table_index--;
    return symbols_table[symbol_table_index];
  }
  return NULL;
}

void pop_symbols() {
  printf("------PRINTING VARIABLES-------\n");
  while (symbol_table_index > 0) {
    symbol_table_index--;
    printf("name: %s    ", symbols_table[symbol_table_index]->name);
    printf("value: %d\n", symbols_table[symbol_table_index]->value);
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
  int type; // Which lexical value the node represents
} node;

node* root;

node* add_nodes(int type, int num, node* n1, node* n2, node* n3, node* n4) {
  node* n = (node*) malloc(sizeof(node));

  //printf("%s\n",macro_to_string(type));
  n->sym = NULL;
  if (n1) {
    n->line_number = n1->line_number;
  } else {
    n->line_number = line_number;
  }
  n->type = type;

  n->children_count = num;
  n->children[0] = n1;
  n->children[1] = n2;
  n->children[2] = n3;
  n->children[3] = n4;

  return n;
}

node* create_node(int type, symbol* sym) {
  int i;
  node* n = (node*) malloc(sizeof(node));

  // printf(">\n");
  n->sym = sym;
  n->line_number = line_number;
  n->type = type;

  n->children_count = 0;
  for (i = 0; i < MAX_LINKS; i++) {
    n->children[i] = NULL;
  }

  return n;
}

/* <---- |------- WRAPPERS FOR BASIC NODE FUNCTIONS -------> */

node* add_empty_node(int type) {
  return add_nodes(type, 0, NULL, NULL, NULL, NULL);
}

node* add_node(int type, node* n) {
  return add_nodes(type, 1, n, NULL, NULL, NULL);
}

node* add_two_nodes(int type, node* n1, node* n2) {
  return add_nodes(type, 2, n1, n2, NULL, NULL);
}

node* add_three_nodes(int type, node* n1, node* n2, node* n3) {
  return add_nodes(type, 3, n1, n2, n3, NULL);
}

node* add_four_nodes(int type, node* n1, node* n2, node* n3, node* n4) {
  return add_nodes(type, 4, n1, n2, n3, n4);
}

node* create_node_constant(int value) {
  return create_node(TYPECONSTANT, create_symbol_constant(value));
}

node* create_node_variable(char* name) {

  // Check for already defined variable
  symbol* sym = find_symbol(name);

  if (sym) {
    error(VARDEFINED, line_number, name);
    return create_node(TYPEVARIABLE, sym);
  }

  sym = create_symbol_variable(name);
  push_symbol(sym);


  return create_node(TYPEVARIABLE, sym);
}

node* create_node_existing_variable(char* name) {

  // Check for not defined variable
  symbol* sym = find_symbol(name);

  if (sym == 0) {
    error(VARNOTDEFINED, line_number, name);
  }
  return create_node(TYPEVARIABLE, sym);
}

node* create_node_int() {
  return  create_node(TYPEINT, create_symbol_type(VType));
}

node* create_node_operator(int type) {
  return create_node(TYPEOPERATOR, create_symbol_type(type));
}

void __print_depth(int depth) {
  for (int i = 0; i < depth; i++) {
    printf("-");
    if (i%3 == 0)
      printf("|");
  }
}

void __print_tree(node* n, int depth) {
  if (n == NULL)
    return;

  if (n->children_count == 0) {
    if (n->sym == NULL) {
      if (n->type == VBREAK || n->type == VCONTINUE) {
        __print_depth(depth);
        printf(">");
        print_control(macro_to_string(n->type));
      }
      return;
    }
    __print_depth(depth);
    printf(">");
    if (n->type == TYPEVARIABLE)
      print_variable(n->sym->name);
    else if (n->type == TYPECONSTANT)
      printf("%d\n", n->sym->value);
    else
      printf("%s\n", macro_to_string(n->sym->value));

    return;
  }else {
    __print_depth(depth);

    if (isAssignOperator(n->type))
      print_assign_operator(macro_to_string(n->type));
    else if (isBinaryOperator(n->type))
      print_binary_operator(macro_to_string(n->type));
    else if (isUnOperator(n->type))
      print_un_operator(macro_to_string(n->type));
    else if (isControlOperator(n->type))
      print_control_main(macro_to_string(n->type));
    else printf("%s\n", macro_to_string(n->type));
  }
  depth++;
  for (int i = 0; i < n->children_count; i++) {
    __print_tree(n->children[i], depth);
  }
}

void print_tree() {
  printf("------PRINTING TREE------\n");
  __print_tree(root, 0);
}

// Tree iterators

/* Parses the tree from the given node downwards and finds if it leads to one leaf.
 * If it leads to only one leaf it returns the leaf.
 */
node* _is_one_way(node* n) {
  if (!n)
    return NULL;

  // If it has more than one children, it is not one way
  if (n->children_count > 1)
    return NULL;

  if (n->children_count == 0)
    return n;

  _is_one_way(n->children[0]);
}

/* Parses the tree from the given node downwards and finds the condition realated
 * to the if or while. If the node is not and if or while, it returns null.
 */
node* get_condition(node* n) {
  if (!n)
    return NULL;

  if (n->children_count == 0)
    return NULL;

  return n->children[0];
}

/* Takes as input a while node and iterates the tree downwards from that node
 * searching for a break statement. If it finds it, returns true.
 */
bool has_break(node* n) {
  if (!n)
    return false;

  bool res;
  for (int i = 0; i < n->children_count; i++) {
    res = has_break(n->children[i]);
  }

  if (n->children_count == 0)
    if (n->type == VBREAK) return true; else return false;
  else
    return res;
}

/* Takes as input a assigment (asopExp) node and returns the variable that is assigned. */
node* get_var_from_assigment(node* n) {
  if (!n)
    return NULL;

  return n->children[0]->children[0];
}

/* Takes as input a node and returns a matrix with the variables containted. */
int ind; // This may not be the perfect structured code example. But I cannot get it to work otherwise :c
node** get_vars(node* n, node* nodes[]) {
  if (!n)
    return NULL;

  for (int i = 0; i < n->children_count; i++) {
    get_vars(n->children[i], nodes);
  }

  if (n->type == TYPEVARIABLE) {

    nodes[ind] = n;
    ind++;
  }
  return nodes;
}

// Delete from memory

node* delete_from_node_backward(node* n) {
  for(int i = 0; i < n->children_count; i++) {
    delete_from_node_backward(n->children[i]);
  }

  if (n->type == TYPECONSTANT)
    free(n->sym);
  free(n);
}

#endif
