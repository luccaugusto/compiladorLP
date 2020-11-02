/* Exemplo de Programa na linguagem L */

Var integer n;
Char nome[40];
Char b;
Const MAXITER=10;

/* Programa Principal */

/* atribuicao invalida */
  MAXITER = 11;

  if MAXITER = 10 then
	  write("MAIOR QUE 10");

  write("Digite seu nome: ");
  readln(nome);
  For n=1 to maxiter do {
    writeln("Ola' ",nome);
  }
