/* Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 *
 * Este programa segue a maioria das recomendações do estilo de código definido 
 * em: https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 *
 * Esta é a versão monolítica do programa, existe para submissão
 * do programa no sistema de testes Verde da PUC.
 * Múltiplos arquivos não são aceitos lá.
 *
 * Esta versão NÃO inclui as funções de testes nem de pilha
 */


/* TODO:
 * Fazer pilha de chamadas para debugar
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


/* DECLARAÇÕES */
/* Definicoes de tipos e estruturas utilizadas */

typedef enum {
	Identificador = 1,
	Const,
	Var,
	Integer,
	Char,
	For,
	If,
	Else,
	And,
	Or,
	Not,
	Igual,
	To,
	A_Parenteses,
	F_Parenteses,
	Menor,
	Maior,
	Diferente,
	MaiorIgual,
	MenorIgual,
	Virgula,
	Mais,
	Menos,
	Vezes,
	Barra,
	PtVirgula,
	A_Chaves,
	F_Chaves,
	Then,
	Readln,
	Step,
	Write,
	Writeln,
	Porcento,
	A_Colchete,
	F_Colchete,
	Do,
	Literal
} Tokens;

typedef enum {
	TP_Integer = 0,
	TP_Char,
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

/* MACROS */
#define TAM_TBL 254
#define SEPARADOR "=-=-=-=-=-=-=-="
#define ERRO_LEXICO -1
#define ERRO_SINTATICO -2
#define ERRO_SINTATICO_EOF -3
#define ACEITACAO_LEX 11
#define ACEITACAO_SIN 12
#define N_ACEITACAO_SIN -4 
#define SUCESSO 0

#define DEBUG_LEX 0
#define DEBUG_SIN 0


/*DECLARAÇÕES DE FUNÇÕES*/

/* utils */
int ehLetra(char l);
int ehDigito(char l);
int ehBranco(char l);
char minusculo(char l);
unsigned int hash(unsigned char *str);
char *concatenar(char *inicio, char *fim);

/* testes */
void testeLexan(void);
void testeColisao(void);
void testeInsercao(void);
void testeBuscaVazia(void);
void testeBuscaSimples(void);
void testeBuscaEmColisao(void);
void testesTabelaSimbolos(void);

/* Tabela de símbolos */
void limparTabela(void);
void inicializarTabela(void);
void adicionarReservados(void);
void mostrarTabelaSimbolos(void);
void limparLista(struct Celula *cel);
struct Celula *pesquisarRegistro(char *lexema);
struct Celula *adicionarRegistro(char *lexema, int token);

/* Analisador Sintatico */
void nulo(void);
void teste(void);
void ansin(void);
void teste1(void);
void leitura(void);
void escrita(void);
void variavel(void);
void listaIds(void);
void expressao(void);
void escritaLn(void);
void constante(void);
void comandos2(void);
void repeticao(void);
void expressao1(void);
void expressao2(void);
void expressao3(void);
void repeticao1(void);
void declaracao(void);
void atribuicao(void);
void blocoComandos(void);
void iniciarAnSin(void);
void fimDeArquivo(void);
void erroSintatico(int tipo);
int casaToken(Tokens encontrado);


/* Analisador léxico */
int lexan(void);

/* Pilha */
void initPilha(void);
void printPilha(void);
void push(char *metodo);
struct elemento *pop(void);

/* Fluxo de execução geral */
void abortar(void);
void sucesso(void);

/* VARIÁVEIS GLOBAIS */

/* parametros da linha de comando */
FILE *progFonte;
FILE *progAsm;

int lex = 1;
int erro = 0;
int linha = 0; /*linha do arquivo*/
int tamPilha = 0;
int estado_sin = 0; /* estado de aceitacao ou nao do analisador sintatico */

char letra; /*posicao da proxima letra a ser lida no arquivo*/
char *erroMsg /*Mensagem de erro a ser exibida*/;

struct elemento *pilha = NULL;
struct registroLex tokenAtual; 
struct Celula *tabelaSimbolos[TAM_TBL];


/* DEFINIÇÃO DE FUNÇÕES */

/* Definicao das funcoes utilitárias do projeto */

/* ************************** *
              HASH
 * ************************** */
unsigned int hash(unsigned char *str)
{
    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return (hash % TAM_TBL);
}



/* ************************** *
           REGEX LIKE 
 * ************************** */
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
	/* valores de a-z em ascii */
	int retorno = 0;
	if (97 <= l && l <= 122)
		retorno = 1;
	return retorno;
}

