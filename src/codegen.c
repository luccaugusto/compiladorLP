/* Geração de código */

#include "codegen.h"

/* escreve str no arquivo progAsm */
void escreve(char *str)
{
	if (DEBUG_GEN) printf("CODEGEN: escreve\n");
	fprintf(progAsm, "%s\n",str);
}

/* inicia o bloco de declaracoes asm */
void initDeclaracao(void)
{
	if (DEBUG_GEN) printf("CODEGEN: initDeclaracao\n");
	escreve("dseg SEGMENT PUBLIC\t\t;inicio seg. dados");
}

/* finaliza o bloco de declaracoes asm */
void fimDeclaracao(void)
{
	if (DEBUG_GEN) printf("CODEGEN: fimDeclaracao\n");
	escreve("dseg ENDS\t\t\t\t;fim seg. dados");
}

/* gera o asm da declaracao de uma variavel ou constante */
void genDeclaracao(Tipo t, Classe classe, int tam, char *val)
{
	char *tipo;

	if (t == TP_Integer) {
		tipo = "int";
	} else if (t == TP_Char) {
		if (tam == 0)
			tipo = "caract";
		else
			tipo = "stirng";
	} else {
		tipo ="logic";
	}

	/* constantes */
	if (classe == CL_Const) {
		escreve("sword %s\t\t\t; %s. em %dh",val, tipo,md);
	} else {

	}
}
