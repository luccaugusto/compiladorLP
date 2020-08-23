make:
	gcc -o LC LC.c

clean:
	rm -f LC
	rm -f a.out

run:
	./LC -f fonte.l -o prog.asm
