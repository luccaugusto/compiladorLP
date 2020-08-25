/* Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 *
 * Tudo em um só arquivo pois é uma restrição deste trabalho
 *
 * Este programa segue a maioria das recomendações do estilo de código definido em: https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* MACROS */
#define TAM_TBL 254
#define SEPARADOR "=-=-=-=-="

/* DECLARAÇÕES */

typedef enum {
	Identificador = 0,
} Tokens;

/*registro na tabela*/
struct Simbolo {
	Tokens token;
	char *lexema;
	char *tipo;
	int mem; /*local na memória*/
};

/* Celulas da lista encadeada */
struct Celula {
	struct Celula *prox;
	struct Simbolo simbolo;
};


/*DECLARAÇÕES DE FUNÇÕES*/
unsigned int hash(unsigned char *str);
struct Celula *adicionarRegistro(char *lexema, int token);
struct Celula *pesquisarRegistro(char *lexema);
void adicionarReservados(void);
void mostrarTabelaSimbolos(void);
void inicializarTabela(void);
void testeInsercao(void);
void testeColisao(void);
void testeBuscaSimples(void);
void testeBuscaEmColisao(void);
void limparLista(struct Celula *cel);
void limparTabela(void);
void testesTabelaSimbolos(void);
void testeBuscaVazia(void);

/* VARIÁVEIS GLOBAIS */
/* parametros da linha de comando */
char *progFonte;
char *progAsm;
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

struct Celula *pesquisarRegistro(char *procurado)
{
	int encontrado = 0;
	unsigned int pos = hash(procurado);
	struct Celula *retorno = NULL;
	struct Celula *prox = tabelaSimbolos[pos];

	while (!encontrado && prox != NULL) {
		if (prox->simbolo.lexema != NULL && strcmp(prox->simbolo.lexema, procurado) == 0) {
			encontrado = 1;
			retorno = prox;
		} else {
			prox = prox->prox;
		}
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

/* testa a busca por um registro que não está na lista
 */
void testeBuscaVazia(void)
{
	printf("Testando busca vazia...........");
	/* strings que colidem e são diferentes */
	char *lex = "esse lexema não está na tabela";
	struct Celula *encontrado = pesquisarRegistro(lex);
	if (encontrado != NULL)
		printf("ERRO: Busca em colisão falhou.\n");
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
	testeBuscaVazia();
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
