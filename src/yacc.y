%{

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "semantic.h"
%}

%union
{
  int yint;
  char ystr[8];
  int nonterm;
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

%%
program   :   '{' decls stmts '}'               { printf("/1\n"); $$ = link_two_nodes($1, $2); }
          ;

decls     :   decls decl                        { printf("/2\n"); $$ = link_two_nodes($1, $2); }
          | /* EMPTY */                         { printf("/3\n"); $$ = create_empty_node(); }
          ;

decl      :   VAR ID vars':'type';'             { printf("/4\n"); $$ = link_three_nodes(create_variable($2), $3, $5); }
          ;

vars      :   ','ID vars                        { printf("/5\n"); $$ = link_two_nodes(create_variable($2), $3); }
          | /*EMPTY*/                           { printf("/6\n"); $$ = create_empty_node(); }
          ;

type      :   INT                               { printf("/7\n"); $$ = create_constant($1); }
          ;

stmts     :   stmts stmt                        { printf("/8\n"); $$ = $1; }
          |   /*EMPTY*/                         { printf("/9\n"); }
          ;

stmt      :   simp';'                           { printf("/10\n"); $$ = $1; }
          |   control                           { printf("/11\n"); $$ = $1; }
          |   ';'                               { printf("/12\n"); $$ = $1; }
          ;

simp      :   ID asop exp                       { printf("/13\n"); $$ = $1; }
          |   PRINT exp                         { printf("/14\n"); $$ = $1; }
          ;

control   :   IF'('exp')' block else_blc        { printf("/15\n"); $$ = $3; }
          |   WHILE'('exp')' block              { printf("/16\n"); $$ = $3; }
          |   FOR'('simp';'exp';'simp')' block
              {
                printf("/17\n");
                $$ = $3;
              }
          |   CONTINUE';'
              {
                printf("/18\n");
                $$ = $1;
              }
          |   BREAK';'
              {
                printf("/19\n");
                $$ = $1;
              }
          ;

else_blc  :   ELSE  block
              {
                printf("/20\n");
                $$ = $2;
              }
          |   /*EMPTY*/
              {
                printf("/21\n");
              }
          ;

block     :   stmt                              { printf("/22\n"); $$ = $1; }
          |   '{'stmts'}'                       { printf("/23\n"); $$ = $2; }
          ;

exp       :   '('exp')'                         { printf("/24\n"); $$ = $2; }
          |   DEC_CONST                         { printf("/25\n"); $$ = $1; }
          |   ID                                { printf("/26\n"); $$ = $1; }
          |   unop exp
              {
                printf("/27\n");
                $$ = $1;
              }
          |   exp binop exp
              {
                printf("/28\n");
                $$ = $1;
              }
          ;

asop      :   '='  { printf("/$/1\n");}
          |   PLE { printf("/$/2\n");}
          |   MNE { printf("/$/3\n");}
          |   MlE { printf("/$/4\n");}
          |   SBE { printf("/$/5\n");}
          |   MDE { printf("/$/6\n");}
          ;

binop     :   '+'  { printf("/$/7\n");}
          |   '-'  { printf("/$/8\n");}
          |   '*'  { printf("/$/9\n");}
          |   '/'  { printf("/$/10\n");}
          |   '%'  { printf("/$/11\n");}
          |   '<'  { printf("/$/12\n");}
          |   LE { printf("/$/13\n");}
          |   '>'  { printf("/$/14\n");}
          |   GE { printf("/$/15\n");}
          |   EQ { printf("/$/16\n");}
          |   NE { printf("/$/17\n");}
          |   AND { printf("/$/18\n");}
          |   OR { printf("/$/19\n");}
          ;

unop      :   '!' { printf("/$/20\n");}
          |   '-' { printf("/$/21\n");}
          ;

%%

void yyerror(char *s)
{
   fputs(s,stderr); putc('\n',stderr);
}

int main(void)
{
   yyparse();
}
