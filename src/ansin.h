/* **************************************************************************
 * Compilador desenvolvido para a disciplina de compiladores 2020-2 PUC minas
 * Este programa segue o estilo de código definido em: 
 * https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 *
 * Arquivo ansin:
 * Implementação do analisador sintático. Quase toda função representa um
 * simbolo da gramatica, com algumas funções auxiliares. Durante a análise
 * sintática, ações semânticas e de geração de código são executadas.
 * (ver semac.c e codegen.c)
 * Portanto é a partir deste arquivo que a lógica principal do programa
 * acontece.
 *
 * *************************************************************************/
#ifndef _ANSIN_H
#define _ANSIN_H

	/* MACROS */
	//define DEBUG_SIN
	#define N_ACEITACAO_SIN -31
	#define ACEITACAO_SIN 32
	#ifdef DEBUG_SIN
		#define DEBUGSIN(s) printf("SIN: %s\n",s); push(s,pilha);
	#else
		#define DEBUGSIN(s)
	#endif
	

	/* TIPOS */
	/* ERROS */
	enum {
		ER_SIN = 20,
		ER_SIN_EOF,
		ER_SIN_NDEC,
		ER_SIN_JADEC,
		ER_SIN_TAMVET,
		ER_SIN_C_INC,
		ER_SIN_T_INC
	};
	
	/* DECLARACOES DE FUNCOES */
	void atr_pos(int pos);
	void atr_tipo();
	int casa_token(Tokens esperado);
	void erro_sintatico(int tipo);
	void iniciar_ansin(void);
	void declaracao(void);
	void bloco_comandos();
	void fim_de_arquivo(void);
	void constante(void);
	void variavel(void);
	int lista_ids(Tipo ultimoTipo);
	void atribuicao(void);
	void repeticao(void);
	void comandos2(void);
	void teste(void);
	void teste1(rot falso, rot fim);
	void leitura(void);
	void nulo(void);
	void escrita(int ln);
	struct Fator *expressao(void);
	struct Fator *expressao_s(void);
	void acaoFilhoTermo2(struct Fator *atual, Tipo gerado);
	struct Fator *termo(void);
	struct Fator *fator(void);
	void lista_expressoes(int ln);

	/* VARIAVEIS GLOBAIS */
	extern int erro;
	extern int estado_sin;
	extern int lex;
	extern int lido;
	extern char* lexAux;
	extern struct pilha_d *pilha;

#endif
