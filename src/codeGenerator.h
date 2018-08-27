#include "tree.h"

#define START_ADDRESS 100
#define VARIABLE_S_ADDRESS 2000

FILE* _file;

bool _create_file() {
  _file = fopen("assem.mixal", "w");

  if (_file == NULL) {
    printf("Couldn't create mixal file: Unable to create directive.\n");
    return false;
  }

  return true;
}

void write_header() {
  fprintf(_file, "*\n*Automaticly generated mixal code from la18.\n*\n\n");

  // Create the buffers used for print
  fprintf(_file, "*PRINT BUFFERS\n");
  fprintf(_file, "SBUF\t\tCON\t\t0\n");
  fprintf(_file, "BUF1\t\tCON\t\t0\n");
  fprintf(_file, "BUF2\t\tCON\t\t0\n");

  // The program start
  fprintf(_file, "\n*PROGRAM START\n");
  fprintf(_file, "START\t\tEQU\t\t%d\n", START_ADDRESS);
  fprintf(_file, "\t\tORIG\t\tSTART\n");
}

void define_variables() {
  int i = 1;
  char* stri;
  int address = VARIABLE_S_ADDRESS;
  // Define the variables from the symbols table
  fprintf(_file, "\n*VARIABLES DEFINITION\n");
  symbol* sym = pop_symbol();
  while (sym != NULL) {
    // The variable's runtime name
    stri = malloc(snprintf( NULL, 0, "%d", i) + 1);
    snprintf(stri, snprintf( NULL, 0, "%d", i) + 1, "%d", i);

    char* name = (char*) malloc(sizeof(stri)+sizeof(char));
    name = strcat(name,"V");
    name = strcat(name, stri);
    printf("%s\n", name);

    // Save the assosiated name
    sym->assos_name = name;
    // Print to the file
    fprintf(_file, "%s\t\tEQU\t\t%d\n",name, address);

    address++;
    i++;
    //free(stri);

    sym = pop_symbol();
  }
}

void calculate_binary_op(node* n) {
  if (!n)
    return;

  for (int i = 0; i < n->children_count; i++) {
    calculate_binary_op(n->children[i]);
  }

  switch (n->type) {
    case TYPECONSTANT:
      fprintf(_file, "\t\tADD\t\t%d\n", n->sym->value);
      break;
  }
}

/* Parses the tree -for the last time hopefully- from the given node
 * and downwards and creates the symbolic code traslating the grammar.
 */
void parse_and_translate(node* n) {
  if (!n)
    return;

  for (int i = 0; i < n->children_count; i++) {
    // Start the serialization magic
    switch (n->type) {
      case VBinopExp:
        fprintf(_file, "\t\tENTA\t\t0\n");
        calculate_binary_op(n);
        break;

      default:
        parse_and_translate(n->children[i]);
    }
  }
}

void write_end() {
  // The end of the program
  fprintf(_file, "\n*END OF PROGRAM\n");
  fprintf(_file, "\t\tHLT\n");
  fprintf(_file, "\t\tEND\t\tSTART\n");
}

void generate_mixal() {
  if (_create_file()) {
      write_header();
      define_variables();
      fprintf(_file, "\n*RUNTIME\n");
      parse_and_translate(root);
      write_end();
  }

  printf("Finished generating code.\n");
}
