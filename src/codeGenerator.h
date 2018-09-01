#include "tree.h"

#define HEAP_S_ADDRESS 3500
#define START_ADDRESS 100
#define VARIABLE_S_ADDRESS 2000

#define CALC_STACK "CSTACK"
#define TMP "tmp"

int indx = 0;
int bi_index = 0;
int if_index = 0;
int con_index = 0;
int loop_index = 0;

int current_loop = -1;
bool is_current_for = false;

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
  fprintf(_file, "*\n*Automaticly generated mixal code from CMeli.\n*\n\n");

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

void define_stack() {
  fprintf(_file, "\n*CALCULATION HEAP\n");
  fprintf(_file, "%s\t\tEQU\t\t%d\n", CALC_STACK, HEAP_S_ADDRESS);
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

void push_to_stack() {
  if (indx > 0)
    fprintf(_file, "\t\tSTA\t\t\%s+%d\n",CALC_STACK, indx);
  else
    fprintf(_file, "\t\tSTA\t\tCSTACK\n");
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

void calculate_binary_op(node*);

void calculate_un_op(node* n) {
  if (!n)
    return;

  switch (n->children[0]->type) {
    case OpNEG:
      fprintf(_file, "\t\tENTA\t\t0\n");
      calculate_binary_op(n->children[0]->children[0]);
      fprintf(_file, "\t\tSTA\t\t%s\n", TMP);
      fprintf(_file, "\t\tENTA\t\t-1\n");
      fprintf(_file, "\t\tMUL\t\t%s\n", TMP);
      move_rX_to_rA();
      break;
    case OpNOT:
      fprintf(_file, "\t\tENTA\t\t0\n");
      calculate_binary_op(n->children[0]->children[0]);
      fprintf(_file, "\t\tENTX\t\t0\n");
      fprintf(_file, "\t\tJANZ\t\t*+2\n");
      fprintf(_file, "\t\tENTX\t\t1\n");
      move_rX_to_rA();
      break;
    default:
      fprintf(_file, "\t\tENTA\t\t0\n");
      calculate_binary_op(n->children[0]->children[0]);
      break;
  }

}

/* Writes the comparision code block for the given operation. This function is
 * meant to be used inside of calculate_binary_op(node*).
 */
void calculate_comparisson(char* op, node* n) {
  calculate_binary_op(n->children[1]);
  push_to_stack();
  calculate_binary_op(n->children[0]);
  fprintf(_file, "\t\tCMPA\t\t%s+%d\n", CALC_STACK, --indx);
  fprintf(_file, "\t\tENTA\t\t1\n"); // assume the result is true
  fprintf(_file, "\t\tJ%s\t\t*+2\n", op); // check condition
  fprintf(_file, "\t\tENTA\t\t0\n");
}

/* Handles a binopExp and downwards, calculating the corresponding result */
void calculate_binary_op(node* n) {
  if (!n)
    return;

  int local_index;
  switch (n->type) {
    case VUnopExp:
      calculate_un_op(n);
      break;
    case VExp:
    case VBinopExp:
    case VBinop1:
    case VBinop2:
    case VFactor:
      calculate_binary_op(n->children[0]);
      break;
    case BiPLUS:
      calculate_binary_op(n->children[0]);
      push_to_stack();
      calculate_binary_op(n->children[1]);
      fprintf(_file, "\t\tADD\t\t%s+%d\n", CALC_STACK, --indx);
      break;
    case BiMINUS:
      calculate_binary_op(n->children[1]);
      push_to_stack();
      calculate_binary_op(n->children[0]);
      fprintf(_file, "\t\tSUB\t\t%s+%d\n", CALC_STACK, --indx);
      break;
    case BiMULT:
      calculate_binary_op(n->children[0]);
      push_to_stack();
      calculate_binary_op(n->children[1]);
      fprintf(_file, "\t\tMUL\t\t%s+%d\n", CALC_STACK, --indx);
      move_rX_to_rA();
      break;
    case BiDIV:
      calculate_binary_op(n->children[1]);
      push_to_stack();
      calculate_binary_op(n->children[0]);
      move_rA_to_rX();
      fprintf(_file, "\t\tENTA\t\t0\n");
      fprintf(_file, "\t\tENT1\t\t1\n");
      fprintf(_file, "\t\tJXNN\t\t*+2\n"); // Keep the sign
      fprintf(_file, "\t\tENT1\t\t-1\n");
      fprintf(_file, "\t\tST1\t\t%s\n", TMP);
      fprintf(_file, "\t\tDIV\t\t%s+%d\n", CALC_STACK, --indx);
      fprintf(_file, "\t\tMUL\t\t%s\n", TMP);
      move_rX_to_rA();
      break;
    case BiMOD:
      calculate_binary_op(n->children[1]);
      push_to_stack();
      calculate_binary_op(n->children[0]);
      move_rA_to_rX();
      fprintf(_file, "\t\tENTA\t\t0\n");
      fprintf(_file, "\t\tDIV\t\t%s+%d\n", CALC_STACK, --indx);
      move_rX_to_rA();
      break;
    case BiGT:
      calculate_comparisson("G", n);
      break;
    case BiLS:
      calculate_comparisson("L", n);
      break;
    case BiGE:
      calculate_comparisson("GE", n);
      break;
    case BiLE:
      calculate_comparisson("LE", n);
      break;
    case BiEQ:
      calculate_comparisson("E", n);
      break;
    case BiNE:
      calculate_comparisson("NE", n);
      break;
    case BiOR:
      local_index = bi_index++;
      calculate_binary_op(n->children[0]);
      fprintf(_file, "\t\tENTX\t\t1\n");
      fprintf(_file, "\t\tJANZ\t\tOE%d\n", local_index);
      calculate_binary_op(n->children[1]);
      fprintf(_file, "\t\tENTX\t\t0\n");
      fprintf(_file, "\t\tJAZ\t\tOE%d\n", local_index);
      fprintf(_file, "\t\tENTX\t\t1\n");

      fprintf(_file, "OE%d\t\tNOP\n", local_index);
      move_rX_to_rA();
      break;
    case BiAND:
      local_index = bi_index++;

      calculate_binary_op(n->children[0]);
      fprintf(_file, "\t\tENTX\t\t0\n");
      fprintf(_file, "\t\tJAZ\t\tOE%d\n", local_index);
      fprintf(_file, "\t\tENTA\t\t0\n");
      calculate_binary_op(n->children[1]);
      fprintf(_file, "\t\tENTX\t\t0\n");
      fprintf(_file, "\t\tJAZ\t\tOE%d\n", local_index);
      fprintf(_file, "\t\tENTX\t\t1\n");

      fprintf(_file, "OE%d\t\tNOP\n", local_index);
      move_rX_to_rA();
      break;
    case TYPECONSTANT:
      if (n->sym->value < 999999999) {
        fprintf(_file, "\t\tADD");
        fprintf(_file, "\t\t=%d=\n", n->sym->value);
      }
      else {
        fprintf(_file, "\t\tLDA\t\t=999999999=\n");
        fprintf(_file, "\t\tADD \t\t=%d=\n", n->sym->value - 999999999);
      }
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

void parse_and_translate(node*);
void handle_if_stmt(node* n) {
    if (!n)
      return;

    int local_index = if_index++;
    con_index++;

    fprintf(_file, "\n");
    fprintf(_file, "*IF CONDITION\n");
    fprintf(_file, "\t\tENTA\t\t0\n");
    calculate_binary_op(n->children[0]);
    fprintf(_file, "C%d\t\tCON\t\t0\n", con_index); // store the condition
    fprintf(_file, "\t\tSTA\t\tC%d\n", con_index);

    fprintf(_file, "\t\tJAZ\t\tF%d\n", local_index);
    fprintf(_file, "*IF BLOCK\n");
    parse_and_translate(n->children[1]);
    fprintf(_file, "*END IF\n");
    fprintf(_file, "F%d\t\tNOP\n\n", local_index); // jump here if the if condition is not meet.

    // Check for else
    if (n->children_count > 2 && n->children[2]->type == VElseBlock) {
        int local_index = ++if_index;
        fprintf(_file, "*ELSE\n");
        fprintf(_file, "\t\tLDA\t\tC%d\n", con_index);
        fprintf(_file, "\t\tJANZ\t\tF%d\n", local_index);
        parse_and_translate(n->children[2]);
        fprintf(_file, "*END ELSE\n");
        fprintf(_file, "F%d\t\tNOP\n\n", local_index); // jump here if the if condition is not meet.
    }

}

void handle_while_stmt(node* n) {
  if (!n)
    return;

  int local_loop = ++loop_index;
  con_index++;
  current_loop = loop_index;

  fprintf(_file, "\n");
  fprintf(_file, "*WHILE CONDITION\n");
  fprintf(_file, "LS%d\t\tNOP\n", local_loop);
  fprintf(_file, "\t\tENTA\t\t0\n");
  calculate_binary_op(n->children[0]);
  fprintf(_file, "C%d\t\tCON\t\t0\n", con_index); // store the condition
  fprintf(_file, "\t\tSTA\t\tC%d\n", con_index);

  fprintf(_file, "\t\tJAZ\t\tL%d\n", local_loop);
  fprintf(_file, "*WHILE BLOCK\n");
  parse_and_translate(n->children[1]);
  fprintf(_file, "\t\tJSJ\t\tLS%d\n", local_loop); // go to the starting statement
  fprintf(_file, "*END WHILE\n");
  fprintf(_file, "L%d\t\tNOP\n\n", local_loop); // jump here if the while condition is not meet.

  current_loop = -1;
}

void handle_for_stmt(node* n) {
  if (!n)
    return;

  int local_loop = ++loop_index;
  con_index++;
  current_loop = loop_index;
  is_current_for = true;

  fprintf(_file, "\n");
  fprintf(_file, "*FOR CONDITION\n");
  // The assigment
  fprintf(_file, "\t\tENTA\t\t0\n");
  parse_and_translate(n->children[0]);
  // The condition
  fprintf(_file, "LS%d\t\tNOP\n", local_loop);
  fprintf(_file, "\t\tENTA\t\t0\n");
  calculate_binary_op(n->children[1]);
  fprintf(_file, "C%d\t\tCON\t\t0\n", con_index); // store the condition
  fprintf(_file, "\t\tSTA\t\tC%d\n", con_index);

  fprintf(_file, "\t\tJAZ\t\tL%d\n", local_loop);
  fprintf(_file, "*FOR BLOCK\n");
  parse_and_translate(n->children[3]);
  fprintf(_file, "WC%d\t\tNOP\t\t\n", local_loop);
  parse_and_translate(n->children[2]); // the inc/dec operator
  fprintf(_file, "\t\tJSJ\t\tLS%d\n", local_loop); // go to the starting statement
  fprintf(_file, "*END FOR\n");
  fprintf(_file, "L%d\t\tNOP\n\n", local_loop); // jump here if the for condition is not meet.

  current_loop = -1;
  is_current_for = true;
}

void handle_break() {
    fprintf(_file, "\t\tJSJ\t\tL%d\n", current_loop); // go to the end of the current loop
}

void handle_continue() {
    if (is_current_for)
      fprintf(_file, "\t\tJSJ\t\tWC%d\n", current_loop); // go before the inc/dec of the current for loop
    fprintf(_file, "\t\tJSJ\t\tLS%d\n", current_loop); // go to the start of the current while loop
}

/* Parses the tree -for the last time hopefully- from the given node
 * and downwards and creates the symbolic code traslating the grammar.
 */
void parse_and_translate(node* n) {
  if (!n)
    return;

  for (int i = 0; i < n->children_count; i++) {
    // Start the serialization magic
    switch (n->children[i]->type) {
      case TYPEVARIABLE:
      case TYPECONSTANT:
        break;
      case VFOR:
        handle_for_stmt(n->children[i]);
        break;
      case VWHILE:
        handle_while_stmt(n->children[i]);
        break;
      case VBREAK:
        handle_break();
        break;
      case VIF:
        handle_if_stmt(n->children[i]);
        break;
      case VUnopExp:
        calculate_un_op(n->children[i]);
        break;
      case VBinopExp:
        fprintf(_file, "\t\tENTA\t\t0\n");
        calculate_binary_op(n->children[i]);
        break;

      case AssigEQ:
        parse_and_translate(n->children[0]);
        parse_and_translate(n->children[1]);
        if (n->children[0] == NULL)
          break;
        assign_value_to_symbol(n->children[i]->children[0]->sym->assos_name);
        break;

      case VPRINT:
        print_exp(n->children[0]);
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
      define_stack();
      define_variables();
      fprintf(_file, "\n*RUNTIME\n");
      parse_and_translate(root);
      write_end();
  }
  fclose(_file);
  printf("Finished generating code.\n");
}
