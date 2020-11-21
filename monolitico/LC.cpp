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
 * .cpp por questoes de compatibilidade com o verde
 *
 * Esta versão NÃO inclui as funções de testes nem de pilha
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//define DEBUG_LEX
//define DEBUG_SIN
//define DEBUG_GEN

/* MACROS */
#define DEVOLVIDO_NULL -7
#define ER_LEX -11
#define ER_LEX_EOF -12
#define ER_LEX_INVD -13
#define ER_LEX_N_ID -14
#define ACEITACAO_LEX 13
#define C_INVALIDO letra != '/' &&\
		!ehBranco(letra)&&\
		!ehDigito(letra)&&\
		!ehLetra(letra) &&\
		letra != '_'    &&\
		letra != '.'    &&\
		letra != '<'    &&\
		letra != '>'    &&\
		letra != '"'    &&\
		letra != ','    &&\
		letra != ';'    &&\
		letra != '+'    &&\
		letra != '-'    &&\
		letra != '('    &&\
		letra != ')'    &&\
		letra != '{'    &&\
		letra != '}'    &&\
		letra != '['    &&\
		letra != ']'    &&\
		letra != '%'    &&\
		letra != '='    &&\
		letra != ':'    &&\
		letra != '\''   &&\
		letra != '.'    \

#define TAM_INT 2
#define TAM_CHA 1
#define TAM_TBL 256
#define A_SEG_PUB CONCAT_BUF((char *)"dseg SEGMENT PUBLIC\n");
#define F_SEG_PUB CONCAT_BUF((char *)"dseg ENDS\n");
//define CONCAT_BUF(...) sprintf(aux, __VA_ARGS__); buf_concatenar();
#define CONCAT_BUF(...)
#define MAX_BUF_SIZE 2048
#define MAX_AUX_SIZE 256

#define ER_SIN -20
#define ER_SIN_EOF -21
#define ER_SIN_NDEC -22
#define ER_SIN_JADEC -23
#define ER_SIN_TAMVET -24
#define ER_SIN_C_INC -25
#define ER_SIN_T_INC -26

#define N_ACEITACAO_SIN -31
#define ACEITACAO_SIN 32

#define NOVO_FATOR(s) Fator *s = (Fator *)malloc(sizeof(Fator)); \
										s->endereco = novoTemp(2); \
										s->tipo = tipoLex; \
										s->op = Nulo

#define SEPARADOR "=-=-=-=-=-=-=-="
#define SUCESSO 0
#ifdef DEBUG_SIN
	#define DEBUGSIN(s) printf((char *)"SIN: %s\n",s);
#else
	#define DEBUGSIN(s)
#endif
#ifdef DEBUG_LEX
	#define DEBUGLEX(...) printf(__VA_ARGS__);
#else
	#define DEBUGLEX(...)
#endif
#ifdef DEBUG_GEN
	#define DEBUGGEN(s) printf((char *)"GEN: %s\n",s);
#else
	#define DEBUGGEN(s)
#endif

typedef int rot;

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
	Literal,
	Nulo
} Tokens;

typedef enum {
	TP_Integer = 1,
	TP_Char,
	TP_Logico,
	TP_Nulo
} Tipo;

typedef enum {
	CL_Const = 1,
	CL_Var,
	CL_Nulo
} Classe;

/*registro na tabela de símbolos*/
struct Simbolo {
	int tamanho;    /* tamanho do array    */
	char *lexema;   /* lexema              */
	int memoria;    /* endereco na memoria */
	Tipo tipo;      /* int char ou logico  */
	Tokens token;   /* token do simbolo    */
	Classe classe;  /* const ou var        */
};

/* Celulas da lista encadeada */
struct Celula {
	Celula *prox;
	Simbolo simbolo;
};

/* Registro léxico 
 * O registro lexico eh mantido para cada comando
 * token,lexema, pos e tamanho se alteram a cada chamada do lexan()
 * os demais atributos se alteram conforme o comando 
 * vai sendo identificado pelo analisador sintatico
 */
struct registroLex {
	int pos;                     /* posicao a ser acessada no array, 0 se qualquer outra coisa */
	int tamanho;                 /* tamanho do array, arrays de tamanho 1 sao variaveis        */
	char *lexema;                /* lexema atual do registro do comando atual                  */
	Tipo tipo;                   /* Tipo atual da expressao ,identificador ou constante atual  */
	Tokens token;                /* token atual do registro do comando atual                   */
	Classe classe;               /* Classe atual da expressao ,identificador ou constante      */
	Celula *endereco;     /* endereco na tabela de simbolos do identificador atual      */
};

/* Fator
 * um fator contém informações
 * sobre o que a expressão gera
 */
struct Fator {
	int endereco;        /* endereco do valor gerado pela expressao */
	int tamanho;         /* tamanho do valor gerado pela expressao  */
	Tokens op;           /* operacao realizada pela expressao       */
	Tipo tipo;           /* tipo do valor gerado pela expressao     */
};


/* VARIÁVEIS GLOBAIS */
int erro;
int lex = 1;
int lido;
char* lexAux;

/* parametros da linha de comando */
FILE *progFonte;
char *fonteNome;
FILE *progAsm;
char *asmNome;

int linha; /*linha do arquivo*/
int estado_sin; /* estado de aceitacao ou nao do analisador sintatico */

char letra; /*letra lida*/
char *erroMsg; /*Mensagem de erro a ser exibida*/
char *lexemaLido; /* lexema lido sem transformar em minusculo */
char devolvido = DEVOLVIDO_NULL; /*caractere devolvido pelo lexan */

/* registro lexico */
int posLex = -1;
int tamanhoLex = 1;
char *lexemaLex = (char *)"";
Tipo tipoLex = TP_Nulo;
Tokens tokenLex = Nulo;
Classe classeLex = CL_Nulo;
struct Celula *enderecoLex = (struct Celula *) malloc(sizeof(struct Celula));


Celula *tabelaSimbolos[TAM_TBL];


char *buffer;       /* buffer de criacao do codigo asm     */
char *aux;          /* buffer auxiliar para criacao do asm */
int CD = 0x4000;    /* contator de dados em hexadecimal    */
int TP = 0x0;       /* contador de temporários             */
rot RT = 1;         /* contador de rotulos                 */

/*DECLARAÇÕES DE FUNÇÕES*/
/* manipulacao do registro lexico */
void atrPos(int);

/* Analisador Sintatico */
void nulo(void);
void teste(void);
void ansin(void);
void teste1(rot, rot);
void leitura(void);
void escrita(int);
void variavel(void);
void listaIds(Tipo);
void constante(void);
void comandos2(void);
void repeticao(void);
struct Fator *expressao(void);
struct Fator *fator(void);
struct Fator *termo(void);
struct Fator *expressaoS(void);
void expressao2(int);
void repeticao1(struct Fator *, rot, rot);
void declaracao(void);
void atribuicao(void);
void blocoComandos(void);
void iniciarAnSin(void);
void fimDeArquivo(void);
void erroSintatico(int);
int casaToken(Tokens);

/* Geração de código */
void flush(void);
void buf_concatenar(void);
int novoTemp(int);
int novoRot(void);
void zeraTemp(void);
void genAtribuicao(struct Fator *, struct Fator *);
void genExp(struct Fator *, char*);
void genDeclaracao(Tipo, Classe, int, char*, int);
void genRepeticao(struct Fator *, struct Fator *, rot, rot);
void genFimRepeticao(struct Fator *, rot, rot, char *);
void genTeste(struct Fator *, rot, rot);
void genElseTeste(rot, rot);
void genFimTeste(rot);
void initDeclaracao(void);
void iniciarCodegen(void);
void fimComandos(void);
void fimDecInitCom(void);
void fatorGeraId(struct Fator *, char *);
void fatorGeraLiteral(struct Fator *, char *);
void fatorGeraExp(struct Fator *,struct Fator *);
void fatorGeraNot(struct Fator *, struct Fator *);
void fatorGeraArray(struct Fator *,struct Fator *, char *);
void fatorGeraMenos(struct Fator *, struct Fator *);

/* Fluxo de execução geral */
void abortar(void);
void sucesso(void);

void atualizaPai(struct Fator *, struct Fator *);
void guardaOp(struct Fator *);
void genOpTermos(struct Fator *, struct Fator *);


/* Tabela de símbolos */
Tipo buscaTipo(char *);
void limparTabela(void);
void inicializarTabela(void);
void adicionarReservados(void);
void mostrarTabelaSimbolos(void);
void limparLista(struct Celula *);
struct Celula *pesquisarRegistro(char *);
struct Celula *adicionarRegistro(char *, Tokens);

/* Analisador léxico */
void lexan(void);

/* utils */
char lexGetChar(void);
int str2int(char*);
int ehLetra(char l);
int ehDigito(char l);
int ehBranco(char l);
char minusculo(char l);
char *encurtar(char *);
char *removeAspas(char *);
char *removeBranco(char *);
char *removeComentario(char *);
Tokens identificaToken(char *);
char *concatenar(char *, char *);
int hash(char *, int);

/* acoes semanticas */
void defTipo(Tipo);
Tipo toLogico(Tipo);
void verificaTam(int);
void defClasse(Classe);
void verificaTipo(Tipo, Tipo);
void verificaClasse(char *, Tipo);
void verificaDeclaracao(char *);

/* testes */
void testeLexan(void);
void testeColisao(void);
void testeInsercao(void);
void testeBuscaVazia(void);
void testeBuscaSimples(void);
void testeBuscaEmColisao(void);
void testesTabelaSimbolos(void);

/* ************************** *
              HASH
 * ************************** */
int hash(char *str, int mod)
{
    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return (hash % mod);
}



/* ************************** *
           REGEX LIKE 
 * ************************** */