int ehBranco(char l)
{
	/* valores de tab, line feed, <CR>, espaço em ascii */
	int retorno = 0;
	if (l == 9 || l == 10 || l == 13 || l == 32)
		retorno = 1;
	return retorno;
}

int identificaTipo(char *lex)
{
	int retorno = Identificador;

	if (strcmp(lex,"const") == 0)
		retorno = Const;
	else if (strcmp(lex,"var") == 0)
		retorno = Var;
	else if (strcmp(lex,"integer") == 0)
		retorno = Integer;
	else if (strcmp(lex,"char") == 0)
		retorno = Char;
	else if (strcmp(lex,"for") == 0)
		retorno = For;
	else if (strcmp(lex,"if") == 0)
		retorno = If;
	else if (strcmp(lex,"else") == 0)
		retorno = Else;
	else if (strcmp(lex,"and") == 0)
		retorno = And;
	else if (strcmp(lex,"or") == 0)
		retorno = Or;
	else if (strcmp(lex,"not") == 0)
		retorno = Not;
	else if (strcmp(lex,"to") == 0)
		retorno = To;
	else if (strcmp(lex,"then") == 0)
		retorno = Then;
	else if (strcmp(lex,"readln") == 0)
		retorno = Readln;
	else if (strcmp(lex,"step") == 0)
		retorno = Step;
	else if (strcmp(lex,"write") == 0)
		retorno = Write;
	else if (strcmp(lex,"writeln") == 0)
		retorno = Writeln;
	else if (strcmp(lex,"do") == 0)
		retorno = Do;

	return retorno;
}

/* ************************** *
   MANIPULACAO DE STRING 
 * ************************** */

char *concatenar(char *inicio, char *fim)
{
	char *retorno;
	int tamInicio = strlen(inicio);
	int tamFim = strlen(fim);

	retorno = malloc(tamInicio+tamFim);

	for (int i=0; i<tamInicio; ++i)
		retorno[i] = inicio[i];
	for (int i=0; i<tamFim; ++i)
		retorno[tamInicio+i] = fim[i];

	/*printf("concatenado %s\n",retorno);*/
	return retorno;
}

/* Converte maiusculas para minusculas */
char minusculo(char l)
{
	if (l >= 65 && l <=90)
		l += 32; 
	return l;
}

/* Implementacao da tabela de simbolos */

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

/* Implementacao do automato do analisador lexico
 * retorna 1 caso de sucesso,
 *         0 caso EOF encontrado,
 *         letra caso erro lexico
 */


