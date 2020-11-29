/* **************************************************************************
 * Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 * Este programa segue o estilo de código definido em: 
 * https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 *
 * Arquivo LC:
 * Arquivo principal do compilador. Lê os parâmetros da linha de comando e
 * inicia as estruturas de dados e processo de compilação.
 *
 * *************************************************************************/

#ifndef _LC_H
#define _LC_H

	/* MACROS */
	#define SUCESSO 0
	
	/* VARIÁVEIS GLOBAIS */
	/* parametros da linha de comando */
	extern FILE *prog_fonte;
	extern FILE *prog_asm;
	
	extern int not_eof;
	extern int erro;
	extern int lido;
	extern int linha; /*linha do arquivo*/
	extern int estado_sin; /* estado de aceitacao ou nao do analisador sintatico */
	
	extern char letra; /*posicao da proxima letra a ser lida no arquivo*/
	extern char *erro_msg; /*Mensagem de erro a ser exibida*/
	extern char *lexema_lido; /* lexema lido sem transformar em minusculo */
	
	extern struct pilha_d *pilha;
	extern struct registro_lex reg_lex; 
	extern struct Celula *tabela_simbolos[TAM_TBL];

	/*DECLARAÇÕES DE FUNÇÕES*/
	/* Fluxo de execução geral */
	void abortar(void);
	void sucesso(void);

#endif
