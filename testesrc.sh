#!/bin/sh
make

echo Teste 1:
./LC.out -f examples/testes/verde1.l -o prog.asm > s.txt
diff -wB s.txt examples/saidas/saida1.txt && echo PASSOU

echo Teste 2:
./LC.out -f examples/testes/verde2.l -o prog.asm > s.txt
diff -wB s.txt examples/saidas/saida2.txt && echo PASSOU

echo Teste 3:
./LC.out -f examples/testes/verde3.l -o prog.asm > s.txt
diff -wB s.txt examples/saidas/saida3.txt && echo PASSOU

echo Teste 4:
./LC.out -f examples/testes/verde4.l -o prog.asm > s.txt
diff -wB s.txt examples/saidas/saida4.txt && echo PASSOU

echo Teste 5:
./LC.out -f examples/testes/verde5.l -o prog.asm > s.txt
diff -wB s.txt examples/saidas/saida5.txt && echo PASSOU

echo Teste 6:
./LC.out -f examples/testes/verde6.l -o prog.asm > s.txt
diff -wB s.txt examples/saidas/saida6.txt && echo PASSOU

echo Teste 7:
./LC.out -f examples/testes/verde7.l -o prog.asm > s.txt
diff -wB s.txt examples/saidas/saida7.txt && echo PASSOU

echo Teste 8:
./LC.out -f examples/testes/verde8.l -o prog.asm > s.txt
diff -wB s.txt examples/saidas/saida8.txt && echo PASSOU

echo Teste 9:
./LC.out -f examples/testes/verde9.l -o prog.asm > s.txt
diff -wB s.txt examples/saidas/saida9.txt && echo PASSOU

echo Teste 10:
./LC.out -f examples/testes/verde10.l -o prog.asm > s.txt
diff -wB s.txt examples/saidas/saida10.txt && echo PASSOU

rm s.txt
make clean