int lexan(void)
{
	int retorno = 1;/* retorna 0 quando chega ao fim do arquivo */
	int estado = 0;
	int posAtual = ftell(progFonte);

	/* zera o token atual */
	tokenAtual.lexema = "";
	tokenAtual.token = 0;
	tokenAtual.endereco = NULL;
	tokenAtual.tipo = 0;
	tokenAtual.tamanho = 0;

	while (estado != ACEITACAO_LEX && !erro && (letra = minusculo(fgetc(progFonte))) != -1) { 
        /* \n é contabilizado sempre */
		if (letra == '\n' || letra == '\r') {
			linha++;
		} 

		if (estado == 0) {
			if (letra == '/') {
				/* comentário ou divisão */ 
				estado = 1;
			} else if (ehBranco(letra)) {
				continue;
			} else if (letra == '_' || letra == '.') {
				/* inicio de identificador */
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 7;
			} else if (letra == '<') {
				/* menor ou menor ou igual ou diferente*/
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 4;
			} else if (letra == '>') {
				/* maior ou maior ou igual */
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 5;
			} else if (letra == '"') {
				/* inicio de string */
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 9;
			} else if (ehDigito(letra)) {
				/* inicio de literal */ 
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 6;
			} else if (letra == ',') {
				tokenAtual.token = Virgula;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
			} else if (letra == ';') {
				tokenAtual.token = PtVirgula;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '+') {
				tokenAtual.token = Mais;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '-') {
				tokenAtual.token = Menos;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '*') {
				tokenAtual.token = Vezes;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '(') {
				tokenAtual.token = A_Parenteses;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == ')') {
				tokenAtual.token = F_Parenteses;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '{') {
				tokenAtual.token = A_Chaves;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '}') {
				tokenAtual.token = F_Chaves;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '[') {
				tokenAtual.token = A_Colchete;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == ']') {
				tokenAtual.token = F_Colchete;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '%') {
				tokenAtual.token = Porcento;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '=') {
				tokenAtual.token = Igual;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
			} else if (letra == '_' || letra == '.') {
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 7;
			} else if (ehLetra(letra)) {
                /*inicio palavra*/
                tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 8;
			} else if (letra >=  48 && letra <= 57) {
                /*inicio inteiro*/
                tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 10;
			} else if (letra == -1) {
				estado = ACEITACAO_LEX;
			} else {
				/* caractere inválido */
				erro = ERRO_LEXICO;
				erroMsg = "Caractere inválido";
			}
            
		} else if (estado == 1) {
			if (letra == '*') {
				/* de fato comentario */
				estado = 2;
			} else {
				/* simbolo '/' encontrado */
				estado = ACEITACAO_LEX;
			}
		} else if (estado == 2) {
			if (letra == '*') {
				/* inicio de fim de comentario */
				estado = 3;
			} else if (letra == -1) {
				/*EOF encontrado*/
				erro = ERRO_LEXICO;
				erroMsg = "Fim de arquivo não esperado";
			}
		} else if (estado == 3) {
			if (letra == '/') {
				/* de fato fim de comentario volta ao inicio para ignorar*/
				estado = 0;
			} else if (letra == -1) {
				/*EOF encontrado*/
				erro = ERRO_LEXICO;
				erroMsg = "Fim de arquivo não esperado";
			} else {
				/* simbolo '*' dentro do comentario */
				estado = 2;
			}
		} else if (estado == 4) {
			if (letra == '=' || letra == '>') {
				/* lexemas de comparacao <= ou <> */
				estado = ACEITACAO_LEX;

				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(tokenAtual.lexema);

				if (letra == '=')
					tokenAtual.token = MenorIgual;
				else
					tokenAtual.token = Diferente;

			} else if (ehBranco(letra) || ehDigito(letra) || ehLetra(letra)) {
				/* lexema de comparacao < */
				estado = ACEITACAO_LEX;

				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
			 	 */
				if (! ehBranco(letra))
					fseek(progFonte, posAtual, SEEK_SET);

				tokenAtual.token = Menor;
				tokenAtual.endereco = pesquisarRegistro(tokenAtual.lexema);
			} else if (letra == -1) {
				/*EOF encontrado, assume que encontrou <*/
				estado = ACEITACAO_LEX;
			}
		} else if (estado == 5) {
			if (letra == '=') {
				/* lexema de comparacao >= */
				estado = ACEITACAO_LEX;

				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.token = Identificador;
				tokenAtual.endereco = pesquisarRegistro(tokenAtual.lexema);

			} else if (ehBranco(letra) || ehDigito(letra) || ehLetra(letra)) {
				/* lexema de comparacao > */
				estado = ACEITACAO_LEX;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
				 */
				if (! ehBranco(letra))
					fseek(progFonte, posAtual,SEEK_SET);

				tokenAtual.token = Maior;
				tokenAtual.endereco = pesquisarRegistro(tokenAtual.lexema);
			} else if (letra == -1) {
				/*EOF encontrado, assume que encontrou >*/
				estado = ACEITACAO_LEX;
			}

		} else if (estado == 7) {
            /*lexema identificador _ . 
            concatena até achar uma letra ou numero */
            if (letra == '_' || letra == '.') {
                tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
            } else if (ehLetra(letra) || ehDigito(letra)) {
                tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 8;
            }
        } else if (estado == 8) {
            /*lexema de identificador
            concatena ate finalizar o identificador ou palavra reservada */
            if (ehLetra(letra) ||  letra == '_' || letra == '.' ) {
                tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
            } else {
				estado = ACEITACAO_LEX;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema */
				if (! ehBranco(letra))
					fseek(progFonte, posAtual, SEEK_SET);

                tokenAtual.token = identificaTipo(tokenAtual.lexema);
				tokenAtual.endereco = pesquisarRegistro(tokenAtual.lexema);
                if (tokenAtual.endereco == NULL) {
                   //adicionar novo token (identificador)
                    tokenAtual.endereco = adicionarRegistro(tokenAtual.lexema,tokenAtual.token);
                }
			} 
        } else if (estado == 9) {
            /*lexema de String
            concatena até encontrar o fechamento das aspas */
            tokenAtual.token = Literal;
			tokenAtual.tipo = TP_Char;
            tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
            if (letra != '"') {
                estado = 9;
            } else {
                estado = ACEITACAO_LEX;
            }
        } else if (estado == 10) {
            /*lexema de Inteiro
            concatena até finalizar o numero */
            if (letra >=  48 && letra <= 57) {
                tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
            } else {
				estado = ACEITACAO_LEX;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
			 	 */
				if (! ehBranco(letra))
					fseek(progFonte, posAtual, SEEK_SET);
				tokenAtual.token = Literal;
				tokenAtual.tipo = TP_Integer;

			} 
        }

		posAtual = ftell(progFonte);
	}

	if (erro) {
		retorno = letra;
	}
	if (letra == -1)
		retorno = 0;
	lex = retorno;
	if (DEBUG_LEX) printf("%s\n",tokenAtual.lexema);
	return retorno;
}

