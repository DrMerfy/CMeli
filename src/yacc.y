%{
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "tree.h"

extern node* root;
%}


%union
{
  int yint;
  char ystr[81];
  struct node_struct* nonterm;
}

%token <yint> BREAK CONTINUE ELSE FOR IF INT PRINT VAR WHILE
%token <ystr> DEC_CONST
%token <ystr> '(' ')' '{' '}'
%token <ystr> ',' ';' ':'
%token <ystr> ID

%right '!'
%right '=' PLE MNE MLE SBE MDE

%left EQ NE
%left '>' GE '<' LE
%left '+' '-'
%left '*' '/' '%'
%left AND OR

%type <nonterm> program decls decl vars type stmts stmt simp control
%type <nonterm> asop exp block else_blc unop binop

%start program

// PRINT REDEX printf\("(\d+)\\n"\);  => /*printf("$1\\n");*/

%%
program   :   '{' decls stmts '}'               { /*printf("1\n");*/ root = add_two_nodes(VProgram, $2, $3); pop_symbols(); /*print_tree(root);*/}
          |   '{' decls stmts                   { printf("1.1\n"); root = add_two_nodes(VProgram, $2, $3); error_message("Missing terminating '}'.",0, NULL, NULL); }
          |       decls stmts                   { printf("1.2\n"); root = add_two_nodes(VProgram, $1, $2); error_message("Missing starting '{'.", 1, NULL, NULL); }
          //|   error                             { printf("1.3\n"); root = 0; error_message("Missing starting '{'.", 1, NULL, NULL); }
          ;

decls     :   decls decl                        { /*printf("2\n");*/ $$ = add_two_nodes(VDecls, $1, $2); }
          | /* EMPTY */                         { /*printf("3\n");*/ $$ = add_empty_node(VDecls);  }
          ;

decl      :   VAR ID vars':'type';'             { /*printf("4\n");*/ $$ = add_three_nodes(VDecl, create_node_variable($2), $3, $5); }
          // ERRORS
          |   VAR ID vars':'type error          { $$ = 0; error_message("Missing ';'", line_number - 1, NULL, " after type declaration."); yyerrok;}
          |   VAR ID error                      { $$ = 0; error_message("Expected ':' or ',' after variable ", line_number, $2, "."); }
          |   VAR error';'                      { $$ = 0; error_message("Expected variable name.", line_number, NULL, NULL); }
          |   VAR error                         { $$ = 0; error_message("Invalid variable definition.", line_number, NULL, NULL); }
          ;

vars      :   ','ID vars                        { /*printf("5\n");*/ $$ = add_two_nodes(VVars, create_node_variable($2), $3); }
          | /*EMPTY*/                           { /*printf("6\n");*/ $$ = add_empty_node(VVars);  }
          |   ','error                          { $$ = 0; error_message("Expected variable name after ','.", line_number, NULL, NULL); }
          ;

type      :   INT                               { /*printf("7\n");*/ $$ = create_node_int(); }
          |   error                             { $$ = 0; error_message("Expected type definition.", line_number, NULL, NULL); }
          ;

stmts     :   stmts stmt                        { /*printf("8\n");*/ $$ = add_two_nodes(VStmts, $1, $2); }
          |   /*EMPTY*/                         { /*printf("9\n");*/ $$ = add_empty_node(VStmts);  }
          ;

stmt      :   simp';'                           { /*printf("10\n");*/ $$ = add_node(VStmt, $1); }
          |   control                           { /*printf("11\n");*/ $$ = add_node(VStmt, $1); }
          |   ';'                               { /*printf("12\n");*/ $$ = add_empty_node(VStmt);  }
          // Pseudo error
          |   simp                              { $$ = 0; error_message("Missing ';'.", line_number - 1, NULL, NULL); }
          ;

simp      :   ID asop exp                       { /*printf("13\n");*/ $$ = add_three_nodes(VSimp, create_node_existing_variable($1), $2, $3); }
          |   PRINT exp                         { /*printf("14\n");*/ $$ = add_node(VSimp, $2); }
          // ERRORS
          |   ID error                          { $$ = 0; char* lookahead = malloc(sizeof(char)); lookahead[0] = yychar; error_message("Expected assigment but got ", line_number, lookahead, "."); }
          ;

