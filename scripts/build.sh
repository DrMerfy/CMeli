cd ../build
# clean
rm *
# build
yacc -dv ../src/syd
flex ../src/lex
gcc lex.yy.c y.tab.c ../src/zyywrap.c