int ehDigito(char l)
{
	/* valores de 0-9 em ascii */
	int retorno = 0;
	if (48 <= l && l <= 57)
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

Tokens identificaToken(char *lex)
{
	Tokens retorno = Identificador;

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

/* remove os caracteres na primeira e ultima posicao, ou seja, aspas */
char *removeAspas(char *str)
{
	char *retorno;
	int tamstr = strlen(str);
	retorno = (char *) malloc(tamstr-2);

	for (int i=0; i<tamstr-2; ++i) {
		retorno[i] = str[i+1];
	}

	return retorno;
}

/* remove o ultimo caractere de str */
char *encurtar(char *str)
{
	char *retorno;
	int tamstr = strlen(str);
	/* nao encurta strings de tamanho 1 */
	if (tamstr > 1) {

		/* remove a ultima posicao e acrescenta a posicao do \0 */
		retorno = (char *) malloc(tamstr);
	
		for (int i=0; i<tamstr-1; ++i)
			retorno[i] = str[i];
	
		retorno[tamstr]='\0';
	} else {
		retorno = str;
	}

	return retorno;
}

char *concatenar(char *inicio, char *fim)
{

	char *retorno;
	int tamInicio = strlen(inicio);
	int tamFim = strlen(fim);
	retorno = (char *)malloc(sizeof(char)*(tamInicio+tamFim));

	strcpy(retorno,inicio);
	strcat(retorno,fim);

	return retorno;
}

/* Converte maiusculas para minusculas */
char minusculo(char l)
{
	if (l >= 65 && l <=90)
		l += 32; 
	return l;
}

/* transforma uma string de numeros em um inteiro */
int str2int(char *str)
{
	int l = strlen(str);
	int val = 1; /* valor da casa (unidade, dezena, centena) */
	int ret = 0;

	/* comeca do fim da string (posição menos significativa), e multiplica 
	 * cada posicao(valor ascii - 48 para encontrar o valor do numero de fato)
	 * por val, que é multiplicado por 10 a cada iteração,
	 * representando as posições mais significativas
	 */
	for (int i=l-1; i>=0; --i) {
		ret += (str[i]-48) * val;
		val*=10;
	}
	return ret;
}

/* remove brancos que nao estejam entre strings */
char *removeBranco(char *str)
{
	char *ret;
	int t = strlen(str);
	int c = 0;
	int concat = 0;

	ret = (char *)malloc(sizeof(char) * t);	

	for (int i=0; i<t; ++i) {
		/* se eh inicio de string nao concatena ate o fim de string */
		if (str[i] == '"')
			concat = !concat; /* toogle */

		/* concatena o caractere se nao for branco ou se for para concatenar brancos */
		if ( !ehBranco(str[i]) || concat)
			ret[c++] = str[i];

	}
	/* finaliza string */
	ret[c] = '\0';

	return (char *)realloc(ret,strlen(ret));
}

/* remove comentarios do lexemaLido */
char *removeComentario(char *str)
{
	char *ret;
	int t = strlen(str);
	int c = 0;
	int concat = 1;
	int remover = 0;

	ret = (char *)malloc(sizeof(char));	
	/* se existe um comentario, remove, senao, retorna a string original */
	for (int i=1; i<t; ++i) {
		if (str[i-1] == '/' && str[i] == '*') {
			remover = 1;
			break;
		}
	}

	if (remover) {
		/* marca quais posicoes nao sao comentario */
		for (int i=1; i<t; ++i) {
			/* se eh inicio de comentario nao concatena ate o fim de comentario*/
			if (str[i-1] == '/' && str[i] == '*') {
				concat = 0;
			} else if (str[i-1] == '*' && str[i] == '/') {
				concat = 1;
				continue;
			} else if ( i<t && str[i] == '/' && str[i+1] == '*') {
				concat = 0;
			}
	
			if (concat)
				ret[c++] = str[i];

		}

	} else {
		ret = str;
	}

	return ret;
}

/* ************************** *
   LEITURA DE ARQUIVO 
 * ************************** */
char lexGetChar(void)
{
	int c = getchar();
	/* EOF */
	if (c == -1) lex = 0;
	char *l = (char *) &c;
	lexemaLido = concatenar(lexemaLido,l);
	return (char) c;
}

/* Implementacao do automato do analisador lexico
 * grava lex como 1 caso de sucesso,
 *         0 caso EOF encontrado,
 *         letra caso erro lexico
 */
void lexan(void)
{
	int estado = 0;

	/* zera o lexemaLido */
	lexemaLido = (char *)"";

	/* zera o token e o lexema */
	lexemaLex = (char *)"";
	tokenLex = Nulo;

	/* gasta o caractere devolvido se existir */
	if (devolvido != DEVOLVIDO_NULL) {
		letra = devolvido;
		devolvido = DEVOLVIDO_NULL;
	} else {
		letra = minusculo(lexGetChar());
	}

	int k=0;
	char *l = (char *) &letra;

	/* por algum motivo o k precisa existir para nao entrar em loop */
	while (estado != ACEITACAO_LEX && !erro && (letra > 0) && k++ < 80) { 
		l = (char *) &letra;

        /* \n é contabilizado sempre */
		if (letra == '\n') {
			linha++;
		} 

		if (estado == 0) {
			if (ehBranco(letra)) {
				goto fimloop;
			} else if (letra == '/') {
				/* comentário ou divisão */ 
				lexemaLex = concatenar(lexemaLex, l);;
				estado = 1;
			} else if (letra == '_' || letra == '.') {
				/* inicio de identificador */
				lexemaLex = concatenar(lexemaLex, l);;
				estado = 7;
			} else if (letra == '<') {
				/* menor ou menor ou igual ou diferente*/
				lexemaLex = concatenar(lexemaLex, l);;
				estado = 4;
			} else if (letra == '>') {
				/* maior ou maior ou igual */
				lexemaLex = concatenar(lexemaLex, l);;
				estado = 5;
			} else if (letra == '"') {
				/* inicio de string */
				lexemaLex = concatenar(lexemaLex, l);;
				tamanhoLex = 1;
				estado = 9;
			} else if (letra == '0') {
				/* possivel hexadecimal */
				lexemaLex = concatenar(lexemaLex, l);;
				estado = 10;
			} else if (ehDigito(letra)) {
				/* inicio de literal */ 
				lexemaLex = concatenar(lexemaLex, l);;
				estado = 6;
			} else if (letra == ',') {
				tokenLex = Virgula;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
			} else if (letra == ';') {
				tokenLex = PtVirgula;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
				
			} else if (letra == '+') {
				tokenLex = Mais;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
				
			} else if (letra == '-') {
				tokenLex = Menos;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
				
			} else if (letra == '*') {
				tokenLex = Vezes;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
				
			} else if (letra == '(') {
				tokenLex = A_Parenteses;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
				
			} else if (letra == ')') {
				tokenLex = F_Parenteses;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
				
			} else if (letra == '{') {
				tokenLex = A_Chaves;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
				
			} else if (letra == '}') {
				tokenLex = F_Chaves;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
				
			} else if (letra == '[') {
				tokenLex = A_Colchete;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
				
			} else if (letra == ']') {
				tokenLex = F_Colchete;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
				
			} else if (letra == '%') {
				tokenLex = Porcento;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
				
			} else if (letra == '=') {
				tokenLex = Igual;
				lexemaLex = concatenar(lexemaLex, l);;
				estado = ACEITACAO_LEX;
			} else if (letra == '_' || letra == '.') {
				lexemaLex = concatenar(lexemaLex, l);;
				estado = 7;
			} else if (ehLetra(letra)) {
                /*inicio palavra*/
                lexemaLex = concatenar(lexemaLex, l);;
				estado = 8;
			} else if (!letra) {
				estado = ACEITACAO_LEX;
			} else {
				/* lexema nao identificado */
				erro = ER_LEX_N_ID;
				erroMsg = (char *)"lexema nao identificado";
				lexemaLido = encurtar(lexemaLido);
				abortar();
			}
            
		} else if (estado == 1) {
			if (letra == '*') {
				/* de fato comentario */
				estado = 2;
			} else {
				/* simbolo '/' encontrado */
				estado = ACEITACAO_LEX;
				tokenLex = Barra;

				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
			 	 */
				if (! ehBranco(letra)) {
					devolvido = letra; 
					lexemaLido = encurtar(lexemaLido);
				}
			}
		} else if (estado == 2) {
			if (letra == '*') {
				/* inicio de fim de comentario */
				estado = 3;
			} else if (letra == -1) {
				/*EOF encontrado*/
				erro = ER_LEX_EOF;
				erroMsg = (char *)"fim de arquivo nao esperado";
				abortar();
			} else if (C_INVALIDO) {
				/* caractere inválido */
				erro = ER_LEX_INVD;
				erroMsg = (char *)"caractere invalido";
				abortar();
				
			} 
		} else if (estado == 3) {
			if (letra == '/') {
				/* de fato fim de comentario volta ao inicio para ignorar*/
				estado = 0;
				lexemaLex = (char *)"";
			} else if (letra == '*') {
				/* ** no comentario, espera pela barra */
				estado = 3;
			} else if (letra == -1) {
				/*EOF encontrado*/
				erro = ER_LEX_EOF;
				erroMsg = (char *)"fim de arquivo nao esperado";
				abortar();
			} else {
				/* simbolo '*' dentro do comentario */
				estado = 2;
			}
		} else if (estado == 4) {
			if (letra == '=' || letra == '>') {
				/* lexemas de comparacao <= ou <> */
				estado = ACEITACAO_LEX;

				lexemaLex = concatenar(lexemaLex, l);;

				if (letra == '=')
					tokenLex = MenorIgual;
				else
					tokenLex = Diferente;

			} else if (ehBranco(letra) || ehDigito(letra) || ehLetra(letra)) {
				/* lexema de comparacao < */
				estado = ACEITACAO_LEX;

				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
			 	 */
				if (! ehBranco(letra)) {
					devolvido = letra;
					lexemaLido = encurtar(lexemaLido);
				}

				tokenLex = Menor;
			} else if (letra == -1) {
				/*EOF encontrado, assume que encontrou <*/
				estado = ACEITACAO_LEX;
			}
		} else if (estado == 5) {
			if (letra == '=') {
				/* lexema de comparacao >= */
				estado = ACEITACAO_LEX;

				lexemaLex = concatenar(lexemaLex, l);;
				tokenLex = MaiorIgual;

			} else if (ehBranco(letra) || ehDigito(letra) || ehLetra(letra)) {
				/* lexema de comparacao > */
				estado = ACEITACAO_LEX;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
				 */
				if (! ehBranco(letra)) {
					devolvido = letra;
					lexemaLido = encurtar(lexemaLido);
				}

				tokenLex = Maior;
			} else if (letra == -1) {
				/*EOF encontrado, assume que encontrou >*/
				estado = ACEITACAO_LEX;
			}

		} else if (estado == 6) {
			/* Inteiros */
			/* le ate encontrar diferente de numero */

			if (ehDigito(letra)) {
				lexemaLex = concatenar(lexemaLex, l);;
			} else {
				estado = ACEITACAO_LEX;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema */
				if (! ehBranco(letra)) {
					devolvido = letra;
					lexemaLido = encurtar(lexemaLido);
				}

                tokenLex = Literal;
				tipoLex = TP_Integer;
			}
		} else if (estado == 7) {
            /*lexema identificador _ . 
            concatena até achar uma letra ou numero */
            if (letra == '_' || letra == '.') {
                lexemaLex = concatenar(lexemaLex, l);;
            } else if (ehLetra(letra) || ehDigito(letra)) {
                lexemaLex = concatenar(lexemaLex, l);;
				estado = 8;
            }
        } else if (estado == 8) {
            /*lexema de identificador
            concatena ate finalizar o identificador ou palavra reservada */
            if (ehLetra(letra) ||  letra == '_' || letra == '.' ) {
                lexemaLex = concatenar(lexemaLex, l);;
            } else {
				estado = ACEITACAO_LEX;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema */
				if (! ehBranco(letra)) {
					devolvido = letra;
					lexemaLido = encurtar(lexemaLido);
				}

				tamanhoLex = 1;
				posLex = -1;
                tokenLex = identificaToken(lexemaLex);

                //adicionar novo token (identificador)
				enderecoLex = pesquisarRegistro(lexemaLex);
                if (enderecoLex == NULL) {
                   enderecoLex = adicionarRegistro(lexemaLex,tokenLex);
				   enderecoLex->simbolo.tipo = tipoLex;
				} else {
					/* palavras reservadas nao possuem tipo,
					 * portanto nao atualiza o tipo do registro
					 * lexico se nao tem tipo
					 */
					if (tipoLex != 0)
						tipoLex = enderecoLex->simbolo.tipo;
					tamanhoLex = enderecoLex->simbolo.tamanho;
				}
			} 
        } else if (estado == 9) {
            /*lexema de String
            concatena até encontrar o fechamento das aspas */

			lexemaLex = concatenar(lexemaLex, l);;
			tamanhoLex++;
            if (letra == '"') {
            	tokenLex = Literal;
				tipoLex = TP_Char;
                estado = ACEITACAO_LEX;
            } else if (letra == EOF) {
				/*EOF encontrado*/
				erro = ER_LEX_EOF;
				erroMsg = (char *)"fim de arquivo nao esperado.";
				abortar();
			} else if (C_INVALIDO) {
				/* caractere inválido */
				erro = ER_LEX_INVD;
				erroMsg = (char *)"caractere invalido";
				abortar();
				
			}
        } else if (estado == 10) {
			/* hexadecimal */
			if (letra == 'x') {           /* de fato hexadecimal */
				lexemaLex = concatenar(lexemaLex, l);;
				estado = 11;
			} else if (ehDigito(letra)) {   /* numero começando com 0 */ 
				lexemaLex = concatenar(lexemaLex, l);;
				estado = 6;
			} else if (!ehLetra(letra) && !ehDigito(letra)) {
				/* valor 0 */

				tokenLex = Literal;
				tipoLex = TP_Integer;

				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
			 	 */
				if (! ehBranco(letra)) {
					devolvido = letra;
					lexemaLido = encurtar(lexemaLido);
				}

				estado = ACEITACAO_LEX;
				
			} else {
				erro = ER_LEX_N_ID;
				erroMsg = (char *)"lexema nao identificado";
				lexemaLido = encurtar(lexemaLido);
				abortar();
			}

		} else if (estado == 11) {
			/* parte numerica do hexadecimal deve conter pelo menos um numero ou A-F */
			if (ehDigito(letra) || (97 <= letra && letra <= 102)) {
				/* leu 0x[A-F0-9] */
				lexemaLex = concatenar(lexemaLex, l);;
				estado = 12;
			} else {
				/* leu só 0x, invalido */
				erro = ER_LEX_N_ID;
				erroMsg = (char *)"lexema nao identificado";
				lexemaLido = encurtar(lexemaLido);
				abortar();
			}
		} else if (estado == 12) {
			/* resto do valor hexadecimal */
			if (ehDigito(letra) || (97 <= letra && letra <= 102)) {

				lexemaLex = concatenar(lexemaLex, l);;

			} else {

				tokenLex = Literal;
				tipoLex = TP_Char;

				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
			 	 */
				if (! ehBranco(letra)) {
					devolvido = letra;
					lexemaLido = encurtar(lexemaLido);
				}

				estado = ACEITACAO_LEX;
			}
		}

fimloop:
		/* se ja aceitou nao le o proximo */
		if (estado != ACEITACAO_LEX)
			letra = minusculo(lexGetChar());

	}

	/* leu EOF */
	if (letra <= 0) lex = 0;

	DEBUGLEX((char *)"LEX: lexema:%s token:%d tipo:%d tam: %d\n",lexemaLex,tokenLex,tipoLex,tamanhoLex);
}

/* retorna o tipo do identificador */
Tipo buscaTipo(char *identificador)
{
	return pesquisarRegistro(identificador)->simbolo.tipo;
}

void adicionarReservados(void)
{
	adicionarRegistro((char *)"const",Const);
	adicionarRegistro((char *)"var",Var);
	adicionarRegistro((char *)"integer",Integer);
	adicionarRegistro((char *)"char",Char);
	adicionarRegistro((char *)"for",For);
	adicionarRegistro((char *)"if",If);
	adicionarRegistro((char *)"else",Else);
	adicionarRegistro((char *)"and",And);
	adicionarRegistro((char *)"or",Or);
	adicionarRegistro((char *)"not",Not);
	adicionarRegistro((char *)"=",Igual);
	adicionarRegistro((char *)"to",To);
	adicionarRegistro((char *)"((char *)",A_Parenteses);
	adicionarRegistro((char *)")",F_Parenteses);
	adicionarRegistro((char *)"<",Menor);
	adicionarRegistro((char *)">",Maior);
	adicionarRegistro((char *)"<>",Diferente);
	adicionarRegistro((char *)">=",MaiorIgual);
	adicionarRegistro((char *)"<=",MenorIgual);
	adicionarRegistro((char *)",",Virgula);
	adicionarRegistro((char *)"+",Mais);
	adicionarRegistro((char *)"-",Menos);
	adicionarRegistro((char *)"*",Vezes);
	adicionarRegistro((char *)"/",Barra);
	adicionarRegistro((char *)";",PtVirgula);
	adicionarRegistro((char *)"{",A_Chaves);
	adicionarRegistro((char *)"}",F_Chaves);
	adicionarRegistro((char *)"then",Then);
	adicionarRegistro((char *)"readln",Readln);
	adicionarRegistro((char *)"step",Step);
	adicionarRegistro((char *)"write",Write);
	adicionarRegistro((char *)"writeln",Writeln);
	adicionarRegistro((char *)"%",Porcento);
	adicionarRegistro((char *)"[",A_Colchete);
	adicionarRegistro((char *)"]",F_Colchete);
	adicionarRegistro((char *)"do",Do);
}

struct Celula *adicionarRegistro(char *lexema, Tokens token)
{
	int pos = hash(lexema,TAM_TBL);
	struct Celula *cel = (struct Celula *) malloc(sizeof(struct Celula));
	struct Simbolo *simb = (struct Simbolo *) malloc(sizeof(struct Simbolo));
	simb->token = token;
	simb->lexema = lexema;
	simb->classe = CL_Nulo;
	simb->tipo = TP_Nulo;
	simb->tamanho=1;
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
	int pos = hash(procurado,TAM_TBL);
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
	printf("============TABELA DE SÍMBOLOS=============\n");
	for (int i=0; i<TAM_TBL; ++i) {
		if (tabelaSimbolos[i] != NULL) {
			printf((char *)"|\t%d\t|-> %s[%d]", i, tabelaSimbolos[i]->simbolo.lexema,tabelaSimbolos[i]->simbolo.tamanho);
			struct Celula *prox = tabelaSimbolos[i]->prox;
			while (prox != NULL){
				printf((char *)" -> %s",prox->simbolo.lexema);
				prox = prox->prox;
			}
			printf((char *)"\n");
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

/* Acoes semanticas 
 * Cada metodo implementa a verificacao de uma acao semantica,
 * em caso de erro, chama a funcao erroSintatico e aborta o programa
 */

void defTipo(Tipo tipo)
{
	tipoLex = tipo;
}

void defClasse(Classe classe)
{
	classeLex = classe;
}

/* converte tipo integer para logico 
 * tipo logico é implicito, 0 é falso,
 * diferente de 0 é verdadeiro.
 * caracteres não são válidos
 */
Tipo toLogico(Tipo tipo)
{
	Tipo t = tipo;
	if (tipo == TP_Integer) t = TP_Logico;
	return t;
}

/* verifica se o tamanho dos arrays eh valido */
void verificaTam(int tam)
{
	/* tipo char ocupa 1 byte portanto o array pode ter 4k posicoes */
	if (tipoLex == TP_Char && tam > 4000) erroSintatico(ER_SIN_TAMVET);

	/* tipo integer ocupa 2 bytes portanto o array pode ter 4k/2 posicoes */
	if (tipoLex == TP_Integer && tam > 2000) erroSintatico(ER_SIN_TAMVET);

	/* else 
	 * atualiza o registro lexico e tabela de simbolos com o tamanho
	 */
	tamanhoLex = tam;
	enderecoLex->simbolo.tamanho = tam;
}

/* Verificacao de tipo
 * verifica se tipo A == tipo B
 */
void verificaTipo(Tipo A, Tipo B)
{

	if ( A != B) {
		erroSintatico(ER_SIN_T_INC);
	}
	
}

/* Verificacao de classe
 * atualiza na tabela de simbolos o tipo e a classe do elemento,
 * caso a classe ja esteja definida, significa que a variavel
 * ou constante ja foi declarada
 */
void verificaClasse(char* lex, Tipo tipo)
{
	enderecoLex = pesquisarRegistro(lex);

	if (enderecoLex->simbolo.classe == CL_Nulo) {
		enderecoLex->simbolo.tipo = tipo;
		enderecoLex->simbolo.classe = classeLex;
	} else {
		erroSintatico(ER_SIN_JADEC);
	}
}

/* Verificacao de declaracao
 * verifica se o identificador ja foi declarado
 * ou se é constante
 */
void verificaDeclaracao(char *identificador)
{
	if (pesquisarRegistro(identificador)->simbolo.classe == CL_Nulo)
		erroSintatico(ER_SIN_NDEC);
}

/* verifica se o identificador eh constante */
void verificaConst(char *identificador)
{
	if (pesquisarRegistro(identificador)->simbolo.classe == CL_Const)
		erroSintatico(ER_SIN_C_INC);
}

/* Verificacao de atribuicao a vetor 
 * operacoes sobre vetores so podem acontecer posicao a posicao
 * operacoes na forma vet1 = vet2 nao sao permitidas
 */
void verificaAtrVetor(void)
{
	/* ATENCAO:
	 * Este erro poderia ter sido deixado para tempo de execucao
	 *
	 * atribuicao de strings 
	 * a string deve ter tamanho menor que o tamanho do vetor -1
	 * pois ainda deve ser acrescentado o $ para encerrar a string
	 */
	if (enderecoLex->simbolo.tipo == TP_Char) {
		if (enderecoLex->simbolo.tamanho < tamanhoLex+1)
			erroSintatico(ER_SIN_TAMVET);

	} 

	/* atribuicoes a vetores nao string */
	else if (enderecoLex->simbolo.tamanho > 1 && posLex == -1) {
		erroSintatico(ER_SIN_T_INC);
	}
}

/* declara novo temporario */
int novoTemp(int t)
{
	TP += t;
	return (TP-t);
}

int novoRot()
{
	return RT++;
}

void zeraTemp(void)
{
	TP = 0x100;
}

/* operacoes aritmeticas, ADD, SUB, IMUL e IDIV
 * recebe a operacao op, o registrador destino RD, o registrador origem RO
 * o registrador resultado RR e o pai
 */
void aritmeticos(char* op, char *RD, char *RO, char *RR, struct Fator *pai)
{
	pai->endereco = novoTemp(TAM_INT);

	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;================inicio de operacao %s===================\n", op);
	if (op == (char *)"IMUL" || op == (char *)"IDIV") {

		CONCAT_BUF((char *)"\t%s %s\n",op, RO);

	} else if (op == (char *)"ADD" || op == (char *)"SUB") {

		CONCAT_BUF((char *)"\t%s %s, %s\n",op, RD, RO);

	}

	CONCAT_BUF((char *)"\tMOV DS:[0%Xh], %s\t\t\t\t\t\t\t;grava o resultado da operacao no endereco\n", pai->endereco, RR);
}

/* comparacoes nao string */
void comp(char *op, struct Fator *pai)
{
	rot verdadeiro = novoRot();
	rot falso = novoRot();

	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;================inicio de comparacao %s===================\n", op);
	CONCAT_BUF((char *)"\tCMP AX, BX\n");
	CONCAT_BUF((char *)"\t%s R%d\t\t\t\t\t\t\t\t;comparacao verdadeiro\n", op, verdadeiro);
	CONCAT_BUF((char *)"\tMOV AX, 0\n");
	CONCAT_BUF((char *)"\tJMP R%d\t\t\t\t\t\t\t\t;comparacao falso\n",falso);

	CONCAT_BUF((char *)"\tR%d:\t\t\t\t\t\t\t\t;verdadeiro\n",verdadeiro);
	CONCAT_BUF((char *)"\tMOV AX, 1\n");
	CONCAT_BUF((char *)"\tR%d:\t\t\t\t\t\t\t\t;falso\n",falso);

	pai->endereco = novoTemp(TAM_INT);
	pai->tipo = TP_Logico;

	CONCAT_BUF((char *)"MOV DS:[0%Xh], AX\t\t\t\t\t\t\t\t;guarda no endereco o resultado da expressao\n", pai->endereco);
}

void compChar(struct Fator *pai)
{
	rot inicio = novoRot();
	rot verdadeiro = novoRot();
	rot falso = novoRot();
	rot fimStr = novoRot();
	rot iguais = novoRot();

	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;================inicio de comparacao de string===================\n");
	CONCAT_BUF((char *)"R%d:\t\t\t\t\t\t\t\t;marca o inicio do loop\n",inicio);
	CONCAT_BUF((char *)"MOV CL, DS:[BX] \t\t;move para CL o caractere da string em BX\n");

	CONCAT_BUF((char *)"MOV DX, BX\t\t\t\t\t;Troca AX de lugar com BX\n");
	CONCAT_BUF((char *)"MOV BX, AX\t\t\t\t\t;pois so BX pode ser utilizado\n");
	CONCAT_BUF((char *)"MOV AX, DX\t\t\t\t\t;para acessar memoria\n");

	CONCAT_BUF((char *)"MOV CH, DS:[BX] \t\t;move para CH o caractere da string em BX\n");
	CONCAT_BUF((char *)"CMP CH, CL\t\t\t\t\t\t\t\t;compara as strings\n");
	CONCAT_BUF((char *)"JE R%d\t\t\t\t\t\t\t\t;jmp verdadeiro\n", verdadeiro);
	CONCAT_BUF((char *)"MOV AX, 0\n");
	CONCAT_BUF((char *)"JMP R%d\t\t\t\t\t\t\t\t;strings diferentes\n",fimStr);
	CONCAT_BUF((char *)"R%d:\t\t\t\t\t\t\t\t;caracteres iguais\n",verdadeiro);
	CONCAT_BUF((char *)"CMP CH, 24h\t\t\t\t\t\t\t\t;verifica se chegou no final da primeira string\n");
	CONCAT_BUF((char *)"JE R%d\t\t\t\t\t\t\t\t;iguais\n",iguais);
	CONCAT_BUF((char *)"CMP CL, 24h\t\t\t\t\t\t\t\t;verifica se chegou no final da segunda string\n");
	CONCAT_BUF((char *)"JE R%\t\t\t\t\t\t\t\t;iguais\n",iguais);
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t;================ nao chegou fim de nenhuma string===================\n");
	CONCAT_BUF((char *)"ADD AX, 1\t\t\t\t\t\t\t\t;anda uma posicao no primeiro string\n");
	CONCAT_BUF((char *)"ADD BX, 1\t\t\t\t\t\t\t\t;anda uma posicao no segundo string\n");

	CONCAT_BUF((char *)"JMP R%d\t\t\t\t\t\t\t\t;volta ao inicio do loop\n", inicio);

	CONCAT_BUF((char *)"R%d:\t\t\t\t\t\t\t\t;strings iguais\n",iguais);
	CONCAT_BUF((char *)"MOV AX, 1\n");

	CONCAT_BUF((char *)"R%d:\t\t\t\t\t\t\t\t;fim de string\n", fimStr);

	pai->endereco = novoTemp(TAM_INT);
	pai->tipo = TP_Logico;

	CONCAT_BUF((char *)"MOV DS:[0%Xh], AX\t\t\t\t\t\t\t\t;salva no endereco o resultado\n", pai->endereco);
}

/* move cursor para linha de baixo */
void proxLinha()
{
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;================gera quebra de linha===================\n");
	CONCAT_BUF((char *)"MOV AH, 02h\n");
	CONCAT_BUF((char *)"MOV DL, 0Dh\n");
	CONCAT_BUF((char *)"INT 21h\n");
	CONCAT_BUF((char *)"MOV DL, 0Ah\n");
	CONCAT_BUF((char *)"INT 21h\n");
}

/* inicia o buffer */
void iniciarCodegen(void)
{ 
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"iniciarCodegen");

	buffer = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
	aux = (char *)malloc(sizeof(char) * MAX_AUX_SIZE);

	/* Pilha */
	CONCAT_BUF((char *)"SSEG SEGMENT STACK\t\t\t\t\t\t\t\t\t\t\t;inicio seg. pilha\n");
	CONCAT_BUF((char *)"\tbyte 0%Xh DUP(?)\t\t\t\t\t\t\t\t\t\t;dimensiona pilha\n",CD);
	CONCAT_BUF((char *)"SSEG ENDS\t\t\t\t\t\t\t\t\t\t\t\t\t;fim seg. pilha\n");

}

/* concatena garantindo que o ultimo caractere eh o \n */
void buf_concatenar(void)
{

	int buf_size = strlen(buffer);


	/* se o buffer vai encher, escreve no arquivo e esvazia */
	if ((buf_size+strlen(aux)) >= MAX_BUF_SIZE)
		flush();

	//buffer = concatenar(buffer, aux);
	//buf_size = strlen(buffer);

	aux[0] = '\0'; /* limpa aux */
}

/* escreve buffer no arquivo progAsm 
 * e em seguida limpa buffer
 */
void flush(void)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"flush");

	//printf("%s",buffer);

	/* limpa o buffer */
	//buffer[0] = '\0';
}

/* inicia o bloco de declaracoes asm */
void initDeclaracao(void)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"initDeclaracao");

	/* dados */
	CONCAT_BUF((char *)"dseg SEGMENT PUBLIC\t\t\t\t\t\t\t\t\t\t\t;inicio seg. dados\n");
	CONCAT_BUF((char *)"\tbyte 0%Xh DUP(?)\t\t\t\t\t\t\t\t\t\t;temporarios\n",CD);
}

