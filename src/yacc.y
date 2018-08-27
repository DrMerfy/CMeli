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
%type <nonterm> asopExp exp block else_blc unopExp binopExp binop1 binop2 factor

%start program

// PRINT REDEX printf\("(\d+)\\n"\);  => /*printf("$1\\n");*/

%%
program   :   '{' decls stmts '}'               { /*printf("1\n");*/ root = add_two_nodes(VProgram, $2, $3); }
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

stmt      :   simp';'                           { /*printf("10\n");*/ $$ = add_node(VSimp, $1); }
          |   control                           { /*printf("11\n");*/ $$ = add_node(VControl, $1); }
          |   ';'                               { /*printf("12\n");*/ $$ = add_empty_node(VStmt);  }
          // Pseudo error
          |   simp                              { $$ = 0; error_message("Missing ';'.", line_number - 1, NULL, NULL); }
          ;

simp      :   asopExp                           { /*printf("13\n");*/ $$ =  add_node(VAsopExp, $1); }
          |   PRINT exp                         { /*printf("14\n");*/ $$ = add_node(VPRINT, $2); }
          ;

control   :   IF'('exp')' block else_blc        { /*printf("15\n");*/ $$ = add_three_nodes(VIF, $3, $5, $6); }
          |   WHILE'('exp')' block              { /*printf("16\n");*/ $$ = add_two_nodes(VWHILE, $3, $5); }
          |   FOR'('simp';'exp';'simp')' block  { /*printf("17\n");*/ $$ = add_four_nodes(VFOR, $3, $5, $7, $9); }
          |   CONTINUE';'                       { /*printf("18\n");*/ $$ = add_empty_node(VCONTINUE);  }
          |   BREAK';'                          { /*printf("19\n");*/ $$ = add_empty_node(VBREAK);  }
          // Errors
          |   IF error                          { $$ = 0; error_message("Missing condition ", line_number, NULL, "in if statement"); }
          |   IF '('exp error                   { $$ = 0; error_message("Missmatched starting '('.", line_number, NULL, NULL); }
          |   WHILE error                       { $$ = 0; error_message("Missing condition ", line_number, NULL, "in while statement"); }
          |   WHILE '('exp error                { $$ = 0; error_message("Missmatched starting '('.", line_number, NULL, NULL); }
          |   FOR error                         { $$ = 0; error_message("Missing condition ", line_number, NULL, "in for statement"); }
          |   FOR '('simp';'exp';'error         { $$ = 0; error_message("Missing loop operation ", line_number, NULL, "in for statement"); }
          |   FOR '('simp';'exp';'simp error    { $$ = 0; error_message("Missmatched starting '('.", line_number, NULL, NULL); }

          ;

else_blc  :   ELSE  block                       { /*printf("20\n");*/ $$ = add_node(VElseBlock, $2); }
          |   /*EMPTY*/                         { /*printf("21\n");*/ $$ = add_empty_node(VElseBlock);  }
          ;

block     :   stmt                              { /*printf("22\n");*/ $$ = add_node(VStmt, $1); }
          |   '{'stmts'}'                       { /*printf("23\n");*/ $$ = add_node(VStmts, $2); }
          |   error                             { $$ = 0; error_message("Expected statement.", line_number, NULL, NULL); }
          ;

exp       :   '('exp')'                         { /*printf("24\n");*/ $$ = add_node(VExp, $2); }
          |   unopExp                           { /*printf("27\n");*/ $$ = add_node(VUnopExp, $1); }
          |   binopExp                          { /*printf("28\n");*/ $$ = add_node(VBinopExp, $1); }
          // ERRORS
          | '('exp error                        { $$ = 0; error_message("Missmatched starting '('.", line_number, NULL, NULL);}
          ;

asopExp   :   ID '=' exp  { $$ = add_two_nodes(AssigEQ, create_node_existing_variable($1), $3); }
          |   ID PLE exp  { $$ = add_two_nodes(AssigPLE, create_node_existing_variable($1), $3); }
          |   ID MNE exp  { $$ = add_two_nodes(AssigMNE, create_node_existing_variable($1), $3); }
          |   ID MLE exp  { $$ = add_two_nodes(AssigMLE, create_node_existing_variable($1), $3); }
          |   ID SBE exp  { $$ = add_two_nodes(AssigSBE, create_node_existing_variable($1), $3); }
          |   ID MDE exp  { $$ = add_two_nodes(AssigMDE, create_node_existing_variable($1), $3); }
          // ERRORS
          |   ID error    { $$ = 0; char* lookahead = malloc(sizeof(char)); lookahead[0] = yychar; error_message("Expected assigment but got ", line_number, lookahead, "."); }

          ;

binopExp  :   binop1 '<' binop1  { $$ = add_two_nodes(BiLS, $1, $3); }
          |   binop1 LE binop1   { $$ = add_two_nodes(BiLE, $1, $3); }
          |   binop1 '>' binop1  { $$ = add_two_nodes(BiGT, $1, $3); }
          |   binop1 GE binop1   { $$ = add_two_nodes(BiGE, $1, $3); }
          |   binop1 EQ binop1   { $$ = add_two_nodes(BiEQ, $1, $3); }
          |   binop1 NE binop1   { $$ = add_two_nodes(BiNE, $1, $3); }
          |   binop1 AND binop1  { $$ = add_two_nodes(BiAND, $1, $3); }
          |   binop1 OR binop1   { $$ = add_two_nodes(BiOR, $1, $3); }
          |   binop1             { $$ = add_node(VBinop1, $1); }
          ;

binop1    :   binop1 '+' binop2   { $$ = add_two_nodes(BiPLUS, $1, $3); }
          |   binop1 '-' binop2   { $$ = add_two_nodes(BiMINUS, $1, $3); }
          |   binop2              { $$ = add_node(VBinop2, $1); }
          ;

binop2    :   binop2 '*' factor        { $$ = add_two_nodes(BiMULT, $1, $3); }
          |   binop2 '/' factor        { $$ = add_two_nodes(BiSUB, $1, $3); }
          |   binop2 '%' factor        { $$ = add_two_nodes(BiDIV, $1, $3); }
          |   factor                   { $$ = add_node(VFactor, $1); }
          ;

factor    :   DEC_CONST                         { /*printf("25\n");*/ $$ = create_node_constant(atoi($1)); }
          |   ID                                { /*printf("26\n");*/ $$ = create_node_existing_variable($1); }
          |   '('exp')'                         { $$ = add_node(VExp, $2); }
          ;

unopExp      :   '!' exp  { $$ = add_node(OpNOT, $2); }
          |   '-' exp  { $$ = add_node(OpNEG, $2); }
          ;

%%

void yyerror(char *s)
{
   printf("%d\n", line_number);
   fputs(s,stderr); putc('\n',stderr);
}
