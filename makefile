make:
	gcc -o LC LC.c

clean:
	rm -f LC
	rm -f LCDEBUG

run:
	./LC -f fonte.l -o prog.asm

debug:
	gcc -o LCDEBUG LC.c -g
	gdb ./LCDEBUG 
