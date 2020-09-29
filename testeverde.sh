#!/bin/sh

g++ -o LC-mono.out monolitico/LC.cpp

echo Teste 1:
(./LC-mono.out < examples/testes/verde1.l ) > s.txt && diff -wB s.txt examples/testes/saida1.txt
echo Teste 2:
(./LC-mono.out < examples/testes/verde2.l ) > s.txt && diff -wB s.txt examples/testes/saida2.txt
echo Teste 3:
(./LC-mono.out < examples/testes/verde3.l ) > s.txt && diff -wB s.txt examples/testes/saida3.txt
echo Teste 4:
(./LC-mono.out < examples/testes/verde4.l ) > s.txt && diff -wB s.txt examples/testes/saida4.txt
echo Teste 5:
(./LC-mono.out < examples/testes/verde5.l ) > s.txt && diff -wB s.txt examples/testes/saida5.txt
echo Teste 6:
(./LC-mono.out < examples/testes/verde6.l ) > s.txt && diff -wB s.txt examples/testes/saida6.txt
echo Teste 7:
(./LC-mono.out < examples/testes/verde7.l ) > s.txt && diff -wB s.txt examples/testes/saida7.txt
echo Teste 8:
(./LC-mono.out < examples/testes/verde8.l ) > s.txt && diff -wB s.txt examples/testes/saida8.txt
echo Teste 9:
(./LC-mono.out < examples/testes/verde9.l ) > s.txt && diff -wB s.txt examples/testes/saida9.txt
echo Teste 10:
(./LC-mono.out < examples/testes/verde10.l) > s.txt && diff -wB s.txt examples/testes/saida10.txt
