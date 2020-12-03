/* **************************************************************************
 * Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 * Este programa segue o estilo de código definido em: 
 * https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 *
 * Arquivo utils:
 * Definições de funções utilitárias do compilador.
 * 
 * *************************************************************************/

/* HEADERS */
#include "types.h"
#include "lexan.h"

/* DECLARACAO DE FUNCOES */
char lex_get_char(void);
int str2int(char *srt);
int eh_letra(char l);
int eh_digito(char l);
int eh_branco(char l);
char minusculo(char l);
char *encurtar(char *str);
char *remove_aspas(char *str);
char *remove_branco(char *str);
char *removeComentario(char *str);
Tokens identifica_token(char *lex);
char *concatenar(char *inicio, char *fim);
unsigned int hash(unsigned char *str, int mod);

/* VARIAVEIS GLOBAIS */
extern FILE *prog_fonte;
extern char *lexema_lido;


/* DEFINICAO DE FUNCOES */
/* ************************** *
              HASH
 * ************************** */
unsigned int
hash(unsigned char *str, int mod)
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
int
eh_digito(char l)
{
	/* valores de 0-9 em ascii */
	int retorno = 0;
	if (48 <= l && l <= 57)
		retorno = 1;
	return retorno;
}

int
eh_letra(char l)
{
	/* valores de a-z em ascii */
	int retorno = 0;
	if (97 <= l && l <= 122)
		retorno = 1;
	return retorno;
}

int
eh_branco(char l)
{
	/* valores de tab, line feed, <CR>, espaço em ascii */
	int retorno = 0;
	if (l == 9 || l == 10 || l == 13 || l == 32)
		retorno = 1;
	return retorno;
}

Tokens
identifica_token(char *lex)
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

/* remove brancos que nao estejam entre strings */
char *
remove_branco(char *str)
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
		if ( !eh_branco(str[i]) || concat)
			ret[c++] = str[i];

	}
	/* finaliza string */
	ret[c] = '\0';

	return (char *)realloc(ret,strlen(ret));
}

/* remove as aspas e espacos fora delas */
char *
remove_aspas(char *str)
{
	char *retorno;
	int tamstr = strlen(str);
	int dentro = 0;
	int c = 0; /* contador do retorno */
	int tipo = '"';
	retorno = (char *) malloc(tamstr-2);

	int i=0;
	/* encontra o caractere de aspas usado */
	for (; i<tamstr; ++i) {
		if (str[i] == '"') {
			tipo = '"';
			break;
		}
		if (str[i] == '\'') {
			tipo = '\'';
			break;
		}
	}

	for (; i<tamstr; ++i) {
		if (str[i] == tipo) {
			dentro = !dentro;
			continue;
		}
		if (dentro)
			retorno[c++] = str[i];
	}

	return retorno;
}

/* remove o ultimo caractere de str */
char *
encurtar(char *str)
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

char *
concatenar(char *inicio, char *fim)
{
	char *retorno;
	int tamInicio = strlen(inicio);
	int tamFim = strlen(fim);

	/* acrescenta a posicao do \0 */
	retorno = (char *) malloc(sizeof(char) * (tamInicio+tamFim+1));

	for (int i=0; i<tamInicio; ++i)
		retorno[i] = inicio[i];

	for (int i=0; i<tamFim; ++i)
		retorno[tamInicio+i] = fim[i];

	retorno[tamInicio+tamFim]='\0';


	return retorno;
}

/* Converte maiusculas para minusculas */
char
minusculo(char l)
{
	if (l >= 65 && l <=90)
		l += 32; 
	return l;
}

/* transforma uma string de numeros em um inteiro */
int
str2int(char *str)
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

/* remove comentarios do lexema_lido */
char *
removeComentario(char *str)
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
char
lex_get_char(void)
{
	int c = fgetc(prog_fonte);
	if (c == '\n') linha++;

	char *l = (char *) &c;
	lexema_lido = concatenar(lexema_lido,l);
	return (char) c;
}