/* finaliza o bloco de declaracoes asm 
 * e inicia o bloco de comandos asm
 */
void fimDecInitCom(void)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"fimDecInitCom");

	/* fim declaracao */
	CONCAT_BUF((char *)"DSEG ENDS\t\t\t\t\t\t\t\t\t\t\t\t\t;fim seg. dados\n");
	/* comandos */
	CONCAT_BUF((char *)"CSEG SEGMENT PUBLIC\t\t\t\t\t\t\t\t\t\t\t;inicio seg. codigo\n");
	CONCAT_BUF((char *)"\tASSUME CS:CSEG, DS: DSEG\n");
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;================inicio do programa===================\nSTRT:\n");

}

void fimComandos(void)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"fimComandos");

	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;================interrompe o programa===================\n");
	CONCAT_BUF((char *)"\tMOV AH, 4Ch\n");
	CONCAT_BUF((char *)"\tINT 21h\n");
	CONCAT_BUF((char *)"\tCSEG ENDS\t\t\t\t\t\t\t\t\t;fim seg. codigo\n");
	CONCAT_BUF((char *)"END STRT\t\t\t\t\t\t\t\t\t\t;fim programa\n");
}

/* gera codigo para acesso a array */
void acessoArray(struct Fator *pai, struct Fator *filho)
{
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;================acesso a array===================\n");
	CONCAT_BUF((char *)"\tMOV AX, 0%Xh \t\t\t\t\t\t;endereco base\n", pai->endereco);
	/* adiciona o resultado da expressao ao inicio do array para encontrar a posicao de acesso */
	CONCAT_BUF((char *)"\tADD AX, DS:[0%Xh] \t\t\t\t\t\t\t\t;soma com offset\n", filho->endereco);
	CONCAT_BUF((char *)"\tfim acesso a array");
}

