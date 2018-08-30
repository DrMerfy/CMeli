#include <stdlib.h>

#include "zyywrap.c"
#include "lex.yy.c"
#include "y.tab.c"

#include "semantic.h"
#include "codeGenerator.h"

int main(void) {

  // Lexical and syntactical analysis
  yyparse();

  // Sentantical analysis and optimization
  analyze_tree();

  if (errors_no > 0) {
    printf("Found %d error(s), cannot continue with compilation.\n", errors_no);
    return 1;
  }

  printf("---------Optimized tree--------\n");
  print_tree();
  generate_mixal();

  //system("ponysay compiled successfully");
}
