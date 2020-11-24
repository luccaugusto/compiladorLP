#ifndef _CODEGEN_H
#define _CODEGEN_H

	//define DEBUG_GEN
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
	#define OFFSET 0x4100

	/* Geração de código */
	void flush(void);
	void buf_concatenar(void);
	int novo_temp(int);
	int novo_rot(void);
	void zera_temp(void);
	void atribuicao_string(int, int, int, int);
	void gen_atribuicao(struct Fator *, struct Fator *);
	void gen_exp(struct Fator *, char*);
	void gen_declaracao(Tipo, Classe, int, char*, int);
	void gen_repeticao(struct Fator *, struct Fator *, rot, rot);
	void gen_fim_repeticao(struct Fator *, rot, rot, char *);
	void gen_teste(struct Fator *, rot, rot);
	void gen_else_teste(rot, rot);
	void gen_fim_teste(rot);
	void init_declaracao(void);
	void iniciar_codegen(void);
	void fim_comandos(void);
	void fim_dec_init_com(void);
	void fator_gera_id(struct Fator *, char *);
	void fator_gera_literal(struct Fator *, char *);
	void fator_gera_exp(struct Fator *,struct Fator *);
	void fator_gera_not(struct Fator *, struct Fator *);
	void fator_gera_array(struct Fator *,struct Fator *, char *);
	void fator_gera_menos(struct Fator *, struct Fator *);

	void atualiza_pai(struct Fator *, struct Fator *);
	void guarda_op(struct Fator *);
	void gen_op_termos(struct Fator *, struct Fator *);

	extern char *buffer;
	extern char *aux;

	extern int MD;
	extern int DS;

#endif