/* gera o asm da declaracao de uma variavel ou constante 
 * e retorna o endereco que foi alocado 
 */
void genDeclaracao(Tipo t, Classe c, int tam, char *val, int negativo)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"genDeclaracao");

	char *tipo;  /* string de tipo        */
	char *classe;/* const ou var          */
	char *nome;  /* string sword ou byte  */
	char *valor; /* string de valor ou ?  */
	int n_bytes; /* numero de bytes usado */

	/* marca o endereco de memoria na tabela de simbolos */
	enderecoLex->simbolo.memoria = CD;

	/* string de tipo para o comentario */
	if (t == TP_Integer) {
		tipo = (char *)"int";
		nome = (char *)"sword";
		n_bytes = TAM_INT*tam;
	} else if (t == TP_Char) {
		if (tam == 0)
			tipo = (char *)"caract";
		else
			tipo = (char *)"string";

		nome = (char *)"byte";
		n_bytes = TAM_CHA*tam;
	} else {
		tipo = (char *)"logic";
		nome = (char *)"byte";
		n_bytes = 1;
	}

	if (c == CL_Const)
		classe = (char *)"const";
	else
		classe = (char *)"var";

	/* string de valor se existir */
	if (val != NULL) {

		valor = val;

		if (negativo)
			valor = concatenar((char *)"-",valor);

		/* adiciona $ ao fim da string */
		if (t == TP_Char) {
			valor = concatenar(removeAspas(valor),(char *)"$");
			valor = concatenar((char *)"\"",valor);
			valor = concatenar(valor,(char *)"\"");
		}

	} else {
		valor = (char *)"?";
	}

	/* arrays */
	if (tam > 1) {
		CONCAT_BUF((char *)"\t%s 0%Xh DUP(?)\t\t\t\t\t\t\t\t\t;var. Vet %s. em 0%Xh\n", nome, tam, tipo, CD);
	} else {
		CONCAT_BUF((char *)"\t%s %s\t\t\t\t\t\t\t\t\t\t\t\t; %s. %s. em 0%Xh\n", nome, valor, classe, tipo, CD);
	}

	/* incrementa a posicao de memoria com o numero de bytes utilizado */
	CD+=n_bytes;
}

void genAtribuicao(struct Fator *pai, struct Fator *fator)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"genAtribuicao");

	/* int e logico */
	if (pai->tipo == TP_Integer || pai->tipo == TP_Logico) {
		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================atribuicao de inteiros e logicos===================\n");
		CONCAT_BUF((char *)"\tMOV AX, DS:[0%Xh] \t\t\t\t\t\t\t;endereco do resultado para ax\n", fator->endereco);
		CONCAT_BUF((char *)"\tMOV DS:[0%Xh], AX \t\t\t\t\t\t;ax para posicao de memoria da variavel\n", pai->endereco);
		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================fim atribuicao de inteiros e logicos===================\n");
	} else {
		/* string, move caractere por caractere */
		rot inicio = novoRot();
		rot fim = novoRot();

		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================atribuicao de strings===================\n");
		CONCAT_BUF((char *)"\tMOV BX, 0%Xh \t\t\t\t\t\t;endereco da string para bx\n", fator->endereco);
		CONCAT_BUF((char *)"\tMOV DI, 0%Xh \t\t\t\t\t\t;endereco do pai para concatenar\n", pai->endereco);

		CONCAT_BUF((char *)"\tR%d: \t\t\t\t\t\t\t\t\t\t\t;inicio do loop\n", inicio);
		CONCAT_BUF((char *)"\tMOV CL, DS:[BX] \t\t\t\t\t\t;joga caractere em CL\n");
		CONCAT_BUF((char *)"\tMOV DS:[DI], CL \t\t\t\t\t\t;transfere pro endereco a string\n");
		CONCAT_BUF((char *)"\tCMP CL, 24h \t\t\t\t\t\t\t\t;verifica se chegou no fim\n");
		CONCAT_BUF((char *)"\tJE R%d \t\t\t\t\t\t\t\t;fim da str\n", fim);
		CONCAT_BUF((char *)"\tADD DI, 1 \t\t\t\t\t\t\t\t;avanca posicao a receber o proximo caractere\n");
		CONCAT_BUF((char *)"\tADD BX, 1 \t\t\t\t\t\t\t\t;proximo caractere\n");
		CONCAT_BUF((char *)"\tJMP R%d\n", inicio);
		CONCAT_BUF((char *)"\tR%d:\n", fim);
		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================fim atribuicao de strings===================\n");
	}
}

/* segunda acao do for
 * gera a declaracao e o fim
 * for ID=EXP TO EXP
 */
