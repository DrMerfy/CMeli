
.phony: build
build: clean compiler

compiler:
	mkdir build;
	cd src; cp semantic.h ../build/ 

	cd build; yacc -vd ../src/yacc.y

	cd build; flex ../src/lex.l

	cd build; gcc lex.yy.c y.tab.c ../src/zyywrap.c -o compiler


.phony: test
test:
	cd scripts; ./test.sh

.phony: clean
clean:
	rm -rf build;


