#include "zyywrap.c"
#include "lex.yy.c"
#include "y.tab.c"

#include "semantic.h"

int main(void) {

  // Lexical and syntactical analysis
  yyparse();

  // Print the created tree
  print_tree();

  analyze_tree();

  //printf("---------Optimized tree--------\n");
  //print_tree();


}