void genRepeticao(struct Fator *pai, struct Fator *filho, rot inicio, rot fim)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"genRepeticao");

	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================inicio da repeticao===================\n");
	CONCAT_BUF((char *)"\tR%d: \t\t\t\t\t\t\t\t\t\t;inicio do loop \n", inicio);
	CONCAT_BUF((char *)"\tMOV CX, DS:[0%Xh] \t\t\t\t\t\t;move o valor de ID para cx \n",pai->endereco);
	CONCAT_BUF((char *)"\tMOV BX, DS:[0%Xh] \t\t\t\t\t\t;move o resultado da TO EXP para BX \n", filho->endereco);
	CONCAT_BUF((char *)"\tCMP CX, BX \t\t\t\t\t\t\t\t\t;compara os valores \n");
	CONCAT_BUF((char *)"\tJG R%d \t\t\t\t\t\t\t\t\t\t;vai para o fim se id > exp \n", fim);
}

/* fim do loop de repeticao 
 * incrementa e desvia
 */
void genFimRepeticao(struct Fator *pai, rot inicio, rot fim,char *step)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"genFimRepeticao");

	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================fim da repeticao===================\n");
	CONCAT_BUF((char *)"\tMOV CX, DS:[0%Xh] \t\t\t\t\t\t;move o valor de ID para cx \n",pai->endereco);
	CONCAT_BUF((char *)"\tADD CX, %s \t\t\t\t\t\t\t\t\t;soma o step\n", step);
	CONCAT_BUF((char *)"\tMOV DS:[0%Xh], CX \t\t\t\t\t\t;guarda o valor de id \n", pai->endereco);
	CONCAT_BUF((char *)"\tJMP R%d \t\t\t\t\t\t\t\t\t\t;volta para o inicio\n", inicio);
	CONCAT_BUF((char *)"\tR%d: \t\t\t\t\t\t\t\t\t\t;fim do loop\n", fim);
}

/* gera o inicio do comando de teste */
void genTeste(struct Fator *filho, rot falso, rot fim)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"genTeste");

	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================inicio do teste===================\n");
	CONCAT_BUF((char *)"\tMOV AX, DS:[0%Xh] \t\t\t\t\t\t;move para ax o resultado da expressao logica\n", filho->endereco);
	CONCAT_BUF((char *)"\tCMP AX, 0 \t\t\t\t\t\t\t\t;checa se eh falso \n");
	CONCAT_BUF((char *)"\tJE R%d \t\t\t\t\t\t\t\t;vai para falso \n", falso);
}

/* gera a parte do else, que pode ser vazia */
void genElseTeste(rot falso, rot fim)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"genElseTeste");

	/* else */
	CONCAT_BUF((char *)"\tJMP R%d \t\t\t\t\t\t\t\t;fim verdadeiro\n",fim);
	CONCAT_BUF((char *)"\tR%d: \t\t\t\t\t\t\t\t;inicio else\n",falso);
}

/* gera o rotulo de fim do teste */
void genFimTeste(rot fim)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"genFimTeste");

	CONCAT_BUF((char *)"\tR%d: \t\t\t\t\t\t\t\t;fim do teste\n", fim);
}

void genEntrada(struct Fator *pai)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"genEntrada");

	/* buffer para entrada do teclado, tam max = 255 */
	int buffer = novoTemp(pai->tamanho+3);
	rot inicio = novoRot();
	rot meio = novoRot();
	rot fim = novoRot();

	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================inicio do comando de entrada===================\n");
	CONCAT_BUF((char *)"\tMOV DX, 0%Xh \t\t\t\t\t\t;buffer para receber a string\n", buffer);
	CONCAT_BUF((char *)"\tMOV AL, 0%Xh \t\t\t\t\t\t;tamanho do buffer \n", pai->tamanho);
	CONCAT_BUF((char *)"\tMOV DS:[0%Xh], AL \t\t\t\t\t\t;move tamanho do buffer para a primeira posicao do buffer\n",buffer);
	CONCAT_BUF((char *)"\tMOV AH, 0Ah \t\t\t\t\t\t; interrupcao para leitura do teclado\n");
	CONCAT_BUF((char *)"\tINT 21h\n");
	/* string */
	if (pai->tipo == TP_Char) {

		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================entrada de strings===================\n");

		CONCAT_BUF((char *)"\tMOV CX, DS:[0%Xh] \t\t\t\t\t\t;de caracteres lidos fica na segunda posicao do buffer\n", buffer+1);
		CONCAT_BUF((char *)"\tMOV DX, 0 \t\t\t\t\t\t;contador de posicoes\n");
		CONCAT_BUF((char *)"\tMOV BX, 0%Xh \t\t\t\t\t\t;contador de posicao no end pai\n", pai->endereco);

		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== transfere para o endereco do pai o conteudo lido===================\n");
		
		CONCAT_BUF((char *)"\tR%d:\n",inicio); /* inicio do loop */
		CONCAT_BUF((char *)"\tCMP DX, CX \t\t\t\t\t\t\t\t;compara o contador de caracteres lidos com quantas posicoes ja foram transferidas\n");
		CONCAT_BUF((char *)"\tJE R%d \t\t\t\t\t\t\t\t;se for igual, ja leu tudo, vai pro fim\n",fim);
		CONCAT_BUF((char *)"\tMOV DI, 0%Xh \t\t\t\t\t\t;move para DI o endereco base a partir da 3 posicao \n", buffer+2);
		CONCAT_BUF((char *)"\tADD DI, DX \t\t\t\t\t\t\t\t;soma offset ao endereco base \n");
		CONCAT_BUF((char *)"\tMOV DI, DS:[DI] \t\t\t\t\t\t;move o caractere para DI\n");
		CONCAT_BUF((char *)"\tMOV DS:[BX], DI \t\t\t\t\t\t;move DI para o endereco do pai\n");
		CONCAT_BUF((char *)"\tADD DX, 1 \t\t\t\t\t\t\t\t;soma 1 no offset\n");
		CONCAT_BUF((char *)"\tADD BX, 1 \t\t\t\t\t\t\t\t;soma 1 no indice do endereco\n");
		CONCAT_BUF((char *)"\tJMP R%d \t\t\t\t\t\t\t\t;pula para o inicio\n", inicio);
		CONCAT_BUF((char *)"\tR%d: \t\t\t\t\t\t\t\t;fim do loop\n", fim);
		CONCAT_BUF((char *)"\tMOV CX, 24h\n");
		CONCAT_BUF((char *)"\tMOV DS:[BX], CX \t\t\t\t\t\t;coloca $ no final \n");
		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================fim entrada de strings===================\n");
	}

	/* inteiro */
	else if (pai->tipo == TP_Integer) {
		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== inicio entrada de inteiros===================\n");
		CONCAT_BUF((char *)"\tMOV DI, 0%Xh \t\t\t\t\t\t;posição do string\n",buffer+2);
		CONCAT_BUF((char *)"\tMOV AX, 0 \t\t\t\t\t\t;acumulador\n");
		CONCAT_BUF((char *)"\tMOV BX, 10 \t\t\t\t\t\t;base decimal\n");
		CONCAT_BUF((char *)"\tMOV DX, 1 \t\t\t\t\t\t;valor sinal +\n");
		CONCAT_BUF((char *)"\tMOV BH, 0\n");
		CONCAT_BUF((char *)"\tMOV BL, DS:[DI] \t\t\t\t\t\t;caractere\n");
		CONCAT_BUF((char *)"\tCMP CX, 2Dh \t\t\t\t\t\t\t\t;verifica sinal\n");
		CONCAT_BUF((char *)"\tJNE R%d \t\t\t\t\t\t\t\t;se não negativo jmp inicio\n", inicio);
		CONCAT_BUF((char *)"\tMOV DX, -1 \t\t\t\t\t\t;valor sinal -\n");
		CONCAT_BUF((char *)"\tADD DI, 1 \t\t\t\t\t\t\t\t;incrementa base\n");
		CONCAT_BUF((char *)"\tMOV BL, DS:[DI] \t\t\t\t\t\t;próximo caractere\n");
		CONCAT_BUF((char *)"\tR%d: \t\t\t\t\t\t\t\t;inicio\n",inicio);
		CONCAT_BUF((char *)"\tPUSH DX \t\t\t\t\t\t\t\t;empilha sinal\n");
		CONCAT_BUF((char *)"\tMOV DX, 0 \t\t\t\t\t\t;reg. multiplicação\n");
		CONCAT_BUF((char *)"\tR%d: \t\t\t\t\t\t\t\t;meio\n",meio);
		CONCAT_BUF((char *)"\tCMP CX, 24h \t\t\t\t\t\t\t\t;verifica fim string\n");
		CONCAT_BUF((char *)"\tJE R%d \t\t\t\t\t\t\t\t;salta fim se fim string\n",fim);
		CONCAT_BUF((char *)"\tIMUL BX \t\t\t\t\t\t\t\t;mult. 10\n");
		CONCAT_BUF((char *)"\tADD CX, -48 \t\t\t\t\t\t\t\t;converte caractere\n");
		CONCAT_BUF((char *)"\tADD AX, CX \t\t\t\t\t\t\t\t;soma valor caractere\n");
		CONCAT_BUF((char *)"\tADD DI, 1 \t\t\t\t\t\t\t\t;incrementa base\n");
		CONCAT_BUF((char *)"\tMOV BH, 0\n");
		CONCAT_BUF((char *)"\tMOV BL, DS:[DI] \t\t\t\t\t\t;próximo caractere\n");
		CONCAT_BUF((char *)"\tJMP R%d \t\t\t\t\t\t\t\t;jmp meio\n",meio);
		CONCAT_BUF((char *)"\tR%d: \t\t\t\t\t\t\t\t;fim\n", fim);
		CONCAT_BUF((char *)"\tPOP BX \t\t\t\t\t\t\t\t;desempilha sinal\n");
		CONCAT_BUF((char *)"\tIMUL BX \t\t\t\t\t\t\t\t;mult. sinal\n");

		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== transfere resultado para o pai===================\n");
		CONCAT_BUF((char *)"\tMOV BX, 0%Xh\n", pai->endereco);
		CONCAT_BUF((char *)"\tMOV DS:[BX], AX\n");
		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== fim entrada de inteiros===================\n");
	}

	proxLinha();
}

