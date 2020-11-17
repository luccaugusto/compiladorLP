#ifndef _CODEGEN_H
#define _CODEGEN_H

	#define DEBUG_GEN

	#define TAM_INT 2
	#define TAM_CHA 1
	#define A_SEG_PUB CONCAT_BUF("dseg SEGMENT PUBLIC\n");
	#define F_SEG_PUB CONCAT_BUF("dseg ENDS\n");
	#define CONCAT_BUF(...) sprintf(aux, __VA_ARGS__); buf_concatenar();
	#define MAX_BUF_SIZE 2048
	#define MAX_AUX_SIZE 256

	/* Geração de código */
	void flush(void);
	void buf_concatenar(void);
	int novoTemp(int);
	int novoRot(void);
	void zeraTemp(void);
	void genAtribuicao(struct Fator *, struct Fator *);
	void genExp(struct Fator *, char*);
	void genDeclaracao(Tipo, Classe, int, char*, int);
	void genRepeticao(struct Fator *, struct Fator *, rot, rot);
	void genFimRepeticao(struct Fator *, rot, rot, char *);
	void genTeste(struct Fator *, rot, rot);
	void genElseTeste(rot, rot);
	void genFimTeste(rot);
	void initDeclaracao(void);
	void iniciarCodegen(void);
	void fimComandos(void);
	void fimDecInitCom(void);
	void fatorGeraId(struct Fator *, char *);
	void fatorGeraLiteral(struct Fator *, char *);
	void fatorGeraExp(struct Fator *,struct Fator *);
	void fatorGeraNot(struct Fator *, struct Fator *);
	void fatorGeraArray(struct Fator *,struct Fator *, char *);
	void fatorGeraMenos(struct Fator *, struct Fator *);

	void atualizaPai(struct Fator *, struct Fator *);
	void guardaOp(struct Fator *);
	void genOpTermos(struct Fator *, struct Fator *);

	extern char *buffer;
	extern char *aux;

	extern int MD;
	extern int DS;

#endif
