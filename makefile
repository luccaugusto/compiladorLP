make:
	gcc -o LC LC.c

clean:
	rm -f LC

run:
	./LC -f fonte.l -o prog.asm
