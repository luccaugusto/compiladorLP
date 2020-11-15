#ifndef _LC_H
#define _LC_H

	/* MACROS */
	#define SUCESSO 0
	#define END_ID regLex.endereco->simbolo.memoria
	#ifdef DEBUG_SIN
		#define DEBUGSIN(s) printf("SIN: %s\n",s); push(s,pilha);
	#else
		#define DEBUGSIN(s)
	#endif
	#ifdef DEBUG_LEX
		#define DEBUGLEX(...) printf(__VA_ARGS__);
	#else
		#define DEBUGLEX(...)
	#endif
	#ifdef DEBUG_GEN
		#define DEBUGGEN(s) printf("GEN: %s\n",s); push(s,pilha);
	#else
		#define DEBUGGEN(s)
	#endif
	
	
	/* VARIÁVEIS GLOBAIS */
	
	/* parametros da linha de comando */
	extern FILE *progFonte;
	extern FILE *progAsm;
	
	extern int lex;
	extern int erro;
	extern int lido;
	extern int linha; /*linha do arquivo*/
	extern int estado_sin; /* estado de aceitacao ou nao do analisador sintatico */
	
	extern char letra; /*posicao da proxima letra a ser lida no arquivo*/
	extern char *erroMsg; /*Mensagem de erro a ser exibida*/
	extern char *lexemaLido; /* lexema lido sem transformar em minusculo */
	
	extern struct pilha_d *pilha;
	extern struct registroLex regLex; 
	extern struct Celula *tabelaSimbolos[TAM_TBL];

	extern int MD; /* memoria de dados */

	extern int rotulo; /* rotulos do asm */
	extern int temporario; /* temporarios do asm */
	
	/*DECLARAÇÕES DE FUNÇÕES*/
	/* Fluxo de execução geral */
	void abortar(void);
	void sucesso(void);

#endif
