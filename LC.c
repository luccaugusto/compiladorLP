#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TAM_TBL 254

enum Tokens
{
	Identificador,
};

 /*parametros da linha de comando*/
char *progFonte;
char *progAsm;

/*registro do token na tabela*/
struct Simbolo
{
	enum Tokens token;
	char *lexema;
	char *tipo;
	int mem; /*local na memória*/
};

struct Celula
{
	struct Celula *prox;
	struct Simbolo simbolo;
};

struct Celula tabelaSimbolos[TAM_TBL];

enum Tokens selecionarToken(char *str)
{
	return Identificador;
}

unsigned int hash(unsigned char *str)
{
    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return (hash % TAM_TBL);
}

void adicionarRegistro(char *novoRegistro)
{

	int token = selecionarToken(novoRegistro);
	
	unsigned int pos = hash(novoRegistro);
	struct Celula cel;
	struct Simbolo simb;
	simb.token = token;
	simb.lexema = novoRegistro;
	simb.tipo = "int";
	simb.mem = 0;
	cel.prox = NULL;
	cel.simbolo = simb;

	if (tabelaSimbolos[pos].simbolo.lexema == NULL) {
		tabelaSimbolos[pos] = cel;
	}else{
		tabelaSimbolos[pos].prox = &cel;
	}
}

void pesquisarRegistro(char *p)
{
}

void adicionarReservados()
{
	adicionarRegistro("const");
	adicionarRegistro("const");
	adicionarRegistro("var");
	adicionarRegistro("integer");
	adicionarRegistro("char");
	adicionarRegistro("for");
	adicionarRegistro("if");
	adicionarRegistro("else");
	adicionarRegistro("and");
	adicionarRegistro("or");
	adicionarRegistro("not");
	adicionarRegistro("=");
	adicionarRegistro("to");
	adicionarRegistro("(");
	adicionarRegistro(")");
	adicionarRegistro("<");
	adicionarRegistro(">");
	adicionarRegistro("<>");
	adicionarRegistro(">=");
	adicionarRegistro("<=");
	adicionarRegistro(",");
	adicionarRegistro("+");
	adicionarRegistro("-");
	adicionarRegistro("*");
	adicionarRegistro("/");
	adicionarRegistro(";");
	adicionarRegistro("{");
	adicionarRegistro("}");
	adicionarRegistro("then");
	adicionarRegistro("readln");
	adicionarRegistro("step");
	adicionarRegistro("write");
	adicionarRegistro("writeln");
	adicionarRegistro("%");
	adicionarRegistro("[");
	adicionarRegistro("]");
	adicionarRegistro("do");
}


int main(int argc, char *argv[])
{
	char c;

	while((c = getopt(argc,argv,"f:o:")) != -1) {
		switch(c) {
			case 'f':
				progFonte = optarg;
				break;
			case 'o':
				progAsm = optarg;
				break;
			case '?':
				if (optopt == 'f')
					printf("Programa fonte não especificado.");
				else if (optopt == 'o')
					printf("Arquivo de saída não especificado.");
				else
					printf("Parametros não especificados");
			default:
					printf("\nUso: LC -f <programa fonte> -o <arquivo de saída>\n");
					return 1;
		}
	}
	if (progFonte == NULL || progAsm == NULL) {
		printf("Parametros não especificados\nUso: LC -f <programa fonte> -o <arquivo de saída>\n");
		return 1;
	}

    adicionarReservados();
	for (int i=0; i<TAM_TBL; ++i) {
		printf("%d - %s", i, tabelaSimbolos[i].simbolo.lexema);
		struct Celula *prox = tabelaSimbolos[i].prox;
		while (prox != NULL){
			printf(" -> %s",prox->simbolo.lexema);
			prox = prox->prox;
		}
		printf("\n");
	}

	return 0;
}
