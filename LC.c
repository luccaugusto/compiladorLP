/* Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 *
 * Tudo em um só arquivo pois é uma restrição deste trabalho
 *
 * Este programa segue a maioria das recomendações do estilo de código definido em: https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 */


/* TODO
 * esta ignorando o lexema que vem logo antes de EOF, consetar
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* MACROS */
#define TAM_TBL 254
#define SEPARADOR "=-=-=-=-="
#define ERRO_LEXICO -1
#define ACEITACAO 10

/* DECLARAÇÕES */

typedef enum {
	Identificador = 0,
	Constante,
} Tokens;

typedef enum {
	Integer = 0,
	Char,
} Tipo;

/*registro na tabela de símbolos*/
struct Simbolo {
	Tokens token;
	char *lexema;
};

/* Celulas da lista encadeada */
struct Celula {
	struct Celula *prox;
	struct Simbolo simbolo;
};

/* Registro léxico */
struct registroLex {
	Tokens token;
	char *lexema;
	struct Celula *endereco;
	Tipo tipo;
	int tamanho;
};


/*DECLARAÇÕES DE FUNÇÕES*/
char minusculo(char l);
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
int lexan(void);
int ehDigito(char l);
int ehLetra(char l);
void concatenar(char *inicio, char *fim);

/* VARIÁVEIS GLOBAIS */
/* parametros da linha de comando */
FILE *progFonte;
FILE *progAsm;
char letra; /*posicao da proxima letra a ser lida no arquivo*/
int linha = 1; /*linha do arquivo*/
char *erroMsg /*Mensagem de erro a ser exibida*/;
struct Celula *tabelaSimbolos[TAM_TBL];
struct registroLex tokenAtual; 


/* DEFINIÇÃO DE FUNÇÕES */
unsigned int hash(unsigned char *str)
{
    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return (hash % TAM_TBL);
}

int ehDigito(char l)
{
	/* valores de 0-9 em ascii */
	int retorno = 0;
	if (30 <= l && l <= 39)
		retorno = 1;
	return retorno;
}

int ehLetra(char l)
{
	/* valores de 0-9 em ascii */
	int retorno = 0;
	if (97 <= l && l <= 122)
		retorno = 1;
	return retorno;
}

void concatenar(char *inicio, char *fim)
{
   while(*inicio)
      inicio++;
     
   while(*fim)
   {
      *inicio = *fim;
      fim++;
      inicio++;
   }
   *inicio = '\0';
}

struct Celula *adicionarRegistro(char *lexema, int token)
{
	unsigned int pos = hash(lexema);
	struct Celula *cel = (struct Celula *) malloc(sizeof(struct Celula));
	struct Simbolo *simb = (struct Simbolo *) malloc(sizeof(struct Simbolo));
	simb->token = token;
	simb->lexema = lexema;
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
			printf("|\t%d\t|-> %s", i, tabelaSimbolos[i]->simbolo.lexema);
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
	limparTabela();
}

/* Converte maiusculas para minusculas */
char minusculo(char l)
{
	if (l >= 65 && l <=90)
		l += 32; 
	return l;
}

int lexan(void)
{
	int retorno = 1;/* retorna 0 quando chega ao fim do arquivo */
	int estado = 0;
	int erro = 0;
	char *lexEncontrado;
	while (estado != ACEITACAO && (letra = minusculo(fgetc(progFonte))) != -1 && !erro) {
		lexEncontrado = &letra;
		/*printf("c: %s | estado: %d | erro: %d | linha: %d\n",lexEncontrado,estado, erro,linha);*/

        /* \n é contabilizado sempre */
		if (letra == '\n')
			linha++;
		if (estado == 0) {
			if (letra == '/') {
				/* comentário ou divisão */ 
				estado = 1;
			} else if (letra == '_' || letra == '.') {
				/* inicio de identificador */
				estado = 7;
			} else if (letra == '<') {
				/* menor ou menor ou igual ou diferente*/
				estado = 4;
			} else if (letra == '>') {
				/* maior ou maior ou igual */
				estado = 5;
			} else if (letra == '"') {
				/* inicio de string */
				estado = 9;
			} else if (ehDigito(letra)) {
				/* inicio de literal */ 
				estado = 6;
			} else if (letra == ',' || letra == ';' || letra == '+' || letra == '-' || \
					letra == '*' || letra == '(' || letra == ')' || letra == '{' || \
					letra == '}' || letra == '[' || letra == ']' || letra == '%' || letra == '=') {
				tokenAtual.token = Identificador;
				tokenAtual.lexema = &letra;
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO;
			}
		} else if (estado == 1) {
			if (letra == '*') {
				/* de fato comentario */
				estado = 2;
			} else {
				/* simbolo '/' encontrado */
				estado = ACEITACAO;
			}
		} else if (estado == 2) {
			if (letra == '*') {
				/* inicio de fim de comentario */
				estado = 3;
			} 
		} else if (estado == 3) {
			if (letra == '/') {
				/* de fato fim de comentario volta ao inicio para ignorar*/
				estado = 0;
			} else {
				/* simbolo '*' dentro do comentario */
				estado = 2;
			}
		} else if (estado == 4) {
			if (letra == '=' || letra == '>') {
				/* lexemas de comparacao <= ou <> */
				estado = ACEITACAO;

				concatenar(lexEncontrado, &letra);
				tokenAtual.token = Identificador;
				tokenAtual.lexema = lexEncontrado;
				tokenAtual.endereco = pesquisarRegistro(lexEncontrado);
			} else if (letra == ' ' || letra == '\n' || ehDigito(letra) || ehLetra(letra)) {
				/* lexema de comparacao < */
				estado = ACEITACAO;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema */
				fseek(progFonte, -sizeof(char),SEEK_CUR);

				tokenAtual.token = Identificador;
				tokenAtual.lexema = lexEncontrado;
				tokenAtual.endereco = pesquisarRegistro(lexEncontrado);
			}
		} else if (estado == 5) {
				printf("%s %c\n",lexEncontrado,letra);
			if (letra == '=') {
				/* lexema de comparacao >= */
				estado = ACEITACAO;

				concatenar(lexEncontrado, &letra);
				tokenAtual.token = Identificador;
				tokenAtual.lexema = lexEncontrado;
				tokenAtual.endereco = pesquisarRegistro(lexEncontrado);

			} else if (letra == ' ' || letra == '\n' || ehDigito(letra) || ehLetra(letra)) {
				/* lexema de comparacao > */
				estado = ACEITACAO;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema */
				fseek(progFonte, -sizeof(char),SEEK_CUR);

				tokenAtual.token = Identificador;
				tokenAtual.lexema = lexEncontrado;
				tokenAtual.endereco = pesquisarRegistro(lexEncontrado);
			}
		}
	}

	if (erro) {
		printf("%d\n%s '%c'\n",linha,erroMsg,letra);
		retorno = ERRO_LEXICO;
	}
	if (letra == -1)
		retorno = 0;
	return retorno;
}

int main(int argc, char *argv[])
{
	char c;

	while((c = getopt(argc,argv,"f:o:")) != -1) {
		switch(c) {
			case 'f':
				progFonte = fopen(optarg, "r");
				break;
			case 'o':
				progAsm = fopen(optarg,"w");
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
	mostrarTabelaSimbolos();
	while(lexan())
		printf("Lexema encontrado: %s\n",tokenAtual.lexema);

	printf("%d linhas compiladas.\n", linha);

	return 0;
}
