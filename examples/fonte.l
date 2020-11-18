/* Exemplo de Programa na linguagem L */

Var integer n,m;
Char x;
integer b=11;
char nome[256];
Const MAXITER=-10;

n=2+2*2;

if n<>6 then {
	write("nao eh 6");
	n = 6;
} else {
	write("eh 6");
	n = 6;
}

/* Programa Principal		*/

	
  write("Digite seu nome: ");
  readln(nome);
  For n=1 to MAXITER do {
    writeln("Ola' ",nome);
  }