/* Analisador sintático
 * retorna 1 caso sucesso,
 *         0 caso erro,
 *
 * TODAS as leituras de lexemas são feitas fora
 * da chamada ao próximo procedimento de símbolo
 * 
 * O procedimento de símbolo não faz leitura do
 * simbolo que espera. Esse símbolo já deve ter
 * sido lido antes.
 */


/* confere se o último token lido é esperado
 * Caso não seja o token esperado, aborta a
 * execução chamando erroSintatico()
 *
 * essa função tem seu uso limitado pois
 * alguns estados aceitam mais de um Token
 * como por exemplo: Var char|integer.
 * Nesses casos casaToken nao pode ser
 * utilizada.
 * Quando só existe uma opção de Token
 * casaToken DEVE ser utilizada.
 */
int casaToken(Tokens esperado)
{
	int retorno = 1;
	if (esperado == 0 && estado_sin != ACEITACAO_SIN) {

		/* chega EOF (0) mas não em estado de aceitação */
		erroSintatico(ERRO_SINTATICO_EOF);

	} else if (esperado != tokenAtual.token){

		erroSintatico(ERRO_SINTATICO);

	}

	return retorno;
}

/* Trata um Erro Sintático
 * e aborta o programa
 */
void erroSintatico(int tipo)
{
	/* mostra a pilha de chamadas */
	/* printPilha(); */

	if (tipo == ERRO_SINTATICO) {
		erro = ERRO_SINTATICO;
		erroMsg = "token não esperado";
	} else {
		erro = ERRO_SINTATICO_EOF;
		erroMsg = "fim de arquivo não esperado.";
	}

	/* Aborta a compilação */
	abortar();
}

