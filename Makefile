
.phony: build
build: clean compiler

compiler:
	mkdir build;
	cd src; cp macros.h ../build/
	cd src;	cp codeGenerator.h ../build/
	cd src; cp semantic.h ../build/
	cd src; cp tree.h ../build/
	cd src; cp out.h ../build/
	cd src; cp main.c ../build/
	cd src; cp zyywrap.c ../build/

	cd build; yacc -vd ../src/yacc.y

	cd build; flex ../src/lex.l

	cd build; gcc main.c -o compiler


.phony: test
test:
	make
	cd scripts; ./test.sh

.phony: run
run:
	cd build; mixasm assem
	cd build; mixvm -r assem

.phony: clean
clean:
	rm -rf build;
