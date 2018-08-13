%{
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "tree.h"
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
program   :   '{' decls stmts '}'               { /*printf("1\n");*/ root = add_two_nodes($2, $3); pop_symbols(); print_tree(root);}
          ;

decls     :   decls decl                        { /*printf("2\n");*/ $$ = add_two_nodes($1, $2); }
          | /* EMPTY */                         { /*printf("3\n");*/ $$ = add_empty_node();  }
          ;

decl      :   VAR ID vars':'type';'             { /*printf("4\n");*/ $$ = add_three_nodes(create_node_variable($2), $3, $5); }
          ;

vars      :   ','ID vars                        { /*printf("5\n");*/ $$ = add_two_nodes(create_node_variable($2), $3); }
          | /*EMPTY*/                           { /*printf("6\n");*/ $$ = add_empty_node();  }
          ;

type      :   INT                               { /*printf("7\n");*/ $$ = create_node_int(); }
          ;

stmts     :   stmts stmt                        { /*printf("8\n");*/ $$ = add_two_nodes($1, $2); }
          |   /*EMPTY*/                         { /*printf("9\n");*/ $$ = add_empty_node();  }
          ;

stmt      :   simp';'                           { /*printf("10\n");*/ $$ = add_node($1); }
          |   control                           { /*printf("11\n");*/ $$ = add_node($1); }
          |   ';'                               { /*printf("12\n");*/ $$ = add_empty_node();  }
          ;

simp      :   ID asop exp                       { /*printf("13\n");*/ $$ = add_three_nodes(create_node_existing_variable($1), $2, $3); }
          |   PRINT exp                         { /*printf("14\n");*/ $$ = add_node($2); }
          ;

control   :   IF'('exp')' block else_blc        { /*printf("15\n");*/ $$ = add_three_nodes($3, $5, $6); }
          |   WHILE'('exp')' block              { /*printf("16\n");*/ $$ = add_two_nodes($3, $5); }
          |   FOR'('simp';'exp';'simp')' block  { /*printf("17\n");*/ $$ = add_four_nodes($3, $5, $7, $9); }
          |   CONTINUE';'                       { /*printf("18\n");*/ $$ = add_empty_node();  }
          |   BREAK';'                          { /*printf("19\n");*/ $$ = add_empty_node();  }
          ;

else_blc  :   ELSE  block                       { /*printf("20\n");*/ $$ = add_node($2); }
          |   /*EMPTY*/                         { /*printf("21\n");*/ $$ = add_empty_node();  }
          ;

block     :   stmt                              { /*printf("22\n");*/ $$ = add_node($1); }
          |   '{'stmts'}'                       { /*printf("23\n");*/ $$ = add_node($2); }
          ;

exp       :   '('exp')'                         { /*printf("24\n");*/ $$ = add_node($2); }
          |   DEC_CONST                         { /*printf("25\n");*/ $$ = create_node_constant(atoi($1)); }
          |   ID                                { /*printf("26\n");*/ $$ = create_node_existing_variable($1); }
          |   unop exp                          { /*printf("27\n");*/ $$ = add_node($2); }
          |   exp binop exp                     { /*printf("28\n");*/ $$ = add_two_nodes($1, $3); }
          ;

asop      :   '='  { $$ = add_empty_node(AssigEQ); }
          |   PLE  { $$ = add_empty_node(AssigPLE); }
          |   MNE  { $$ = add_empty_node(AssigMNE); }
          |   MLE  { $$ = add_empty_node(AssigMLE); }
          |   SBE  { $$ = add_empty_node(AssigSBE); }
          |   MDE  { $$ = add_empty_node(AssigMDE); }
          ;

binop     :   '+'  { $$ = add_empty_node(BiPLUS); }
          |   '-'  { $$ = add_empty_node(BiMINUS); }
          |   '*'  { $$ = add_empty_node(BiMULT); }
          |   '/'  { $$ = add_empty_node(BiSUB); }
          |   '%'  { $$ = add_empty_node(BiDIV); }
          |   '<'  { $$ = add_empty_node(BiLS); }
          |   LE   { $$ = add_empty_node(BiLE); }
          |   '>'  { $$ = add_empty_node(BiGT); }
          |   GE   { $$ = add_empty_node(BiGE); }
          |   EQ   { $$ = add_empty_node(BiEQ); }
          |   NE   { $$ = add_empty_node(BiNE); }
          |   AND  { $$ = add_empty_node(BiAND); }
          |   OR   { $$ = add_empty_node(BiOR); }
          ;

unop      :   '!'  { $$ = add_empty_node(OpNOT); }
          |   '-'  { $$ = add_empty_node(OpNEG); }
          ;

%%

void yyerror(char *s)
{
   fputs(s,stderr); putc('\n',stderr);
}
