/* Definicao das funcoes utilitárias do projeto */
#ifndef _UTILS
#define _UTILS
	#include "types.h"
	#include "lexan.h"

	char getChar(void);
	int str2int(char*);
	int ehLetra(char l);
	int ehDigito(char l);
	int ehBranco(char l);
	char minusculo(char l);
	int identificaToken(char *);
	char *concatenar(char *, char *);
	unsigned int hash(unsigned char *, int);
	
	extern FILE *progFonte;
	extern char *lexemaLido;

	
	/* ************************** *
	              HASH
	 * ************************** */
	unsigned int hash(unsigned char *str, int mod)
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
	
	int identificaToken(char *lex)
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
	
		/* acrescenta a posicao do \0 */
		retorno = (char *) malloc(tamInicio+tamFim+1);
	
		for (int i=0; i<tamInicio; ++i)
			retorno[i] = inicio[i];
	
		for (int i=0; i<tamFim; ++i)
			retorno[tamInicio+i] = fim[i];
	
		retorno[tamInicio+tamFim]='\0';
	
	
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
	char getChar(void)
	{
		int c = fgetc(progFonte);
		char *l = (char *) &c;
		if (!ehBranco(c)) lexemaLido = concatenar(lexemaLido,l);
		return (char) c;
	}
#endif