/* Consime o primeiro token e chama 
 * o simbolo inicial
 */
void iniciarAnSin(void)
{
	/* consome o primeiro token */
	lexan();
	/* inicia pelo primeiro simbolo da gramatica */
	declaracao();
}

/* Declaracao de variáveis ou constantes 
 * Var variavel();
 * Const constante();
 */
void declaracao(void)
{
	if (DEBUG_SIN) printf("Declaracao\n");
	/*push("Declaracao");*/
	/* var ou const */
	if (tokenAtual.token == Var) {
		lexan();
		variavel();
		declaracao();
	} else if (tokenAtual.token == Const) {
		lexan();
		constante();
		declaracao();
	} else {
		lexan();
		blocoComandos();
		fimDeArquivo();
	}
}


/* Bloco de comandos
 * For, If, ID= , ;, readln();, write();, writeln();
 */
void blocoComandos()
{
	if (DEBUG_SIN) printf("blocoComandos\n");
	/*push("blocoComandos");*/
	switch(tokenAtual.token)
	{
		case Identificador:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			atribuicao();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case For:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			repeticao();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case If:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			teste();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case PtVirgula:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			nulo();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case Readln:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			leitura();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case Write:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			escrita();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case Writeln:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			escritaLn();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case F_Chaves:
			/* encontrou o fim do bloco de comandos atual,
			 * retorna e deixa o metodo que chamou tratar o }
			 */
			estado_sin = ACEITACAO_SIN;
			return;

		default:
			return;
	}
}

/* EOF */
void fimDeArquivo(void)
{
	if (DEBUG_SIN) printf("fimDeArquivo\n");
	/*push("fimdearquivo");*/

	/* se lex nao for 0 ainda n leu o EOF */
	if (lex)
		erroSintatico(ERRO_SINTATICO);

	/* leu fim de arquivo mas nao em estado de aceitacao */
	else if (estado_sin != ACEITACAO_SIN)
		erroSintatico(ERRO_SINTATICO_EOF);

	sucesso();
}

/***********************************************
 *
 *  Procedimentos de Declaração
 *
 ***********************************************/

/* Const id = literal; */
void constante(void)
{
	if (DEBUG_SIN) printf("constante\n");
	/*push("constante");*/
	estado_sin = N_ACEITACAO_SIN;
	if (casaToken(Identificador)) {
		lexan();
		if (casaToken(Igual)) {
			lexan();
			if (casaToken(Literal)) {
				lexan();
				if (casaToken(PtVirgula)) {
					declaracao();
				}
			}
		}
	}
}

/* var char|integer listaIds();
 */
void variavel(void)
{
	if (DEBUG_SIN) printf("variavel\n");
	/*push("variavel");*/
	estado_sin = N_ACEITACAO_SIN;
	if (tokenAtual.token == Char || tokenAtual.token == Integer) {
		lexan();
		listaIds();
	} else {
		erroSintatico(ERRO_SINTATICO);
	}
	estado_sin = ACEITACAO_SIN;
}


/* id;
 * id,id,...;
 * id=literal,...|;
 * id[int],...|;
 */
