#ifndef _CODEGEN_H
#define _CODEGEN_H

	#define DEBUG_GEN 0 
	#define CONCAT_BUF(...) sprintf(aux, __VA_ARGS__); buf_concatenar();
	#define MAX_BUF_SIZE 2048
	#define MAX_AUX_SIZE 256

	/* Geração de código */
	void flush(void);
	void fimComandos(void);
	void initComandos(void);
	void fimDeclaracao(void);
	void initDeclaracao(void);
	void buf_concatenar(void);
	void iniciarCodegen(void);
	void genDeclaracao(Tipo, Classe, int, char*, int);

	extern char *buffer;
	extern char *aux;

	extern int MD;

#endif
