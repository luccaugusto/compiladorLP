#ifndef __CODEGEN
#define __CODEGEN

	#define DEBUG_GEN 0 
	#define CONCAT_BUF(...) sprintf(aux, __VA_ARGS__); buf_concatenar();
	#define MAX_BUF_SIZE 2048

	/* Geração de código */
	void flush(void);
	void fimDeclaracao(void);
	void initDeclaracao(void);
	void buf_concatenar(void);
	void iniciarCodegen(void);
	void genDeclaracao(Tipo, Classe, int, char*);

	extern char *aux;
	extern char *buffer;
	extern int iniciouDec;
	extern int MD;

#endif
