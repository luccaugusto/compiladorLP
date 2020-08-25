#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define TAM_TBL 254
#define SEPARADOR "-=-=-=-=-="

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

void adicionarReservados(void)
{
	adicionarRegistro("const",Identificador);
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

/* printa a tabela de simbolos */
void mostrarTabelaSimbolos(void)
{
	printf(SEPARADOR"TABELA DE SÍMBOLOS"SEPARADOR"\n");
	for (int i=0; i<TAM_TBL; ++i) {
		if (tabelaSimbolos[i] != NULL) {
			printf("| %d\t|-> %s", i, tabelaSimbolos[i]->simbolo.lexema);
			struct Celula *prox = tabelaSimbolos[i]->prox;
			while (prox != NULL){
				printf(" -> %s",prox->simbolo.lexema);
				prox = prox->prox;
			}
			printf("\n");
		}
	}
}

void inicializarTabela(void)
{
    adicionarReservados();
}

/* testa a inserção de um registro na tabela */
void testeInsercao(void)
{
	printf("Testando inserção simples......");
	char *lex = "teste";
	struct Celula *inserido = adicionarRegistro(lex, Identificador);
	if (inserido == NULL)
		printf("ERRO: Inserção falhou\n");
	else
		printf("OK\n");
}

/* testa a inserção de um registro que cause uma colisão na tabela */
void testeColisao(void)
{
	printf("Testando inserção com colisão..");
	char *lex = "teste2";
	struct Celula *inserido = adicionarRegistro(lex, Identificador);
	struct Celula *colisao = adicionarRegistro(lex, Identificador);
	if (inserido->prox != colisao)
		printf("ERRO: Colisão falhou\n");
	else
		printf("OK\n");
}

/* testa a busca por um registro que está no início da lista, ou seja,
 * sem colisões.
 */
void testeBuscaSimples(void)
{
	printf("Testando busca simples.........");
	char *lex = "teste3";
	struct Celula *inserido = adicionarRegistro(lex, Identificador);
	struct Celula *encontrado = pesquisarRegistro(lex);
	if (encontrado != inserido)
		printf("ERRO: Busca simples falhou\n");
	else
		printf("OK\n");
}

/* testa a busca por um registro que está no meio da lista, ou seja,
 * quando houveram colisões.
 */
void testeBuscaEmColisao(void)
{
	printf("Testando busca em colisão......");
	/* strings que colidem e são diferentes */
	char *lex = "not";
	char *lex2 = "%";
	/*guarda somente o registro que colidiu, que é o que buscamos*/
	adicionarRegistro(lex, Identificador);
	struct Celula *inserido = adicionarRegistro(lex2, Identificador);
	struct Celula *encontrado = pesquisarRegistro(lex2);
	if (encontrado != inserido)
		printf("ERRO: Busca em colisão falhou.\n \
					  ponteiro inserido: %p\n \
				      ponteiro encontrado: %p\n", inserido, encontrado);
	else
		printf("OK\n");
}

/* limpa a lista encadeada recursivamente.
 * Utilizado somente para fins de testes
 */
void limparLista(struct Celula *cel)
{
	if (cel != NULL) {
		limparLista(cel->prox);
		free(cel);
	}
}

/* limpa a tabela de símbolos
 * usado para fins de testes
 */
void limparTabela(void)
{
	for (int i=0; i<TAM_TBL; ++i) {
		if (tabelaSimbolos[i] != NULL) {
			limparLista(tabelaSimbolos[i]->prox);
			free(tabelaSimbolos[i]);
			tabelaSimbolos[i] = NULL;
		}
	}
}

void testesTabelaSimbolos(void)
{
	testeInsercao();
	testeColisao();
	testeBuscaSimples();
	testeBuscaEmColisao();
	mostrarTabelaSimbolos();
	limparTabela();
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

	testesTabelaSimbolos();

	inicializarTabela();

	return 0;
}
