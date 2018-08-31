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

%nonassoc IF_ELSE
%nonassoc BREAK CONTINUE ELSE FOR IF INT PRINT VAR WHILE
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
//             /\*printf\("(\d+)\\n"\);\*/ => printf("$1\\n");

%%
program   :   '{' decls stmts '}'               { /*printf("1\n");*/ root = add_two_nodes(VProgram, $2, $3); }
          |   '{' decls stmts                   { printf("1.1\n"); root = add_two_nodes(VProgram, $2, $3); error_message("Missing terminating '}'.",0, NULL, NULL); errors_no++; }
          |       decls stmts                   { printf("1.2\n"); root = add_two_nodes(VProgram, $1, $2); error_message("Missing starting '{'.", 1, NULL, NULL); errors_no++; }
          //|   error                             { printf("1.3\n"); root = 0; error_message("Missing starting '{'.", 1, NULL, NULL); }
          ;

decls     :   decls decl                        { /*printf("2\n");*/ $$ = add_two_nodes(VDecls, $1, $2); }
          | /* EMPTY */                         { /*printf("3\n");*/ $$ = add_empty_node(VDecls);  }
          ;

decl      :   VAR ID vars':'type';'             { /*printf("4\n");*/ $$ = add_three_nodes(VDecl, create_node_variable($2), $3, $5); }
          // ERRORS
          // |   VAR ID vars':'type error          { $$ = 0; error_message("Missing ';'", line_number - 1, NULL, " after type declaration."); yyerrok; errors_no++; }
          // |   VAR ID error                      { $$ = 0; error_message("Expected ':' or ',' after variable ", line_number, $2, "."); errors_no++; }
          // |   VAR error';'                      { $$ = 0; error_message("Expected variable name.", line_number, NULL, NULL); errors_no++; }
          // |   VAR error                         { $$ = 0; error_message("Invalid variable definition.", line_number, NULL, NULL); errors_no++; }
          ;

vars      :   ','ID vars                        { /*printf("5\n");*/ $$ = add_two_nodes(VVars, create_node_variable($2), $3); }
          | /*EMPTY*/                           { /*printf("6\n");*/ $$ = add_empty_node(VVars);  }
          // |   ','error                          { $$ = 0; error_message("Expected variable name after ','.", line_number, NULL, NULL); errors_no++; }
          ;

type      :   INT                               { /*printf("7\n");*/ $$ = create_node_int(); }
          // |   error                             { $$ = 0; error_message("Expected type definition.", line_number, NULL, NULL); errors_no++; }
          ;

stmts     :   stmts stmt                        { /*printf("8\n");*/ $$ = add_two_nodes(VStmts, $1, $2); }
          |   /*EMPTY*/                         { /*printf("9\n");*/ $$ = add_empty_node(VStmts);  }
          ;

stmt      :   simp';'                           { /*printf("10\n");*/ $$ = add_node(VSimp, $1); }
          |   control                           { /*printf("11\n");*/ $$ = add_node(VControl, $1); }
          |   ';'                               { /*printf("12\n");*/ $$ = add_empty_node(VStmt);  }
          // Pseudo errors_no
          // |   simp                              { $$ = 0; error_message("Missing ';'.", line_number - 1, NULL, NULL); errors_no++; }
          // |   decl                              { $$ = 0; error_message("Declaration is only allowed at the start of the program.", line_number - 1, NULL, NULL); errors_no++; }
          // |   ELSE                              { $$ = 0; error_message("Unmatched else statement.", line_number, NULL, NULL); errors_no++; }
          // |   error                             { $$ = 0; char* lookahead = malloc(sizeof(char)); lookahead[0] = yychar; error_message("Unexpected ", line_number, lookahead, "."); errors_no++; }
          ;

simp      :   asopExp                           { /*printf("13\n");*/ $$ =  add_node(VAsopExp, $1); }
          |   PRINT exp                         { /*printf("14\n");*/ $$ = add_node(VPRINT, $2); }
          ;

control   :   IF'('exp')' block %prec IF_ELSE        { /*printf("15\n");*/ $$ = add_two_nodes(VIF, $3, $5); }
          |   IF'('exp')' block else_blc        { /*printf("15\n");*/ $$ = add_three_nodes(VIF, $3, $5, $6); }
          |   WHILE'('exp')' block              { /*printf("16\n");*/ $$ = add_two_nodes(VWHILE, $3, $5); }
          |   FOR'('simp';'exp';'simp')' block  { /*printf("17\n");*/ $$ = add_four_nodes(VFOR, $3, $5, $7, $9); }
          |   CONTINUE';'                       { /*printf("18\n");*/ $$ = add_empty_node(VCONTINUE);  }
          |   BREAK';'                          { /*printf("19\n");*/ $$ = add_empty_node(VBREAK);  }
          // Errors
          // |   IF error                          { $$ = 0; error_message("Missing condition ", line_number, NULL, "in if statement"); errors_no++; }
          // |   IF '('exp error                   { $$ = 0; error_message("Missmatched starting '('.", line_number, NULL, NULL); errors_no++; }
          // |   WHILE error                       { $$ = 0; error_message("Missing condition ", line_number, NULL, "in while statement"); errors_no++; }
          // |   WHILE '('exp error                { $$ = 0; error_message("Missmatched starting '('.", line_number, NULL, NULL); errors_no++; }
          // |   FOR error                         { $$ = 0; error_message("Missing condition ", line_number, NULL, "in for statement"); errors_no++; }
          // |   FOR '('simp';'exp';'error         { $$ = 0; error_message("Missing loop operation ", line_number, NULL, "in for statement"); errors_no++; }
          // |   FOR '('simp';'exp';'simp error    { $$ = 0; error_message("Missmatched starting '('.", line_number, NULL, NULL); errors_no++; }
          ;