/* geracao de codigo para saida de texto */
void genSaida(struct Fator *pai, int ln)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"genSaida");

	int endereco = pai->endereco;
	rot inicio = novoRot();
	rot meio = novoRot();
	rot fim = novoRot();

	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================inicio saida===================\n");
	/* conversao de inteiro para string */
	if (pai->tipo == TP_Integer) {
		endereco = novoTemp(pai->tamanho);
		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================saida de inteiros===================\n");
		CONCAT_BUF((char *)"\tMOV AX, DS:[0%Xh]\t\t\t\t\t\t\t;carrega para AX o valor\n", pai->endereco);
		CONCAT_BUF((char *)"\tMOV DI, 0%Xh \t\t\t\t\t\t\t\t;end. string temp.\n",endereco);
		CONCAT_BUF((char *)"\tMOV CX, 0 \t\t\t\t\t\t\t\t\t;contador\n");
		CONCAT_BUF((char *)"\tCMP AX, 0 \t\t\t\t\t\t\t\t\t;verifica sinal\n");
		CONCAT_BUF((char *)"\tJGE R%d \t\t\t\t\t\t\t\t\t\t;salta se número positivo\n", inicio);
		CONCAT_BUF((char *)"\tMOV BL, 2Dh \t\t\t\t\t\t\t\t;senão, escreve sinal –\n");
		CONCAT_BUF((char *)"\tMOV DS:[DI], BL\n");
		CONCAT_BUF((char *)"\tADD DI, 1 \t\t\t\t\t\t\t\t\t;incrementa índice\n");
		CONCAT_BUF((char *)"\tneg AX \t\t\t\t\t\t\t\t\t\t;toma módulo do número\n");
		CONCAT_BUF((char *)"\tR%d:\n",inicio);
		CONCAT_BUF((char *)"\tMOV BX, 10 \t\t\t\t\t\t\t\t\t;divisor\n");
		CONCAT_BUF((char *)"\tR%d:\n",meio);
		CONCAT_BUF((char *)"\tADD CX, 1 \t\t\t\t\t\t\t\t\t;incrementa contador\n");
		CONCAT_BUF((char *)"\tMOV DX, 0 \t\t\t\t\t\t\t\t\t;estende 32bits p/ div.\n");
		CONCAT_BUF((char *)"\tIDIV BX \t\t\t\t\t\t\t\t\t;divide DXAX por BX\n");
		CONCAT_BUF((char *)"\tPUSH DX \t\t\t\t\t\t\t\t\t;empilha valor do resto\n");
		CONCAT_BUF((char *)"\tCMP AX, 0 \t\t\t\t\t\t\t\t\t;verifica se quoc. é 0\n");
		CONCAT_BUF((char *)"\tJNE R%d \t\t\t\t\t\t\t\t\t\t;se não é 0, continua\n", meio);
		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================agora, desemp. os valores e escreve o string===================\n");
		CONCAT_BUF((char *)"\tR%d:\n",fim);
		CONCAT_BUF((char *)"\tPOP DX \t\t\t\t\t\t\t\t\t\t;desempilha valor\n");
		CONCAT_BUF((char *)"\tADD DX, 30h \t\t\t\t\t\t\t\t;transforma em caractere\n");
		CONCAT_BUF((char *)"\tMOV DS:[DI],DL \t\t\t\t\t\t\t\t;escreve caractere\n");
		CONCAT_BUF((char *)"\tADD DI, 1 \t\t\t\t\t\t\t\t\t;incrementa base\n");
		CONCAT_BUF((char *)"\tADD CX, -1 \t\t\t\t\t\t\t\t\t;decrementa contador\n");
		CONCAT_BUF((char *)"\tCMP CX, 0 \t\t\t\t\t\t\t\t\t;verifica pilha vazia\n");
		CONCAT_BUF((char *)"\tJNE R%d \t\t\t\t\t\t\t\t\t\t;se não pilha vazia, loop\n", fim);
		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================grava fim de string===================\n");
		CONCAT_BUF((char *)"\tMOV DL, 24h \t\t\t\t\t\t\t\t;fim de string\n");
		CONCAT_BUF((char *)"\tMOV DS:[DI], DL \t\t\t\t\t\t\t;grava '$'\n");
		CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================fim saida de inteiros===================\n");
	}

	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================exibe string===================\n");
	CONCAT_BUF((char *)"\tMOV DX, 0%Xh\n", endereco);
	CONCAT_BUF((char *)"\tMOV AH, 09h\n");
	CONCAT_BUF((char *)"\tINT 21h	\n");
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================fim saida===================\n");

	if (ln)
		proxLinha();
}

void fatorGeraLiteral(struct Fator *fator, char *val)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"fatorGeraLiteral");

	
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================gera literal===================\n");
	/* string */
	if (fator->tipo == TP_Char) {
		A_SEG_PUB
			CONCAT_BUF((char *)"\tbyte \"%s$\" \t\t\t\t\t\t\t\t;string %s em 0%Xh\n",removeAspas(val),val,CD);
		F_SEG_PUB
		
		fator->endereco = CD;
		fator->tamanho = tamanhoLex;

		CD += (fator->tamanho+1) * TAM_CHA;

	/* nao string */
	} else {
		fator->endereco = novoTemp(TAM_INT);
		fator->tamanho = 1;
		CONCAT_BUF((char *)"\tMOV AX, %s \t\t\t\t\t\t\t\t\t;literal para AX\n",val);
		CONCAT_BUF((char *)"\tMOV DS:[0%Xh], AX \t\t\t\t\t\t\t;ax para memoria\n",fator->endereco);
	}
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================fim gera literal===================\n");
}

void fatorGeraId(struct Fator *fator, char *id)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"fatorGeraId");

	struct Celula *registro = pesquisarRegistro(id);

	fator->endereco = registro->simbolo.memoria;
	fator->tipo = registro->simbolo.tipo;
	fator->tamanho = registro->simbolo.tamanho;
}

void fatorGeraArray(struct Fator *fator, struct Fator *expr, char *id)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"fatorGeraArray");

	fator->endereco = novoTemp((expr->tipo == TP_Integer) ? TAM_INT : TAM_CHA);

	fator->tamanho = 1;

	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================acessar array===================\n");
	
	CONCAT_BUF((char *)"\tMOV BX, DS:[0%Xh] \t\t\t\t\t\t;move para BX o endereco da expressao que vai conter o indice \n", expr->endereco);


	if (fator->tipo == TP_Integer)
		CONCAT_BUF((char *)"\tADD BX, DS:[0%Xh] \t\t\t\t\t\t\t\t;int usa 2 bytes, portanto contamos a posicao\n", expr->endereco);


	CONCAT_BUF((char *)"\tADD BX, 0%Xh \t\t\t\t\t\t\t\t;soma o endereco do id indice + posicao = endereco real \n", pesquisarRegistro(id)->simbolo.memoria);


	CONCAT_BUF((char *)"\tMOV BX, DS:[BX] \t\t\t\t\t\t;move para um registrador o valor na posicao de memoria calculada \n", fator->endereco);


	CONCAT_BUF((char *)"\tMOV DS:[0%Xh], BX \t\t\t\t\t\t;move o que estiver no endereco real para o temporario \n", fator->endereco);
}

void fatorGeraExp(struct Fator *fator, struct Fator *expr)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"fatorGeraExp");

	fator->endereco = expr->endereco;
	fator->tamanho = expr->tamanho;
	fator->tipo = expr->tipo;
}

void fatorGeraNot(struct Fator *pai, struct Fator *filho)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"fatorGeraNot");

	int tam = (tipoLex == TP_Integer || tipoLex == TP_Logico) ? TAM_INT : TAM_CHA;

	pai->endereco = novoTemp(tam);
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================nega expressao===================\n");
	CONCAT_BUF((char *)"\tMOV AX, DS:[0%Xh]\n", filho->endereco);
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================expressao not simulada com expressoes aritmeticas===================\n");
	CONCAT_BUF((char *)"\tneg AX\n");
	CONCAT_BUF((char *)"\tADD AX, 1\n");
	CONCAT_BUF((char *)"\tMOV DS:[0%Xh], AX \t\t\t\t\t\t;move o resultado para o endereco de memoria\n",pai->endereco);
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================fim nega expressao===================\n");
}

void fatorGeraMenos(struct Fator *pai, struct Fator *filho)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"fatorGeraMenos");

	pai->endereco = novoTemp(TAM_INT);
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== - (exp)===================\n");
	CONCAT_BUF((char *)"\tMOV AX, DS:[0%Xh]\n", filho->endereco);
	
	CONCAT_BUF((char *)"\tneg AX \t\t\t\t\t\t\t\t;negacao aritmetica \n");
	CONCAT_BUF((char *)"\tMOV DS:[0%Xh], AX\n",pai->endereco);
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;===================fim - (exp)===================\n");
}

/* repassa dados do filho para o pai */
void atualizaPai(struct Fator *pai, struct Fator *filho)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"atualizaPai");

	pai->endereco = filho->endereco;
	pai->tipo = filho->tipo;
	pai->tamanho = filho->tamanho;
}

/* salva o operador */
void guardaOp(struct Fator *pai)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"guardaOp");

	pai->op = tokenLex;
}

/* operacoes entre termos */
void genOpTermos(struct Fator *pai, struct Fator *filho)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN((char *)"genOpTermos");

	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== inicio operacoes===================\n");
	CONCAT_BUF((char *)"\tMOV AX, DS:[0%Xh] \t\t\t\t\t\t;operando 1 em AX\n",pai->endereco);
	CONCAT_BUF((char *)"\tMOV BX, DS:[0%Xh] \t\t\t\t\t\t\t;operando 2 em BX\n",filho->endereco);

	char *op;        /* codigo assembly da operacao */
	char *RD = (char *)"AX"; /* registrador Destino         */
	char *RO = (char *)"BX"; /* registrador origem          */
	char *RR = (char *)"AX"; /* registrador origem          */

	switch (pai->op) {
		case And:  /* fallthrough */
					CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== And===================\n");
		case Vezes:
					op = (char *)"IMUL";
					aritmeticos(op,RD,RO,RR,pai);
					break;

		case Barra: op = (char *)"IDIV";
					aritmeticos(op,RD,RO,RR,pai);
					break;

		case Porcento: 
					op = (char *)"IDIV";
					RR = (char *)"DX";
					aritmeticos(op,RD,RO,RR,pai);
					break;

		case Or:    /* fallthrough */
					CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== Or===================\n");
		case Mais: 
					op = (char *)"ADD";
					aritmeticos(op, RD, RO,RR, pai);
					break;

		case Menos:
					op = (char *)"SUB";
					aritmeticos(op, RD, RO,RR, pai);
					break;

		case Igual:
					CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== igual===================\n");
					op = (char *)"JE";
					if (pai->tipo == TP_Char)
						compChar(pai);
					else
						comp(op, pai);

					break;

		case Diferente:
					CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== Diferente===================\n");
					op = (char *)"JNE";
					comp(op, pai);
					break;

		case Maior: 
					CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== Maior===================\n");
					op = (char *)"JG";
					comp(op, pai);
					break;

		case Menor:
					CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== Menor===================\n");
					op = (char *)"JL";
					comp(op, pai);
					break;

		case MaiorIgual:
					CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== MaiorIgual===================\n");
					op = (char *)"JGE";
					comp(op, pai);
					break;

		case MenorIgual:
					CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== MenorIgual===================\n");
					op = (char *)"JLE";
					comp(op, pai);
					break;

	}
	CONCAT_BUF((char *)"\t\t\t\t\t\t\t\t\t\t\t\t;=================== fim operacao===================\n");

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

	
/* atribui posicao de acesso ao vetor no registro lexico */
void atrPos(int pos)
{
	posLex = pos;
}

/* atribui tipo à constante na tabela de simbolos */
void atrTipo()
{
	enderecoLex->simbolo.tipo = tipoLex;
}

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

	if (esperado != tokenLex){
		if (lex) erroSintatico(ER_SIN);
		else erroSintatico(ER_SIN_EOF);
	}

	lexan();
	return retorno;
}

/* Trata um Erro Sintático
 * e aborta o programa
 */
void erroSintatico(int tipo)
{

	erro = tipo;
	switch (tipo)
	{
		case ER_SIN:
			erroMsg = (char *)"token nao esperado";
			break;
		case ER_SIN_EOF:
			erroMsg = (char *)"fim de arquivo nao esperado";
			break;
		case ER_SIN_NDEC:
			erroMsg = (char *)"identificador nao declarado";
			break;
		case ER_SIN_JADEC:
			erroMsg = (char *)"identificador ja declarado";
			break;
		case ER_SIN_TAMVET:
			erroMsg = (char *)"tamanho do vetor excede o maximo permitido";
			break;
		case ER_SIN_C_INC:
			erroMsg = (char *)"classe de identificador incompativel";
			break;
		case ER_SIN_T_INC :
			erroMsg = (char *)"tipos incompativeis";
			break;
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

	/* codegen: inicia bloco de declaracoes */
	initDeclaracao();

	/* inicia pelo primeiro simbolo da gramatica */
	declaracao();
}

/* Declaracao de variáveis ou constantes 
 * Var variavel();
 * Const constante();
 */
void declaracao(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"declaracao");

	/* var ou const */
	if (tokenLex == Var) {

		lido=0;
		lexan();
		variavel();
		declaracao();

	} else if (tokenLex == Const) {

		lido=0;
		lexan();
		constante();
		declaracao();

	} else {
		/* existem casos especificos onde o
		 * token do bloco de comandos ja foi lido
		 * e portanto nao precisa ser lido aqui,
		 * conferir listaIds para ver a lista desses
		 * casos 
		 *
		 * se ainda nao leu, le
		 * se ja leu, utiliza o lexema lido
		 * e marca que nao leu
		 * */
		if (!lido) lexan();
		else lido = 0;

		/* codegen: finaliza bloco de declaracoes e 
		 * inicializa bloco do programa 
		 */
		fimDecInitCom();

		blocoComandos();
		fimDeArquivo();
	}
}