control   :   IF'('exp')' block else_blc        { /*printf("15\n");*/ $$ = add_three_nodes(VControl, $3, $5, $6); }
          |   WHILE'('exp')' block              { /*printf("16\n");*/ $$ = add_two_nodes(VControl, $3, $5); }
          |   FOR'('simp';'exp';'simp')' block  { /*printf("17\n");*/ $$ = add_four_nodes(VControl, $3, $5, $7, $9); }
          |   CONTINUE';'                       { /*printf("18\n");*/ $$ = add_empty_node(VControl);  }
          |   BREAK';'                          { /*printf("19\n");*/ $$ = add_empty_node(VControl);  }
          // Pseudo errors
          //|   IF'('')' block else_blc           { $$ = add_three_nodes($3, $5, $6); error_message("Condition always false.", line_number, NULL, NULL); }
          // Errors
          |   IF error                          { $$ = 0; error_message("Missing condition ", line_number, NULL, "in if statement"); }
          |   IF '('exp error                   { $$ = 0; error_message("Missmatched starting '('.", line_number, NULL, NULL); }
          |   WHILE error                       { $$ = 0; error_message("Missing condition ", line_number, NULL, "in while statement"); }
          |   WHILE '('exp error                { $$ = 0; error_message("Missmatched starting '('.", line_number, NULL, NULL); }
          |   FOR error                         { $$ = 0; error_message("Missing condition ", line_number, NULL, "in for statement"); }
          |   FOR '('simp';'exp';'simp error    { $$ = 0; error_message("Missmatched starting '('.", line_number, NULL, NULL); }

          ;

else_blc  :   ELSE  block                       { /*printf("20\n");*/ $$ = add_node(VElseBlock, $2); }
          |   /*EMPTY*/                         { /*printf("21\n");*/ $$ = add_empty_node(VElseBlock);  }
          ;

block     :   stmt                              { /*printf("22\n");*/ $$ = add_node(VBlock, $1); }
          |   '{'stmts'}'                       { /*printf("23\n");*/ $$ = add_node(VBlock, $2); }
          |   error                             { $$ = 0; error_message("Expected statement.", line_number, NULL, NULL); }
          ;

exp       :   '('exp')'                         { /*printf("24\n");*/ $$ = add_node(VExp, $2); }
          |   DEC_CONST                         { /*printf("25\n");*/ $$ = create_node_constant(atoi($1)); }
          |   ID                                { /*printf("26\n");*/ $$ = create_node_existing_variable($1); }
          |   unop exp                          { /*printf("27\n");*/ $$ = add_two_nodes(VExp, $1, $2); }
          |   exp binop exp                     { /*printf("28\n");*/ $$ = add_three_nodes(VExp, $1, $2, $3); }

          // ERRORS
          | '('exp error                        { $$ = 0; error_message("Missmatched starting '('.", line_number, NULL, NULL);}
          ;

asop      :   '='  { $$ = create_node_operator(AssigEQ); }
          |   PLE  { $$ = create_node_operator(AssigPLE); }
          |   MNE  { $$ = create_node_operator(AssigMNE); }
          |   MLE  { $$ = create_node_operator(AssigMLE); }
          |   SBE  { $$ = create_node_operator(AssigSBE); }
          |   MDE  { $$ = create_node_operator(AssigMDE); }
          ;

binop     :   '+'  { $$ = create_node_operator(BiPLUS); }
          |   '-'  { $$ = create_node_operator(BiMINUS); }
          |   '*'  { $$ = create_node_operator(BiMULT); }
          |   '/'  { $$ = create_node_operator(BiSUB); }
          |   '%'  { $$ = create_node_operator(BiDIV); }
          |   '<'  { $$ = create_node_operator(BiLS); }
          |   LE   { $$ = create_node_operator(BiLE); }
          |   '>'  { $$ = create_node_operator(BiGT); }
          |   GE   { $$ = create_node_operator(BiGE); }
          |   EQ   { $$ = create_node_operator(BiEQ); }
          |   NE   { $$ = create_node_operator(BiNE); }
          |   AND  { $$ = create_node_operator(BiAND); }
          |   OR   { $$ = create_node_operator(BiOR); }
          ;

unop      :   '!'  { $$ = create_node_operator(OpNOT); }
          |   '-'  { $$ = create_node_operator(OpNEG); }
          ;

%%

void yyerror(char *s)
{
   printf("%d\n", line_number);
   fputs(s,stderr); putc('\n',stderr);
}
