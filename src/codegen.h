/* **************************************************************************
 * Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 * Este programa segue o estilo de código definido em: 
 * https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 *
 * Arquivo codegen:
 * Este arquivo contém funções responsáveis pela geração de código.
 * Existem funções que gerenciam variáveis de contadores de memória 
 * e rótulos e funções que de fato geram código assembly.
 *
 * *************************************************************************/

#ifndef _CODEGEN_H
#define _CODEGEN_H

	/* MACROS */
	/*#define DEBUG_GEN*/
	#ifdef DEBUG_GEN
		#define DEBUGGEN(s) printf("GEN: %s\n",s); push(s,pilha);
	#else
		#define DEBUGGEN(s)
	#endif
	#define TAM_INT 2
	#define TAM_CHA 1
	#define A_SEG_PUB CONCAT_BUF("dseg SEGMENT PUBLIC\n");
	#define F_SEG_PUB CONCAT_BUF("dseg ENDS\n");
	#define CONCAT_BUF(...) sprintf(aux, __VA_ARGS__); buf_concatenar();
	#define MAX_BUF_SIZE 1024
	#define MAX_AUX_SIZE 256
	#define CD_INICIAL 0x4000
	#define TP_INICIAL OFFSET
	#define OFFSET 0x4100

	/* DECLARACAO DE FUNCOES */
	/* Geração de código */
	int novo_temp(int t);
	int novo_rot(void);
	void zera_temp(void);
	void aritmeticos(char* op, char *RD, char *RO, char *RR, struct Expr *pai);
	void comp(char *op, struct Expr *pai);
	void comp_char(struct Expr *pai);
	void prox_linha(void);
	void iniciar_codegen(void);
	void buf_concatenar(void);
	void flush(void);
	void init_declaracao(void);
	void fim_dec_init_com(void);
	void fim_comandos(void);
	void acesso_array(struct Expr *pai, struct Expr *filho);
	void gen_declaracao(Tipo t, Classe c, int tam, char *val, int negativo);
	void atribuicao_string(int end1, int end2, int tamMax);
	void gen_atribuicao(struct Expr *pai, struct Expr *fator);
	void gen_repeticao(struct Expr *pai, struct Expr *filho, rot inicio, rot fim);
	void gen_fim_repeticao(struct Expr *pai, rot inicio, rot fim,struct Expr *step);
	void gen_teste(struct Expr *filho, rot falso, rot fim);
	void gen_else_teste(rot falso, rot fim);
	void gen_fim_teste(rot fim);
	void gen_entrada(struct Expr *pai);
	void gen_saida(struct Expr *pai, int ln);
	void fator_gera_literal(struct Expr *fator, char *val);
	void fator_gera_id(struct Expr *fator, char *id);
	void fator_gera_array(struct Expr *fator, struct Expr *expr, char *id);
	void fator_gera_exp(struct Expr *fator, struct Expr *expr);
	void fator_gera_not(struct Expr *pai, struct Expr *filho);
	void fator_gera_menos(struct Expr *pai, struct Expr *filho);
	void atualiza_pai(struct Expr *pai, struct Expr *filho);
	void guarda_op(struct Expr *pai);
	void gen_op_termos(struct Expr *pai, struct Expr *filho);

#endif
