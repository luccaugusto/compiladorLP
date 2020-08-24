#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define TAM_TBL 254

 /*parametros da linha de comando*/
char *progFonte;
char *progAsm;

enum Tokens
{
	Identificador,
};

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

struct Celula *tabelaSimbolos[TAM_TBL];

struct Celula *proc;

unsigned int hash(unsigned char *str)
{
    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return (hash % TAM_TBL);
}

struct Celula *adicionarRegistro(char *lexema, int token)
{
	unsigned int pos = hash(lexema);
	struct Celula *cel = (struct Celula *) malloc(sizeof(struct Celula));
	struct Simbolo *simb = (struct Simbolo *) malloc(sizeof(struct Simbolo));
	simb->token = token;
	simb->lexema = lexema;
	simb->tipo = "int";
	simb->mem = 0;
	cel->prox = NULL;
	cel->simbolo = *simb;
	
	if (tabelaSimbolos[pos] == NULL) {
		tabelaSimbolos[pos] = cel;
	}else{
		tabelaSimbolos[pos]->prox = cel;
	}
	return cel;
}

struct Celula *pesquisarRegistro(char *lexema)
{
	int encontrado = 0;
	int i = 0;
	char *lexAtual;
	struct Celula *retorno = NULL;
	while (!encontrado && i<TAM_TBL) {
		if (tabelaSimbolos[i] != NULL && tabelaSimbolos[i]->simbolo.lexema != NULL) {
			if (strcmp(tabelaSimbolos[i]->simbolo.lexema, lexema) == 0) {
				encontrado = 1;
				retorno = tabelaSimbolos[i];
			} else {
				struct Celula *prox = tabelaSimbolos[i]->prox;
				while (prox != NULL && !encontrado){
					if (strcmp(prox->simbolo.lexema, lexema) == 0) {
						encontrado = 1;
						retorno = prox;
					}
					prox = prox->prox;
				}
			}
		}
		++i;
	}
	return retorno;
}

void adicionarReservados()
{
	proc = adicionarRegistro("const",Identificador);
	adicionarRegistro("var",Identificador);
	adicionarRegistro("integer",Identificador);
	adicionarRegistro("char",Identificador);
	adicionarRegistro("for",Identificador);
	adicionarRegistro("if",Identificador);
	adicionarRegistro("else",Identificador);
	adicionarRegistro("and",Identificador);
	adicionarRegistro("or",Identificador);
	adicionarRegistro("not",Identificador);
	adicionarRegistro("=",Identificador);
	adicionarRegistro("to",Identificador);
	adicionarRegistro("(",Identificador);
	adicionarRegistro(")",Identificador);
	adicionarRegistro("<",Identificador);
	adicionarRegistro(">",Identificador);
	adicionarRegistro("<>",Identificador);
	adicionarRegistro(">=",Identificador);
	adicionarRegistro("<=",Identificador);
	adicionarRegistro(",",Identificador);
	adicionarRegistro("+",Identificador);
	adicionarRegistro("-",Identificador);
	adicionarRegistro("*",Identificador);
	adicionarRegistro("/",Identificador);
	adicionarRegistro(";",Identificador);
	adicionarRegistro("{",Identificador);
	adicionarRegistro("}",Identificador);
	adicionarRegistro("then",Identificador);
	adicionarRegistro("readln",Identificador);
	adicionarRegistro("step",Identificador);
	adicionarRegistro("write",Identificador);
	adicionarRegistro("writeln",Identificador);
	adicionarRegistro("%",Identificador);
	adicionarRegistro("[",Identificador);
	adicionarRegistro("]",Identificador);
	adicionarRegistro("do",Identificador);
}

void inicializarTabela()
{
	/*
	for(int i=0; i<TAM_TBL; ++i) {
		struct Celula *cel = (struct Celula *) malloc(sizeof(struct Celula));
		struct Simbolo *simb = (struct Simbolo *) malloc(sizeof(struct Simbolo));
		cel->simbolo = *simb;
		tabelaSimbolos[i] = cel;
	}
	*/
    adicionarReservados();
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

	inicializarTabela();
	/*
	for (int i=0; i<TAM_TBL; ++i) {
		printf("%d - %s", i, tabelaSimbolos[i]->simbolo.lexema);
		struct Celula *prox = tabelaSimbolos[i]->prox;
		while (prox != NULL){
			printf(" -> %s",prox->simbolo.lexema);
			prox = prox->prox;
		}
		printf("\n");
	}
	*/
	printf("pesquisando por const\n");
	printf("%p, %p\n",pesquisarRegistro("const"), proc);


	return 0;
}