/* Bloco de comandos
 * For, If, ID= , ;, readln();, write();, writeln();
 */
void blocoComandos()
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"blocoComandos");

	switch(tokenLex)
	{
		case Identificador:
			/* acao semantica */
			verificaDeclaracao(lexemaLex);
			verificaConst(lexemaLex);

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
			escrita(0);
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case Writeln:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			escrita(1);
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case F_Chaves:
			/* encontrou o fim do bloco de comandos atual,
			 * retorna e deixa o metodo que chamou tratar o '}'
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
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"fimDeArquivo");


	/* se lex nao for 0 ainda n leu o EOF */
	/* leu fim de arquivo mas nao em estado de aceitacao */
	if (lex)
		erroSintatico(ER_SIN);

	if (estado_sin != ACEITACAO_SIN)
		erroSintatico(ER_SIN_EOF);

	/* codegen: finaliza o programa */
	fimComandos();

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
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"constante");

	/* salva o lexema atual e o tipo para verificacao da classe */
	lexAux = lexemaLex;
	Tipo t = tipoLex;

	int negativo = 0;

	defClasse(CL_Const);

	estado_sin = N_ACEITACAO_SIN;
	casaToken(Identificador);

	/* Ação semantica */
	verificaClasse(lexAux, t);

	casaToken(Igual);

	/* literal negativo */
	if (tokenLex == Menos) {
		negativo = 1;
		lexan(); 
	}

	/* atribui tipo a constante */
	atrTipo();

	lexAux = removeComentario(lexemaLido);
	casaToken(Literal);

	/* codegen */
	genDeclaracao(tipoLex,
			classeLex,
			tamanhoLex,
			lexAux,
			negativo
			);

	casaToken(PtVirgula); lido = 1;
}

/* var char|integer listaIds();
*/
void variavel(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"variavel");

	/* Ação semantica */
	defClasse(CL_Var);

	estado_sin = N_ACEITACAO_SIN;
	if (tokenLex == Char || tokenLex == Integer) {
		if (tokenLex == Char) tipoLex = TP_Char;
		else tipoLex = TP_Integer;

		lexan();
		listaIds(tipoLex);
	} else {
		erroSintatico(ER_SIN);
	}
	estado_sin = ACEITACAO_SIN;
}


/* id;
 * id,id,...;
 * id=literal,...|;
 * id[int],...|;
 */
void listaIds(Tipo ultimoTipo)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"listaIds");

	Tipo t;
	int negativo = 0;

	/* acao semantica */
	verificaClasse(lexemaLex, ultimoTipo);

	casaToken(Identificador);
	if (tokenLex == Virgula){
		/* Lendo id,id */

		/* codegen */
		genDeclaracao(
				enderecoLex->simbolo.tipo,
				classeLex,
				enderecoLex->simbolo.tamanho,
				NULL,
				negativo
				);

		lexan();
		listaIds(ultimoTipo);

	} else if (tokenLex == PtVirgula) {
		/* lendo fim de um comando */

		/* codegen */
		genDeclaracao(
				enderecoLex->simbolo.tipo,
				classeLex,
				enderecoLex->simbolo.tamanho,
				NULL,
				negativo
				);

		lexan();
		/* Lista de declaracoes tipo Var integer c; char d; */
		if (tokenLex == Integer || tokenLex == Char)
			variavel();
		else
			/* fim do comando e marca lido como 1
			 * pois leu um lexema que nao
			 * foi utilizado aqui, portanto
			 * o proximo metodo nao precisa ler
			 * este lexema
			 * */
			lido = 1;

	} else if (tokenLex == Igual) {
		/* lendo id=literal */
		lexan();

		/* literal negativo */
		if (tokenLex == Menos) {
			negativo = 1;
			lexan();
		}

		t = tipoLex;
		lexAux = lexemaLex;

		casaToken(Literal);

		/* acao semantica */
		verificaTipo(enderecoLex->simbolo.tipo, t);

		/* codegen */
		genDeclaracao(
				t,
				classeLex,
				enderecoLex->simbolo.tamanho,
				lexAux,
				negativo
				);

		if (tokenLex == Virgula) {
			/* outro id */
			lexan();
			listaIds(ultimoTipo);
		} else {
			/* terminou de ler o comando */
			casaToken(PtVirgula);

			/* Lista de declaracoes tipo Var integer c; char d; */
			if (tokenLex == Integer || tokenLex == Char)
				variavel();
			else
				/* fim do comando e marca lido como 1
				 * pois leu um lexema que nao
				 * foi utilizado aqui, portanto
				 * o proximo metodo nao precisa ler
				 * este lexema
				 * */
				lido = 1;
		}
	} else {
		/* lendo id[literal] */
		casaToken(A_Colchete);

		lexAux = lexemaLex;
		casaToken(Literal);

		/* acao semantica */
		verificaTam(str2int(lexAux));

		casaToken(F_Colchete);

		/* codegen */
		genDeclaracao(
				enderecoLex->simbolo.tipo,
				classeLex,
				enderecoLex->simbolo.tamanho,
				NULL,
				negativo
				);


		if (tokenLex == Virgula) {
			/* outro id */
			lexan();
			listaIds(ultimoTipo);
		} else {
			/* terminou de ler o comando */
			casaToken(PtVirgula);

			/* Lista de declaracoes tipo Var integer c; char d; */
			if (tokenLex == Integer || tokenLex == Char)
				variavel();
			else
				/* fim do comando e marca lido como 1
				 * pois leu um lexema que nao
				 * foi utilizado aqui, portanto
				 * o proximo metodo nao precisa ler
				 * este lexema
				 * */
				lido = 1;
		}
	}
}

/***********************************************
 *
 *  Procedimentos de Bloco de comandos
 *
 ***********************************************/


/* Atribuicao
 * ID=expressao();
 */
void atribuicao(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"atribuicao");

	NOVO_FATOR(pai);
	NOVO_FATOR(expr);  /* fator da expressao do lado direito */
	Tipo tipoId = enderecoLex->simbolo.tipo;
	pai->tipo = tipoId;

	/* codegen
	 * salva no pai o endereco de id
	 * para caso leia outro id na expressao de atribuicao
	 *
	 * zera os temporarios
	 */
	pai->endereco = enderecoLex->simbolo.memoria;
	zeraTemp();

	/* lendo array: id[expressao()] */
	if (tokenLex == A_Colchete) {
		lexan();

		NOVO_FATOR(aux);

		aux = expressao();
		/* acao semantica */
		verificaTipo(aux->tipo, TP_Integer);

		/* marca como array */
		atrPos(1);
		casaToken(F_Colchete);
		fatorGeraArray(pai, aux, enderecoLex->simbolo.lexema);
	}

	casaToken(Igual);


	expr = expressao();

	/* acao semantica */
	verificaTipo(expr->tipo, tipoId);
	verificaAtrVetor();

	/* codegen */
	genAtribuicao(pai,expr);

	casaToken(PtVirgula);
} 

/* Repeticao
 * ID = literal to literal repeticao1();
 */
void repeticao(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"repeticao");

	Tipo t = enderecoLex->simbolo.tipo;
	NOVO_FATOR(pai);
	NOVO_FATOR(filho);
	NOVO_FATOR(filho2);

	lexAux = lexemaLex;

	/* codegen
	 * salva o endereco do id no pai
	 * para caso leia outro id na expressao de atribuicao
	 * e zera temporarios
	 */
	pai->endereco = enderecoLex->simbolo.memoria;
	pai->tipo = t;
	zeraTemp();

	/* acao semantica */
	verificaDeclaracao(lexAux);
	verificaConst(lexAux);
	verificaTipo(t,TP_Integer);

	casaToken(Identificador);

	/* lendo array: id[i] */
	if (tokenLex == A_Colchete) {
		lexan();

		NOVO_FATOR(aux);
		aux = expressao();

		/* acao semantica */
		verificaTipo(aux->tipo,TP_Integer);

		/* codegen */
		acessoArray(pai, aux);

		casaToken(F_Colchete);
	}

	/* ja leu ( id|id[i] ) e pode fechar o comando */
	casaToken(Igual);

	filho = expressao();

	/* codegen */
	genAtribuicao(pai, filho);

	/* acao semantica */
	verificaTipo(filho->tipo,TP_Integer);

	casaToken(To);

	filho2 = expressao();

	/* codegen */
	rot inicio = novoRot();
	rot fim = novoRot();
	genRepeticao(pai,filho2,inicio,fim);

	repeticao1(pai, inicio, fim);

}

/* 
 * step literal do comandos2();
 * ou
 * do comandos2();
 */
void repeticao1(struct Fator *pai, rot inicio, rot fim)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"repeticao1");

	Tipo t;
	char *step = (char *)"1";

	if (tokenLex == Step) {
		lexan();

		/* guarda o step */
		t = tipoLex;
		step = lexemaLex;

		casaToken(Literal);

		/* acao semantica */
		verificaTipo(t, TP_Integer);

	}

	casaToken(Do);
	comandos2();

	/* codegen */
	genFimRepeticao(pai, inicio, fim, step);

}

/* R1 na gramatica
 * { blocoComandos() }
 * ou
 * comando unico
 */
void comandos2(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"comandos2");

	switch(tokenLex)
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
			escrita(0);
			break;

		case Writeln:
			lexan();
			escrita(1);
			break;

		case A_Chaves:
			lexan();
			blocoComandos();
			/* o } ja foi lido por alguem na chamada antiga chamou */
			casaToken(F_Chaves);
			break;

	case F_Chaves:
			/* encontrou o fim do bloco de comandos atual,
			 * retorna e deixa o metodo que chamou tratar o }
			 */
	return;

	default:
	if (lex) erroSintatico(ER_SIN);
	else erroSintatico(ER_SIN_EOF);
	}
}

/* Teste
 * expressao() then comandos2() teste1()
 */
void teste(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"teste");

	NOVO_FATOR(expr);
	rot falso = novoRot();
	rot fim = novoRot();

	/* codegen */
	zeraTemp();

	expr = expressao();

	/* acao semantica */
	verificaTipo(expr->tipo, TP_Logico);

	/* codegen */
	genTeste(expr, falso, fim);

	/* then foi lido antes de retornar de expressao() */
	casaToken(Then);

	comandos2();

	if (tokenLex == F_Chaves)
		lexan(); 

	teste1(falso, fim);
}

/* else comandos2()
 * ou fim do if
 * blocoComandos()
 */
void teste1(rot falso, rot fim)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"teste1");

	genElseTeste(falso, fim);

	if (tokenLex == Else) {
		lexan();
		comandos2();
	}

	genFimTeste(fim);
}

/* Comando de leitura
 * readln(id)
 */
