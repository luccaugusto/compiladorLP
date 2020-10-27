make:
	gcc -o LC.out src/LC.c
	g++ -o LC-mono.out monolitico/LC.cpp

clean:
	rm -f LC.out
	rm -f LC-mono.out
	rm -f LCDEBUG.out
	rm -f prog.asm

run:
	./LC.out -f examples/fonte.l -o prog.asm

runm:
	./LC-mono.out < examples/fonte.l

no_debug:
	sed -i "s/DEBUG_LEX 1/DEBUG_LEX 0/g" src/lexan.h
	sed -i "s/DEBUG_SIN 1/DEBUG_SIN 0/g" src/ansin.h
	sed -i "s/DEBUG_TS 1/DEBUG_TS 0/g"   src/ts.h

debug:
	sed -i "s/DEBUG_LEX 0/DEBUG_LEX 1/g" src/lexan.h
	sed -i "s/DEBUG_SIN 0/DEBUG_SIN 1/g" src/ansin.h

debug_full:
	sed -i "s/DEBUG_LEX 0/DEBUG_LEX 1/g" src/lexan.h
	sed -i "s/DEBUG_SIN 0/DEBUG_SIN 1/g" src/ansin.h
	sed -i "s/DEBUG_TS 0/DEBUG_TS 1/g"   src/ts.h

debug_gdb:
	gcc -o LCDEBUG.out src/LC.c -g
	gdb ./LCDEBUG.out

teste:
	make
	./LC.out -f examples/testes/fonte.l -o prog.asm
