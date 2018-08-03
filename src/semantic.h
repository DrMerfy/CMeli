#ifndef SEMANTIC_H
#define SEMANTIC_H

/* <------- LANGUAGE SPECIFIC -------> */
#define MAX_LINKS 4
#define MAX_VARIABLES 128

/* <------- GLOBALS -------> */
typedef int bool;
enum { false, true };

int line_number = 1; // The current line on the input file

/* <------- SYMBOL -------> */

typedef struct symbol_struct {
  char* name;
  char type;

  bool tmp;
  bool read_only;
} symbol;

symbol* create_symbol() {

}


/* <------- NODE -------> */

typedef struct node_struct {
  symbol* sym;

  int childer_count;
  struct node_struct* children[MAX_LINKS]; // The possible childer of the node

  int line_number;
} node;

node* create_node() {

}

node* link_nodes() {

}

node* create_variable(char* name) {

}

node* create_constant(int value) {

}

/* <---- |------- WRAPPERS FOR BASIC NODE FUNCTIONS -------> */

node* create_empty_node() {

}

node* link_node(node* n) {

}

node* link_two_nodes(node* n1, node* n2) {

}

node* link_three_nodes(node* n1, node* n2, node* n3) {

}

/* <------- SYMBOLS TABLE -------> */

symbol* symbols_table[MAX_VARIABLES];
symbol* active_symbols[MAX_VARIABLES];
char used_symbols[MAX_VARIABLES];

int symbol_table_index = 0;
int active_symbols_index = 0;

void push_symbol(symbol* sym) {

}

void pop_symbols() {

}

symbol* find_symbol(char* name) {

}


#endif
