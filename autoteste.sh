#!/bin/sh

VERSION="src"
C=0
TOTAL=0
[ "$1" == 'm' ] && VERSION="mono"

make no_debug &&
make && (

	for teste in $(ls examples/testes/)
	do

		TOTAL=$((TOTAL+1))
	
		T=${teste%%.*}
		echo "$T:"
		if [ "$VERSION" == "src" ]
		then
			./LC.out -f "examples/testes/$teste" -o prog.asm > s.txt
			diff -wB s.txt "examples/saidas/saida-"$T".txt" && echo PASSOU && C=$((C+1))
		else
			./LC-mono.out < "examples/testes/$teste" > s.txt
			diff -wB s.txt "examples/saidas/saida-"$T".txt" && echo PASSOU && C=$((C+1))
		fi
	done


	echo "=============================="
	echo "$C/$TOTAL testes bem sucedidos"
	echo "=============================="

	rm s.txt
) && 
make clean
