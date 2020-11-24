#!/bin/sh

TEST_DIR="examples"
VERSION="src"
C=0
TOTAL=0
[ "$1" == 'm' ] && VERSION="mono"

make no_debug &&
make && (

	for teste in $(ls $TEST_DIR/testes)
	do

		TOTAL=$((TOTAL+1))
	
		T=${teste%%.*}
		echo "$T:"
		if [ "$VERSION" == "src" ]
		then
			./LC.out -f "$TEST_DIR/testes/$teste" -o prog.asm > s.txt
			diff -wB s.txt "$TEST_DIR/saidas/saida-"$T".txt" && echo PASSOU && C=$((C+1))
		else
			./LC-mono.out < "$TEST_DIR/testes/$teste" > s.txt
			diff -wB s.txt "$TEST_DIR/saidas/saida-"$T".txt" && echo PASSOU && C=$((C+1))
		fi
	done


	echo "=============================="
	echo "$C/$TOTAL testes bem sucedidos"
	echo "=============================="

	rm s.txt
) && 
make clean