else_blc  :   ELSE  block                       { /*printf("201\n");*/ $$ = add_node(VElseBlock, $2); }
          ;

block     :   stmt                              { /*printf("22\n");*/ $$ = add_node(VStmt, $1); }
          |   '{'stmts'}'                       { /*printf("23\n");*/ $$ = add_node(VStmts, $2); }
          |   error                             { $$ = add_empty_node(VError); error_message("Expected statement.", line_number, NULL, NULL); errors_no++; }
          ;

exp       :   binopExp AND binopExp             { $$ = add_two_nodes(BiAND, $1, $3); }
          |   binopExp OR binopExp              { $$ = add_two_nodes(BiOR, $1, $3); }
          |   binopExp                          { /*printf("28\n");*/ $$ = add_node(VBinopExp, $1); }
          ;

asopExp   :   ID '=' exp  { $$ = add_two_nodes(AssigEQ, create_node_existing_variable($1), $3); }
          |   ID PLE exp  { $$ = add_two_nodes(AssigPLE, create_node_existing_variable($1), $3); }
          |   ID MNE exp  { $$ = add_two_nodes(AssigMNE, create_node_existing_variable($1), $3); }
          |   ID MLE exp  { $$ = add_two_nodes(AssigMLE, create_node_existing_variable($1), $3); }
          |   ID SBE exp  { $$ = add_two_nodes(AssigSBE, create_node_existing_variable($1), $3); }
          |   ID MDE exp  { $$ = add_two_nodes(AssigMDE, create_node_existing_variable($1), $3); }
          // ERRORS
          |   ID error    { $$ = add_empty_node(VError); char* lookahead = malloc(sizeof(char)); lookahead[0] = yychar; error_message("Expected assigment but got ", line_number, lookahead, "."); errors_no++; }

          ;

binopExp  :   binop1 '<' binop1  { $$ = add_two_nodes(BiLS, $1, $3); }
          |   binop1 LE binop1   { $$ = add_two_nodes(BiLE, $1, $3); }
          |   binop1 '>' binop1  { $$ = add_two_nodes(BiGT, $1, $3); }
          |   binop1 GE binop1   { $$ = add_two_nodes(BiGE, $1, $3); }
          |   binop1 EQ binop1   { $$ = add_two_nodes(BiEQ, $1, $3); }
          |   binop1 NE binop1   { $$ = add_two_nodes(BiNE, $1, $3); }
          |   binop1             { /*printf("29\n");*/ $$ = add_node(VBinop1, $1); }
          ;

binop1    :   binop1 '+' binop2   { /*printf("30\n");*/ $$ = add_two_nodes(BiPLUS, $1, $3); }
          |   binop1 '-' binop2   { $$ = add_two_nodes(BiMINUS, $1, $3); }
          |   binop2              { /*printf("31\n");*/ $$ = add_node(VBinop2, $1); }
          ;

binop2    :   binop2 '*' factor        { $$ = add_two_nodes(BiMULT, $1, $3); }
          |   binop2 '/' factor        { $$ = add_two_nodes(BiDIV, $1, $3); }
          |   binop2 '%' factor        { $$ = add_two_nodes(BiMOD, $1, $3); }
          |   factor                   { /*printf("32\n");*/ $$ = add_node(VFactor, $1); }
          ;

factor    :   DEC_CONST                         { /*printf("33\n");*/ $$ = create_node_constant(atoi($1)); }
          |   ID                                { /*printf("34\n");*/ $$ = create_node_existing_variable($1); }
          |   unopExp                           { /*printf("34\n");*/ $$ = add_node(VUnopExp, $1); }
          |   '('exp')'                         { /*printf("37\n");*/ $$ = add_node(VExp, $2); }
          // ERRORS
          |   '('exp error                        { $$ = add_empty_node(VError); error_message("Missmatched starting '('.", line_number, NULL, NULL); errors_no++; }

          ;

unopExp   :   '!' factor  { /*printf("35\n");*/ $$ = add_node(OpNOT, $2); }
          |   '-' factor         { /*printf("36\n");*/ $$ = add_node(OpNEG, $2); }
          ;

%%

void yyerror(char *s)
{
   printf("%d\n", line_number);
   fputs(s,stderr); putc('\n',stderr);
}
