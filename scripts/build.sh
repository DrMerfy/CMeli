cd ../build
# clean
rm *
# build
yacc -dv ../src/yacc.y
flex ../src/lex.l
gcc lex.yy.c y.tab.c ../src/zyywrap.c
