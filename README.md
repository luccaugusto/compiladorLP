# compiladorLP

Compilador desenvolvido para a disciplina de compiladores 2020-2 na pucminas.
A estrutura de arquivos se configura da seguinte maneira:
+ docs
+ monolitico
+ src
+ exemplos
+ 8086

## Docs
Contém as especificações da linguagem, automato do analisador léxico e a gramática. A linguagem foi baseada no documento especificacoes.pdf. A partir dessa especificação criamos o automato do analisador léxico e a gramática.

## Monolitico
Essa pasta contém uma versão monolítica do programa, sem divisões de arquivos. Não é a versão oficial, é uma versão necessária para submissão no verde, plataforma de testes da PUC.

Definitivamente não está elegante e está um tanto confuso, além de não gerar código.

## Src
Arquivos fonte do programa.
### LC.c e LC.h
Contém a main e variáveis mais globais.
### semac.c
Funções de ações semanticas.
### lexan.c e lexan.h
Funções e macros do analisador léxico.
### codegen.c e codegen.h
Funções e macros para a geração de código asm.
### ansin.c e ansin.h
Funções que implementam a gramática e a análise sintática.
### ts.c e ts.h
Tabela de símbolos.
### pilha.c
Implementação de uma pilha. Utilizada para debug.
### testes.c
Testes da tabela de símbolos e lexan.

## Exemplos
Esta pasta contém arquivos de exemplo de códigos na linguagem L. Existem tanto arquivos válidos quanto inválidos, utilizados para testar o compilador. Na pasta testes existem arquivos fonte e na pasta saída existem as saídas corretas correspondentes a cada arquivo de testes. Testes automatizados são realizados pelo script autoteste.sh

## 8086
Esta pasta contem arquivos do MASM para ser executado pelo dosbox, simulando o ambiente DOS, já que o programa gera código para este ambiente.
