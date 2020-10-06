/* Exemplo de Programa na linguagem L */

var integer n[3], max;

/* Programa Principal */

	for i=0 to 2 do {
		write("Digite um numero: ");
		readln(n[i]);
	}
	if n[0] > n[1] then {
		if n[0] > n[2] then max=n[0]; else max=n[2];
	} else if n[1] > n[2] then max=n[1]; else max=n[2];
	writeln("O maior numero e' ",max);
