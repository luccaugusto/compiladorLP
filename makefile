make:
	gcc -o LC.out src/LC.c
	g++ -o LC-mono.out monolitico/LC.cpp

clean:
	rm -f LC.out
	rm -f LC-mono.out
	rm -f LCDEBUG.out
	rm -f prog.asm
	rm -f 8086/prog.asm

run:
	./LC.out -f examples/fonte.l -o prog.asm
	cp prog.asm 8086/prog.asm

runm:
	./LC-mono.out < examples/fonte.l

no_debug:
	sed -i "s/#define DEBUG_LEX/\/\/define DEBUG_LEX/g" src/lexan.h
	sed -i "s/#define DEBUG_SIN/\/\/define DEBUG_SIN/g" src/ansin.h
	sed -i "s/#define DEBUG_TS/\/\/define DEBUG_TS/g"   src/ts.c
	sed -i "s/#define DEBUG_GEN/\/\/define DEBUG_GEN/g" src/codegen.h

debug:
	sed -i "s/\/\/define DEBUG_LEX/#define DEBUG_LEX/g" src/lexan.h
	sed -i "s/\/\/define DEBUG_SIN/#define DEBUG_SIN/g" src/ansin.h
	sed -i "s/\/\/define DEBUG_GEN/#define DEBUG_GEN/g" src/codegen.h

debug_full:
	sed -i "s/\/\/define DEBUG_LEX/#define DEBUG_LEX/g" src/lexan.h
	sed -i "s/\/\/define DEBUG_SIN/#define DEBUG_SIN/g" src/ansin.h
	sed -i "s/\/\/define DEBUG_GEN/#define DEBUG_GEN/g" src/codegen.h
	sed -i "s/\/\/define DEBUG_TS/#define DEBUG_TS/g"   src/ts.c

debug_gdb:
	gcc -o LCDEBUG.out src/LC.c -g
	gdb ./LCDEBUG.out

teste:
	make
	./LC.out -f examples/testes/fonte.l -o prog.asm

dosbox:
	cd 8086/ && dosbox .
