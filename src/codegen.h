#ifndef __CODEGEN
#define __CODEGEN

	#define DEBUG_GEN 0 

	/* Geração de código */
	void escreve(char *);
	void fimDeclaracao(void);
	void initDeclaracao(void);
	void genDeclaracao(Tipo, Classe, int, char*);

#endif