void listaIds(void)
{
	if (DEBUG_SIN) printf("listaIds\n");
	/*push("listaIds");*/
	if (casaToken(Identificador)) {
		lexan();
		if (tokenAtual.token == Virgula){
			/* Lendo id,id */
			lexan();
			listaIds();
		} else if (tokenAtual.token == PtVirgula) {
			/* lendo id; */
			lexan();
			if (tokenAtual.token == Integer || tokenAtual.token == Char)
				/* Lista de declaracoes tipo Var integer c; char d; */
				variavel();
			/* else fim do comando */

		} else if (tokenAtual.token == Igual) {
			/* lendo id=literal */
			lexan();
			if (casaToken(Literal)) {
				lexan();
				if (tokenAtual.token == Virgula) {
					/* outro id */
					lexan();
					listaIds();
				} else if (casaToken(PtVirgula)) {
					/* terminou de ler o comando */
					lexan();
					if (tokenAtual.token == Integer || tokenAtual.token == Char)
						/* Lista de declaracoes tipo Var integer c; char d; */
						variavel();
					/* else fim do comando */
				}
			}
		} else if (casaToken(A_Colchete)) {
			/* lendo id[int] */
			lexan();
			if (casaToken(Literal)) {
				lexan();
				if (casaToken(F_Colchete)) {
					lexan();
					if (tokenAtual.token == Virgula) {
						/* outro id */
						lexan();
						listaIds();
					} else if (casaToken(PtVirgula)) {
						/* terminou de ler o comando */
						lexan();
						if (tokenAtual.token == Var || tokenAtual.token == Const) 
							/* var|const integer id[2]; var|const char ... */
							declaracao();
						else /* integer id[2]; integer listaIds()... */
							estado_sin = ACEITACAO_SIN;
							variavel();
					}
				}
			}
		}
	}
}

/***********************************************
 *
 *  Procedimentos de Bloco de comandos
 *
 ***********************************************/


/* Atribuicao
 * ID=literal;
 */
void atribuicao(void)
{
	if (DEBUG_SIN) printf("atribuicao\n");
	/*push("atribuicao");*/
	/* lendo array: id[i] */
	if (tokenAtual.token == A_Colchete) {
		lexan();
		if (tokenAtual.token == Identificador || tokenAtual.token == Literal) {
			lexan();
			if (casaToken(F_Colchete)) {
				lexan();
			}
		} else {
			erroSintatico(ERRO_SINTATICO);
		}
	}
	if (casaToken(Igual)) {
		lexan();
		if (tokenAtual.token == Identificador) {
			lexan();
			/* lendo array: id[i] */
			if (tokenAtual.token == A_Colchete) {
				lexan();
				if (tokenAtual.token == Identificador || tokenAtual.token == Literal) {
					lexan();
					if (casaToken(F_Colchete)) {
						lexan();
					}

				} else {
					erroSintatico(ERRO_SINTATICO);
				}
			}
			if (casaToken(PtVirgula)) {
				lexan();
			}

		} else if (casaToken(Literal)) {
			lexan();
			if (casaToken(PtVirgula)) {
				lexan();
			}
		}
	}
} 

/* Repeticao
 * ID = literal to literal repeticao1();
 */
void repeticao(void)
{
	if (DEBUG_SIN) printf("repeticao\n");
	/*push("repeticao");*/
	if (casaToken(Identificador)) {
		lexan();
		if (casaToken(Igual)) {
			lexan();
			if (casaToken(Literal)) {
				lexan();
				if (casaToken(To)) {
					lexan();
					if (casaToken(Literal)) {
						lexan();
						repeticao1();
					}
				}
			}
		}
	}
}

/* 
 * R2 na gramatica
 *
 * step literal do comandos2();
 * ou
 * do comandos2();
 */
void repeticao1(void)
{
	if (DEBUG_SIN) printf("repeticao1\n");
	/*push("repeticao1");*/
	if (tokenAtual.token == Step) {
		lexan();
		if (casaToken(Literal)) {
			lexan();
			if (casaToken(Do)) {
				lexan();
				comandos2();
			}
		}
	} else if (casaToken(Do)) {
		lexan();
		comandos2();
	}
}

/* R1 na gramatica
 * { blocoComandos() }
 * ou
 * comando unico
 */
