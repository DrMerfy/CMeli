#include <stdlib.h>

#include "zyywrap.c"
#include "lex.yy.c"
#include "y.tab.c"

#include "semantic.h"
#include "codeGenerator.h"

int main(void) {

  // Lexical and syntactical analysis
  yyparse();

  // Print the created tree
  //print_tree();

  analyze_tree();

  printf("---------Optimized tree--------\n");
  print_tree();
  generate_mixal();

  //system("ponysay compiled successfully");
}