void leitura(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"leitura");
	NOVO_FATOR(pai);
	NOVO_FATOR(expr);
	char *lexId;

	casaToken(A_Parenteses);

	lexId = lexemaLex;
	casaToken(Identificador);

	/* acao semantica */
	verificaConst(lexId);

	/* codegen */
	pai->endereco = pesquisarRegistro(lexId)->simbolo.memoria;
	pai->tipo = buscaTipo(lexId);
	zeraTemp();

	if (tokenLex == A_Colchete) {
		/* lendo array: id[i] */
		lexan();
		expr = expressao();
		casaToken(F_Colchete);

		/* acao semantica */
		verificaTipo(expr->tipo, TP_Integer);
		fatorGeraArray(pai, expr, lexId);
	}

	/* ja leu ( id|id[i] ) e pode fechar o comando */
	casaToken(F_Parenteses);
	casaToken(PtVirgula);

	genEntrada(pai);

}

/* Comando nulo
 * ;
 */
void nulo(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"nulo");

	casaToken(PtVirgula);
}

/* Comando de escrita
 * write(id|const)
 */
void escrita(int ln)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"escrita");

	zeraTemp();
	casaToken(A_Parenteses);
	expressao2(ln);
	casaToken(F_Parenteses);
	casaToken(PtVirgula);
}

/* le uma expressao e retorna o tipo final
 * Expressão
 * X -> Xs [ O Xs ]
 * O  -> = | <> | < | > | >= | <=
 */
struct Fator *expressao(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"expressao");

	NOVO_FATOR(ret);
	NOVO_FATOR(filho);

	filho = expressaoS();
	
	/* codegen */
	atualizaPai(ret,filho);

	/* operacoes tipo x tipo -> logico */
	if (tokenLex == Igual || tokenLex == Diferente) {

		/* codegen */
		guardaOp(ret);

		lexan();

		NOVO_FATOR(filho2);
		filho2 = expressaoS();

		/* acao semantica */
		verificaTipo(ret->tipo, filho2->tipo);

		/* codegen */
		genOpTermos(ret,filho2);

		ret->tipo = TP_Logico;
	}

	/* operacoes tipo int x int -> logico */
	else if (tokenLex == Menor || tokenLex == Maior ||
			tokenLex == MaiorIgual || tokenLex == MenorIgual) 
	{

		/* codegen */
		guardaOp(ret);

		lexan();

		NOVO_FATOR(filho2);
		filho2 = expressaoS();

		/* acao semantica */
		verificaTipo(ret->tipo,TP_Integer);
		verificaTipo(filho2->tipo,TP_Integer);

		/* codegen */
		genOpTermos(ret,filho2);

		ret->tipo = TP_Logico;
	}


	return ret;
}

/* Expressao simples
 * Xs -> [-] T {( + | - | ‘or’) T}
 */
struct Fator *expressaoS(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"expressaoS");

	int menos = 0;
	int notOp = 0;
	NOVO_FATOR(ret);
	NOVO_FATOR(filho);

	if (tokenLex == Menos) {
		menos = 1;
		lexan();
	} else if (tokenLex == Not) {
		notOp = 1;
		lexan();
	}

	filho = termo();

	/* codegen */
	atualizaPai(ret,filho);
	if (menos) {
		/* acao semantica */
		verificaTipo(filho->tipo, TP_Integer);
		fatorGeraMenos(ret,filho);
	} else if (notOp) {
		verificaTipo(toLogico(filho->tipo), TP_Logico);
		fatorGeraNot(ret,filho);
	}


	/* operacoes int x int -> int */
	if (tokenLex == Mais || tokenLex == Menos) {

		/* codegen */
		guardaOp(ret);

		lexan();

		NOVO_FATOR(filho2);
		filho2 = termo();

		/* acao semantica */
		verificaTipo(ret->tipo, TP_Integer);
		verificaTipo(filho2->tipo, TP_Integer);

		/* codegen */
		genOpTermos(ret,filho2);

		ret->tipo = TP_Integer;
	}
	
	/* operacoes logico x logico -> logico */
	else if (tokenLex == Or) {

		/* codegen */
		guardaOp(ret);
		lexan();

		NOVO_FATOR(filho2);
		filho2 = termo();
		
		/* acao semantica */
		verificaTipo(toLogico(ret->tipo), TP_Logico);
		verificaTipo(toLogico(filho2->tipo), TP_Logico);

		/* codegen */
		genOpTermos(ret,filho2);

		ret->tipo = TP_Logico;
	}

	return ret;
}

void acaoFilhoTermo2(struct Fator *atual, Tipo gerado)
{
		/* codegen */
		guardaOp(atual);

		lexan();

		NOVO_FATOR(filho2);
		filho2 = fator();

		/* acao semantica */
		verificaTipo(atual->tipo, gerado);
		verificaTipo(filho2->tipo,  gerado);

		/* codegen */
		genOpTermos(atual,filho2);

}

/* Termo
 * T  -> F {( * | / | % | ‘and’ ) F}
 */
struct Fator *termo(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"termo");

	NOVO_FATOR(atual);
	NOVO_FATOR(filho);

	filho = fator();

	/* codegen */
	atualizaPai(atual,filho);

	/* operacoes int x int -> int */
	if (tokenLex == Vezes ||
			 tokenLex == Barra || tokenLex == Porcento )
	{
		acaoFilhoTermo2(atual, TP_Integer);
	}
	/* operacoes logico x logico -> logico */
	else if (tokenLex == And) {
		acaoFilhoTermo2(atual, TP_Logico);
	}

	return atual;
}

/* Fator
 * F  -> ‘(‘ X ‘)’ | literal | id ['[' X ']']
 */
struct Fator *fator(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"fator");

	NOVO_FATOR(ret);

	int array = 0;
	char *lexId;

	if (tokenLex == A_Parenteses) {

		lexan();
		NOVO_FATOR(expr);
		expr = expressao();

		/* codegen */
		fatorGeraExp(ret,expr);
		
		casaToken(F_Parenteses);

	/* fator -> literal */
	} else if (tokenLex == Literal) {

		lexAux = removeComentario(lexemaLido);
		ret->tipo = tipoLex;

		/* codegen */
		fatorGeraLiteral(ret,lexAux);
		
		lexan();

	/* fator -> id */
	} else if (tokenLex == Identificador) {
		lexId = lexemaLex;

		/* acao semantica */
		verificaDeclaracao(lexId);

		lexan();

		NOVO_FATOR(aux);
		/* lendo array: id[expressao()] */
		if (tokenLex == A_Colchete) {
			lexan();

			aux = expressao();

			/* acao semantica */
			verificaTipo(aux->tipo, TP_Integer);

			/* marca como array */
			atrPos(1); array = 1;

			casaToken(F_Colchete);
		}

		/* codegen */
		if (array)
			fatorGeraArray(ret,aux,lexId);
		else
			fatorGeraId(ret,lexId);

		ret->tipo = pesquisarRegistro(lexId)->simbolo.tipo;
	}

	return ret;
}


/* lista de expressoes */
void expressao2(int ln)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN((char *)"expressao2");
	NOVO_FATOR(expr);

	expr = expressao();
	genSaida(expr, ln);

	if (tokenLex == Virgula) {
		lexan();
		expressao2(ln);
	}

}


/* testa a inserção de um registro na tabela */
void testeInsercao(void)
{
	printf((char *)"Testando inserção simples......");
	char *lex = (char *)"teste";
	struct Celula *inserido = adicionarRegistro(lex, Identificador);
	if (inserido == NULL)
		printf((char *)"ERRO: Inserção falhou\n");
	else
		printf((char *)"OK\n");
}

/* testa a inserção de um registro que cause uma colisão na tabela */
void testeColisao(void)
{
	printf((char *)"Testando inserção com colisão..");
	char *lex = (char *)"teste2";
	struct Celula *inserido = adicionarRegistro(lex, Identificador);
	struct Celula *colisao = adicionarRegistro(lex, Identificador);
	if (inserido->prox != colisao)
		printf((char *)"ERRO: Colisão falhou\n");
	else
		printf((char *)"OK\n");
}

/* testa a busca por um registro que está no início da lista, ou seja,
 * sem colisões.
 */
void testeBuscaSimples(void)
{
	printf((char *)"Testando busca simples.........");
	char *lex = (char *)"teste3";
	struct Celula *inserido = adicionarRegistro(lex, Identificador);
	struct Celula *encontrado = pesquisarRegistro(lex);
	if (encontrado != inserido)
		printf((char *)"ERRO: Busca simples falhou\n");
	else
		printf((char *)"OK\n");
}

/* testa a busca por um registro que está no meio da lista, ou seja,
 * quando houveram colisões.
 */
void testeBuscaEmColisao(void)
{
	printf((char *)"Testando busca em colisão......");
	/* strings que colidem e são diferentes */
	char *lex = (char *)"not";
	char *lex2 = (char *)"%";
	/*guarda somente o registro que colidiu, que é o que buscamos*/
	adicionarRegistro(lex, Identificador);
	struct Celula *inserido = adicionarRegistro(lex2, Identificador);
	struct Celula *encontrado = pesquisarRegistro(lex2);
	if (encontrado != inserido)
		printf((char *)"ERRO: Busca em colisão falhou.\n \
					  ponteiro inserido: %p\n \
				      ponteiro encontrado: %p\n", inserido, encontrado);
	else
		printf((char *)"OK\n");
}

/* testa a busca por um registro que não está na lista
 */
void testeBuscaVazia(void)
{
	printf((char *)"Testando busca vazia...........");
	/* strings que colidem e são diferentes */
	char *lex = (char *)"esse lexema não está na tabela";
	struct Celula *encontrado = pesquisarRegistro(lex);
	if (encontrado != NULL)
		printf((char *)"ERRO: Busca em colisão falhou.\n");
	else
		printf((char *)"OK\n");
}

/* roda todos os testes da tabela de simbolos */
void testesTabelaSimbolos(void)
{
	testeInsercao();
	testeColisao();
	testeBuscaSimples();
	testeBuscaEmColisao();
	testeBuscaVazia();
	limparTabela();
}

/* testa o analisador lexico lendo o arquivo todo
 * e printando todos os lexemas encontrados
 */
void testeLexan(void)
{
	while(lex){
		lexan();
		printf((char *)"atual: %s\n",lexemaLex);
	}
}

/* DEFINIÇÃO DE FUNÇÕES */

/*
 * pára o programa e reporta o erro
 */
void abortar(void)
{
#ifdef DEBUG_SIN
	//printPilha(pilha);
#endif
#ifdef DEBUG_TS
	mostrarTabelaSimbolos();
#endif

	/* remove o arquivo pois o codigo gerado eh invalido */
	remove(asmNome);

	switch(erro) {
		case ER_LEX:
			printf("%d\n%s [%c].\n", linha+1, erroMsg, letra);
			break;

		case ER_SIN:            /* Fallthrough */
		case ER_LEX_N_ID:       /* Fallthrough */
		case ER_SIN_NDEC:       /* Fallthrough */
		case ER_SIN_JADEC:      /* Fallthrough */
		case ER_SIN_C_INC:
			printf("%d\n%s [%s].\n", linha+1, erroMsg, removeBranco(removeComentario(lexemaLido)));
			break;

		case ER_LEX_INVD:        /* Fallthrough */
		case ER_LEX_EOF:         /* Fallthrough */
		case ER_SIN_EOF:         /* Fallthrough */
		case ER_SIN_TAMVET:      /* Fallthrough */
		case ER_SIN_T_INC:
			printf("%d\n%s.\n", linha+1, erroMsg);
			break;
	}
	exit(erro);
}

/* escreve o que estiver no buffer em progAsm e
 * reporta linhas compiladas
 */
void sucesso(void)
{
	flush();
	printf("%d linhas compiladas.\n", linha-1);
}


int main(int argc, char *argv[])
{

	iniciarCodegen();
	inicializarTabela();
	iniciarAnSin();

	return SUCESSO;
}