void comandos2(void)
{
	if (DEBUG_SIN) printf("comandos2\n");
	/*push("comandos2");*/
	switch(tokenAtual.token)
	{
		case Identificador:
			lexan();
			atribuicao();
			break;

		case For:
			lexan();
			repeticao();
			break;

		case If:
			lexan();
			teste();
			break;

		case PtVirgula:
			lexan();
			nulo();
			break;

		case Readln:
			lexan();
			leitura();
			break;

		case Write:
			lexan();
			escrita();
			break;

		case Writeln:
			lexan();
			escritaLn();
			break;

		case A_Chaves:
			lexan();
			blocoComandos();
			/* o } ja foi lido por alguem na chamada antiga chamou */
			if (casaToken(F_Chaves)) {
				lexan();
			}
			break;

		case F_Chaves:
			/* encontrou o fim do bloco de comandos atual,
			 * retorna e deixa o metodo que chamou tratar o }
			 */
			return;

		default:
			erroSintatico(ERRO_SINTATICO);
	}
}

/* Teste
 * expressao() then comandos2() teste1()
 */
void teste(void)
{
	if (DEBUG_SIN) printf("teste\n");
	/*push("teste");*/
	expressao();
	/* then foi lido antes de retornar de expressao() */
	if (casaToken(Then)) {
		lexan();
		comandos2();
		if (tokenAtual.token == F_Chaves) {
			lexan(); 
		}
		teste1();
	}
}

/* else comandos2()
 * ou fim do if
 * blocoComandos()
 */
void teste1(void)
{
	if (DEBUG_SIN) printf("teste1\n");
	/*push("teste1");*/
	lexan();
	if (tokenAtual.token == Else) {
		lexan();
		comandos2();
	}
}

/* Comando de leitura
 * readln(id)
 */
void leitura(void)
{
	if (DEBUG_SIN) printf("leitura\n");
	/*push("leitura");*/
	if (casaToken(A_Parenteses)) {
		lexan();
		if (casaToken(Identificador)) {
			lexan();
			if (tokenAtual.token == A_Colchete) {
				/* lendo array: id[i] */
				lexan();
				if (tokenAtual.token == Identificador || tokenAtual.token == Literal) {
					lexan();
					if (casaToken(F_Colchete)) {
						lexan();
					}
				} else {
					erroSintatico(ERRO_SINTATICO);
				}
			}
			/* ja leu ( id|id[i] ) e pode fechar o comando */
			if (casaToken(F_Parenteses)) {
				lexan();
				if (casaToken(PtVirgula)) {
					lexan();
				}
			}
		}
	}
}

/* Comando nulo
 * ;
 */
void nulo(void)
{
	if (DEBUG_SIN) printf("nulo\n");
	/*push("nulo");*/
	if (casaToken(PtVirgula)) {
		lexan();
	}
}

/* Comando de escrita
 * write(id|const)
 */
void escrita(void)
{
	if (DEBUG_SIN) printf("escrita\n");
	/*push("escrita");*/
	if (casaToken(A_Parenteses)) {
		lexan();
		expressao2();
		if (casaToken(F_Parenteses)) {
			lexan();
			if (casaToken(PtVirgula)) {
				lexan();
			}
		}
	}
}

/* Funciona como um wrapper para o escrita.
 * a sintaxe dos dois é a mesma porém o escritaLn
 * deve colocar a quebra de linha no final.
 * Isso será tratado posteriormente, ao implementar
 * a geração de código
 * TODO colocar quebra de linha
 */
void escritaLn(void)
{
	if (DEBUG_SIN) printf("escritaLn\n");
	/*push("escritaLn");*/
	escrita();
}

void expressao(void)
{
	if (DEBUG_SIN) printf("expressao\n");
	/*push("expressao");*/
	if (tokenAtual.token == A_Parenteses) {
		lexan();
		expressao();
		lexan();
		if (casaToken(F_Parenteses)) {
			lexan();
		}
	} else if (tokenAtual.token == Identificador) {
		 /* id */
		lexan();
		/* lendo array: id[i] */
		if (tokenAtual.token == A_Colchete) {
			lexan();
			if (tokenAtual.token == Identificador || tokenAtual.token == Literal) {
				lexan();
				if (casaToken(F_Colchete)) {
					lexan();
					expressao1();
				}
			} else {
				erroSintatico(ERRO_SINTATICO);
			}
		} else {
			expressao1();
		}
	} else if (casaToken(Literal)) {
		lexan();
		expressao1();
	}
}

