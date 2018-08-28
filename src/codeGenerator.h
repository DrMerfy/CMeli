#include "tree.h"

#define HEAP_S_ADDRESS 3500
#define START_ADDRESS 100
#define VARIABLE_S_ADDRESS 2000

#define CALC_HEAP "CHEAP"
#define TMP "tmp"

int indx = 0;

FILE* _file;

bool store_from_a = true;

bool _create_file() {
  _file = fopen("assem.mixal", "w");

  if (_file == NULL) {
    printf("Couldn't create mixal file: Unable to create directive.\n");
    return false;
  }

  return true;
}

void write_header() {
  fprintf(_file, "*\n*Automaticly generated mixal code from CScript.\n*\n\n");

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

void define_heap() {
  fprintf(_file, "\n*CALCULATION HEAP\n");
  fprintf(_file, "%s\t\tEQU\t\t%d\n", CALC_HEAP, HEAP_S_ADDRESS);
  fprintf(_file, "%s\t\tCON\t\t0\n", TMP);

}

void define_variables() {
  int i = 1;
  int address = VARIABLE_S_ADDRESS;
  // Define the variables from the symbols table
  fprintf(_file, "\n*VARIABLES DEFINITION\n");
  symbol* sym = pop_symbol();
  while (sym != NULL) {
    // The variable's runtime name
    char* stri = malloc(snprintf( NULL, 0, "%d", i) + 1);
    snprintf(stri, snprintf( NULL, 0, "%d", i) + 1, "%d", i);

    char* name = (char*) malloc(sizeof(stri)+sizeof(char));
    name = strcat(name,"V");
    name = strcat(name, stri);

    // Save the assosiated name
    sym->assos_name = malloc(sizeof(name));
    strcpy(sym->assos_name, name);
    // Print to the file
    fprintf(_file, "%s\t\tEQU\t\t%d\n",name, address);

    address++;
    i++;
    free(stri);
    free(name);

    sym = pop_symbol();
  }
}

void push_to_heap() {
  if (indx > 0)
    fprintf(_file, "\t\tSTA\t\t\%s+%d\n",CALC_HEAP, indx);
  else
    fprintf(_file, "\t\tSTA\t\tCHEAP\n");
  fprintf(_file, "\t\tENTA\t\t0\n");
  indx++;
}

void move_rX_to_rA() {
  fprintf(_file, "\n");
  fprintf(_file, "\t\tSTX\t\t%s\n", TMP);
  fprintf(_file, "\t\tLDA\t\t%s\n\n", TMP);
}

void move_rA_to_rX() {
    fprintf(_file, "\n");
    fprintf(_file, "\t\tSTA\t\t%s\n", TMP);
    fprintf(_file, "\t\tLDX\t\t%s\n\n", TMP);
}

void calculate_binary_op(node* n) {
  if (!n)
    return;

  switch (n->type) {
    case VExp:
    case VBinopExp:
    case VBinop1:
    case VBinop2:
    case VFactor:
      calculate_binary_op(n->children[0]);
      break;
    case BiPLUS:
      calculate_binary_op(n->children[0]);
      push_to_heap();
      calculate_binary_op(n->children[1]);
      fprintf(_file, "\t\tADD\t\t%s+%d\n", CALC_HEAP, --indx);
      break;
    case BiMULT:
      calculate_binary_op(n->children[0]);
      push_to_heap();
      calculate_binary_op(n->children[1]);
      fprintf(_file, "\t\tMUL\t\t%s+%d\n", CALC_HEAP, --indx);
      move_rX_to_rA();
      break;
    case BiSUB:
      calculate_binary_op(n->children[0]);
      push_to_heap();
      calculate_binary_op(n->children[1]);
      move_rA_to_rX();
      fprintf(_file, "\t\tDIV\t\t%s+%d\n", CALC_HEAP, --indx);
      break;

    case TYPECONSTANT:
      fprintf(_file, "\t\tADD");
      fprintf(_file, "\t\t=%d=\n", n->sym->value);
      break;
    case TYPEVARIABLE:
      fprintf(_file, "\t\tADD");
      fprintf(_file, "\t\t%s\n", n->sym->assos_name);
      break;
  }
}

void assign_value_to_symbol(char* name) {
  if (store_from_a)
    fprintf(_file, "\t\tSTA\t\t%s\n", name);
  else {
    fprintf(_file, "\t\tSTX\t\t%s\n", name);
    fprintf(_file, "\t\tLDA\t\t%s\n", name);
  }
  store_from_a = true;
}

void print_exp(node* n) {
  if (!n)
    return;

  fprintf(_file, "\t\tENTA\t\t0\n");
  calculate_binary_op(n->children[0]);

  fprintf(_file, "\n*PRINTING\n");
  fprintf(_file, "\t\tENTX\t\t44\n"); //Plus sign initialization
  fprintf(_file, "\t\tJANN\t\t*+2\n"); //Check if negative
  fprintf(_file, "\t\tENTX\t\t45\n");
  fprintf(_file, "\t\tSTX\t\tSBUF\n");
  fprintf(_file, "\t\tCHAR\n");
  fprintf(_file, "\t\tSTA\t\tBUF1\n");
  fprintf(_file, "\t\tSTX\t\tBUF2\n");
  fprintf(_file, "\t\tOUT\t\tSBUF(19)\n");
}

/* Parses the tree -for the last time hopefully- from the given node
 * and downwards and creates the symbolic code traslating the grammar.
 */
void parse_and_translate(node* n, node* parent) {
  if (!n)
    return;

  for (int i = 0; i < n->children_count; i++) {
    // Start the serialization magic
    switch (n->children[i]->type) {
      case TYPEVARIABLE:
      case TYPECONSTANT:
        break;

      case VBinopExp:
        fprintf(_file, "\t\tENTA\t\t0\n");
        calculate_binary_op(n->children[i]);
        break;

      case AssigEQ:
        parse_and_translate(n->children[0], n);
        parse_and_translate(n->children[1], n);
        if (n->children[0] == NULL)
          break;
        assign_value_to_symbol(n->children[i]->children[0]->sym->assos_name);
        break;

      case VPRINT:
        print_exp(n->children[0]);
        break;

      default:
        parse_and_translate(n->children[i], n);
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
      define_heap();
      define_variables();
      fprintf(_file, "\n*RUNTIME\n");
      parse_and_translate(root, NULL);
      write_end();
  }

  printf("Finished generating code.\n");
}
