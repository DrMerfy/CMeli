#include "zyywrap.c"
#include "lex.yy.c"
#include "y.tab.c"

#include "tree.h"

int main(void) {
   yyparse();
   print_tree();
}
