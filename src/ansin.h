#ifndef _ANSIN_H
#define _ANSIN_H

	#define ER_SIN -20
	#define ER_SIN_EOF -21
	#define ER_SIN_NDEC -22
	#define ER_SIN_JADEC -23
	#define ER_SIN_TAMVET -24
	#define ER_SIN_C_INC -25
	#define ER_SIN_T_INC -26
	
	#define N_ACEITACAO_SIN -31
	#define ACEITACAO_SIN 32
	
	#define DEBUG_SIN 0
	
	#include "types.h"
	
	extern int erro;
	extern int estado_sin;
	extern int lex;
	extern int lido;
	extern struct pilha_d *pilha;
	
	/* manipulacao do registro lexico */
	void atrPos(int);
	
	/* Analisador Sintatico */
	void nulo(void);
	void teste(void);
	void ansin(void);
	void teste1(void);
	void leitura(void);
	void escrita(void);
	void variavel(void);
	void listaIds(void);
	Tipo expressao(void);
	void escritaLn(void);
	void constante(void);
	void comandos2(void);
	void repeticao(void);
	Tipo expressao1(Tipo);
	void expressao2(void);
	void expressao3(void);
	void repeticao1(void);
	void declaracao(void);
	void atribuicao(void);
	void blocoComandos(void);
	void iniciarAnSin(void);
	void fimDeArquivo(void);
	void erroSintatico(int);
	int casaToken(Tokens);
#endif
