#include <stdlib.h>

#include "zyywrap.c"
#include "lex.yy.c"
#include "y.tab.c"

#include "semantic.h"
#include "codeGenerator.h"

int main(int argc, char* argv[]) {

  // Open the file
	yyin = fopen(argv[1], "r");
	if(yyin == NULL) {
		printf("Cannot open input file: %s\n", argv[1]);
		return 0;
	}

  // Lexical and syntactical analysis
  yyparse();
  fclose(yyin);

  // Sentantical analysis and optimization
  analyze_tree();

  if (errors_no > 0) {
    printf("Found %d error(s), cannot continue with compilation.\n", errors_no);
    return 1;
  }

  //  print_tree();

  generate_mixal();

  system("ponysay compiled successfully");
}
