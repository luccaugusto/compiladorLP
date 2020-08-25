make:
	gcc -o LC.out LC.c

clean:
	rm -f LC.out
	rm -f LCDEBUG.out

run:
	./LC -f fonte.l -o prog.asm

debug:
	gcc -o LCDEBUG.out LC.c -g
	gdb ./LCDEBUG.out