void expressao1(void)
{
	if (DEBUG_SIN) printf("expressao1\n");
	/* op id */
	if (tokenAtual.token == MaiorIgual ||
		tokenAtual.token == MenorIgual ||
		tokenAtual.token == Maior      ||
		tokenAtual.token == Menor      ||
		tokenAtual.token == Diferente  ||
		tokenAtual.token == Or         ||
		tokenAtual.token == Mais       ||
		tokenAtual.token == Menos      ||
		tokenAtual.token == Porcento   ||
		tokenAtual.token == Barra      ||
		tokenAtual.token == And        ||
		tokenAtual.token == Vezes      ||
		tokenAtual.token == Not        ||
		tokenAtual.token == Igual)
	{
		lexan();
		expressao(); 
	}
	/* leu lambda */
	/* id sozinho, retorna (lambda) */
}

/* lista de expressoes */
void expressao2(void)
{
	expressao();
	expressao3();
}

void expressao3(void)
{
	if (tokenAtual.token == Virgula) {
		lexan();
		expressao2();
	}
	/* else lambda */
}

/*
 * pára o programa e reporta o erro
 */
void abortar(void)
{
	switch(erro) {
		case ERRO_LEXICO: 
			printf("%d\n %s [%c] \n", linha, erroMsg, lex);
			break;
		case ERRO_SINTATICO:
			printf("%d\n %s [%s] \n", linha, erroMsg, tokenAtual.lexema);
			break;
		case ERRO_SINTATICO_EOF:
			printf("%d\n %s \n", linha, erroMsg);
			break;
	}
	exit(erro);
}

/* pára o programa e reporta linhas compiladas */
void sucesso(void)
{
	printf("%d linhas compiladas.\n", linha);
	exit(SUCESSO);
}

void adicionarReservados(void)
{
	adicionarRegistro("const",Const);
	adicionarRegistro("var",Var);
	adicionarRegistro("integer",Integer);
	adicionarRegistro("char",Char);
	adicionarRegistro("for",For);
	adicionarRegistro("if",If);
	adicionarRegistro("else",Else);
	adicionarRegistro("and",And);
	adicionarRegistro("or",Or);
	adicionarRegistro("not",Not);
	adicionarRegistro("=",Igual);
	adicionarRegistro("to",To);
	adicionarRegistro("(",A_Parenteses);
	adicionarRegistro(")",F_Parenteses);
	adicionarRegistro("<",Menor);
	adicionarRegistro(">",Maior);
	adicionarRegistro("<>",Diferente);
	adicionarRegistro(">=",MaiorIgual);
	adicionarRegistro("<=",MenorIgual);
	adicionarRegistro(",",Virgula);
	adicionarRegistro("+",Mais);
	adicionarRegistro("-",Menos);
	adicionarRegistro("*",Vezes);
	adicionarRegistro("/",Barra);
	adicionarRegistro(";",PtVirgula);
	adicionarRegistro("{",A_Chaves);
	adicionarRegistro("}",F_Chaves);
	adicionarRegistro("then",Then);
	adicionarRegistro("readln",Readln);
	adicionarRegistro("step",Step);
	adicionarRegistro("write",Write);
	adicionarRegistro("writeln",Writeln);
	adicionarRegistro("%",Porcento);
	adicionarRegistro("[",A_Colchete);
	adicionarRegistro("]",F_Colchete);
	adicionarRegistro("do",Do);
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

	/*testesTabelaSimbolos();*/

	inicializarTabela();

    /*mostrarTabelaSimbolos();*/
	/* testeLexan();*/
    
	iniciarAnSin();


	return 0;

}