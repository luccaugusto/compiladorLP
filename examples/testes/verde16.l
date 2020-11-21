/* Exemplo de Programa na linguagem L */

Var integer n;
Char nome[40],ch;
Const MAXITER=10;

/* Programa Principal */

  write("Digite seu nome: ");
  readln(nome);
  For ch=1 to MAXITER do {
    writeln("Ola' ",nome);
  }
