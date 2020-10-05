# compiladorLP

Compilador desenvolvido para a disciplina de compiladores 2020-2 na pucminas.
A estrutura de arquivos se configura da seguinte maneira:
+ docs
+ monolitico
+ src
+ exemplos

## Docs
Contém as especificações da linguagem, automato do analisador léxico e a gramática. A linguagem foi baseada no documento especificacoes.pdf. A partir dessa especificação criamos o automato do analisador léxico e a gramática.

## Monolitico
Essa pasta contém uma versão monolítica do programa, sem divisões de arquivos. Não é a versão oficial, é uma versão necessária para submissão no verde, plataforma de testes da PUC.

Definitivamente não está elegante e está um tanto confuso, sem divisão de arquivos o programa se torna difícil de manter.

## Src
Esta pasta contém todos os arquivos de headers e funções do compilador organizados de forma mais compreensiva. É a versão "oficial", o monolítico é a junção dos arquivos desta pasta.

## Exemplos
Esta pasta contém arquivos de exemplo de códigos na linguagem L. Estes arquivos foram utilizados para testar o compilador.
