#ifndef _CODEGEN_H
#define _CODEGEN_H

	#define DEBUG_GEN 1 
	#define A_SEG_PUB CONCAT_BUF("dseg SEGMENT PUBLIC\n");
	#define F_SEG_PUB CONCAT_BUF("dseg ENDS\n");
	#define CONCAT_BUF(...) sprintf(aux, __VA_ARGS__); buf_concatenar();
	#define MAX_BUF_SIZE 2048
	#define MAX_AUX_SIZE 256
	#define NOVO_FATOR(s) struct Fator *s = (struct Fator *)malloc(sizeof(struct Fator)); \
											s->endereco = novoTemp(2); \
											s->tipo = regLex.tipo
	#define NOVO_TERMO(s) struct Termo *s = (struct Termo *)malloc(sizeof(struct Termo)); \
											s->endereco = novoTemp(2); \
											s->tipo = regLex.tipo

	/* Geração de código */
	void flush(void);
	int novoTemp(int);
	void zeraTemp(void);
	void fimComandos(void);
	void fimDecInitCom(void);
	void initDeclaracao(void);
	void buf_concatenar(void);
	void iniciarCodegen(void);
	void fatorGeraId(struct Fator *);
	void genExp(struct Fator *, char*);
	void fatorGeraConst(struct Fator *, char *);
	void fatorGeraExp(struct Fator *,struct Fator *);
	void fatorGeraNot(struct Fator *, struct Fator *);
	void genDeclaracao(Tipo, Classe, int, char*, int);
	void fatorGeraArray(struct Fator *,struct Fator *);
	void fatorGeraMenos(struct Fator *, struct Fator *);

	void acaoTermoFator1(struct Fator *);
	void acaoTermoFator2(struct Fator *);
	void acaoTermoFator3(struct Fator *);

	extern char *buffer;
	extern char *aux;

	extern int MD;
	extern int DS;

#endif
