#!/bin/sh

g++ -o LC-mono.out monolitico/LC.cpp

echo Teste 1:
./LC-mono.out < examples/testes/verde1.l  > s.txt 
diff -wB s.txt examples/saidas/saida1.txt && echo PASSOU PUB
./LC-mono.out < examples/testes/verde1p.l  > s.txt 
diff -wB s.txt examples/saidas/saida1p.txt && echo PASSOU PRI

echo Teste 2:
./LC-mono.out < examples/testes/verde2.l  > s.txt 
diff -wB s.txt examples/saidas/saida2.txt && echo PASSOU PUB
./LC-mono.out < examples/testes/verde2p.l  > s.txt 
diff -wB s.txt examples/saidas/saida2p.txt && echo PASSOU PRI

echo Teste 3:
./LC-mono.out < examples/testes/verde3.l  > s.txt 
diff -wB s.txt examples/saidas/saida3.txt && echo PASSOU PUB
./LC-mono.out < examples/testes/verde3p.l  > s.txt 
diff -wB s.txt examples/saidas/saida3p.txt && echo PASSOU PRI

echo Teste 4:
./LC-mono.out < examples/testes/verde4.l  > s.txt 
diff -wB s.txt examples/saidas/saida4.txt && echo PASSOU PUB
./LC-mono.out < examples/testes/verde4p.l  > s.txt 
diff -wB s.txt examples/saidas/saida4p.txt && echo PASSOU PRI

echo Teste 5:
./LC-mono.out < examples/testes/verde5.l  > s.txt 
diff -wB s.txt examples/saidas/saida5.txt && echo PASSOU PUB
./LC-mono.out < examples/testes/verde5p.l  > s.txt 
diff -wB s.txt examples/saidas/saida5p.txt && echo PASSOU PRI

echo Teste 6:
./LC-mono.out < examples/testes/verde6.l  > s.txt 
diff -wB s.txt examples/saidas/saida6.txt && echo PASSOU PUB
./LC-mono.out < examples/testes/verde6p.l  > s.txt 
diff -wB s.txt examples/saidas/saida6p.txt && echo PASSOU PRI

echo Teste 7:
./LC-mono.out < examples/testes/verde7.l  > s.txt 
diff -wB s.txt examples/saidas/saida7.txt && echo PASSOU PUB
./LC-mono.out < examples/testes/verde7p.l  > s.txt 
diff -wB s.txt examples/saidas/saida7p.txt && echo PASSOU PRI

echo Teste 8:
./LC-mono.out < examples/testes/verde8.l  > s.txt 
diff -wB s.txt examples/saidas/saida8.txt && echo PASSOU PUB
./LC-mono.out < examples/testes/verde8p.l  > s.txt 
diff -wB s.txt examples/saidas/saida8p.txt && echo PASSOU PRI

echo Teste 9:
./LC-mono.out < examples/testes/verde9.l  > s.txt 
diff -wB s.txt examples/saidas/saida9.txt && echo PASSOU PUB
./LC-mono.out < examples/testes/verde9p.l  > s.txt 
diff -wB s.txt examples/saidas/saida9p.txt && echo PASSOU PRI

echo Teste 10:
./LC-mono.out < examples/testes/verde10.l > s.txt 
diff -wB s.txt examples/saidas/saida10.txt && echo PASSOU PUB
./LC-mono.out < examples/testes/verde10p.l > s.txt 
diff -wB s.txt examples/saidas/saida10p.txt && echo PASSOU PRI

rm s.txt
