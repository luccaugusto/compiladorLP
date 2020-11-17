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
	
	//define DEBUG_SIN
	
	#include "types.h"
	
	extern int erro;
	extern int estado_sin;
	extern int lex;
	extern int lido;
	extern char* lexAux;
	extern struct pilha_d *pilha;
	
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
	void listaIds(void);
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
#endif
