make:
	gcc -o LC.out src/LC.c
	g++ -o LC-mono.out monolitico/LC.cpp

clean:
	rm -f LC.out
	rm -f LCDEBUG.out
	rm -f prog.asm

run:
	./LC.out -f examples/fonte.l -o prog.asm

run2:
	./LC.out -f exemplo1.l -o prog.asm

runm:
	./LC-mono.out < examples/fonte.l

debug:
	gcc -o LCDEBUG.out src/LC.c -g
	gdb ./LCDEBUG.out

teste:
	make
	./LC.out -f examples/testes/fonte.l -o prog.asm
