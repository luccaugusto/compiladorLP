#ifndef __CODEGEN
#define __CODEGEN

	#define DEBUG_GEN 0 
	#define CONCAT_BUF sprintf(aux,
	#define END_CONCAT_BUF ); buf_concatenar();
	#define ESCREVE escreve(buffer)

	/* Geração de código */
	void escreve(char *);
	void fimDeclaracao(void);
	void initDeclaracao(void);
	void buf_concatenar(void);
	void genDeclaracao(Tipo, Classe, int, char*);

#endif
