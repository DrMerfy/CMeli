
.phony: build
build: clean compiler

compiler:
	mkdir build;
	cd src; cp tree.h ../build/
	cd src; cp errors.h ../build/
	cd src; cp main.c ../build/
	cd src; cp zyywrap.c ../build/

	cd build; yacc -vd ../src/yacc.y

	cd build; flex ../src/lex.l

	cd build; gcc main.c -o compiler


.phony: test
test:
	cd scripts; ./test.sh

.phony: clean
clean:
	rm -rf build;
