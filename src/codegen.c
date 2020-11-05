/* Geração de código */

#include "codegen.h"
#include "LC.h"

/*
 * TODO
 * concatenar strings no buffer e escrever uma vez so em cada funcao
 * aumentar MD com o numero de bytes da variavel
 */

char *buffer; /* buffer de criacao do codigo asm     */
char *aux;    /* buffer auxiliar para criacao do asm */

/* concatena garantindo que o ultimo caractere eh o \n */
void buf_concatenar()
{
	/* so verifica se inicio contem \n pois
	 * fim sera inicio ao fim da funcao
	 */
	if (buffer[strlen(buffer)-1] != '\n')
		buffer = concatenar(buffer,"\n");

	buffer = concatenar(buffer, aux);
}

/* escreve str no arquivo progAsm */
void escreve(char *str)
{
	if (DEBUG_GEN) printf("CODEGEN: escreve\n");
	fprintf(progAsm, "%s\n",buffer);
}

/* inicia o bloco de declaracoes asm */
void initDeclaracao(void)
{
	if (DEBUG_GEN) printf("CODEGEN: initDeclaracao\n");
	CONCAT_BUF
		"dseg SEGMENT PUBLIC\t\t;inicio seg. dados"
	END_CONCAT_BUF;
	
	CONCAT_BUF
		"byte %dh DUP(?)\t\t\t; temporarios",MD
	END_CONCAT_BUF;
}

/* finaliza o bloco de declaracoes asm */
void fimDeclaracao(void)
{
	if (DEBUG_GEN) printf("CODEGEN: fiMDeclaracao\n");
	CONCAT_BUF
		"dseg ENDS\t\t\t\t;fim seg. dados"
	END_CONCAT_BUF;
	ESCREVE;
}

/* gera o asm da declaracao de uma variavel ou constante */
void genDeclaracao(Tipo t, Classe classe, int tam, char *val)
{
	char *tipo;  /* string de tipo        */
	char *nome;  /* string sword ou byte  */
	char *valor; /* string de valor ou ?  */
	int n_bytes; /* numero de bytes usado */

	/* string de tipo para o comentario */
	if (t == TP_Integer) {
		tipo = "int";
		nome = "sword";
		n_bytes = 2*tam;
	} else if (t == TP_Char) {
		if (tam == 0)
			tipo = "caract";
		else
			tipo = "string";

		nome = "byte";
		n_bytes = tam;
	} else {
		tipo = "logic";
		nome = "byte";
		n_bytes = 1;
	}

	/* string de valor se existir */
	if (val != NULL)
		valor = val;
	else
		valor = "?";

	/* constantes */
	if (classe == CL_Const) {
		CONCAT_BUF
			"sword %s\t\t\t; const. %s. em %dh", val, tipo, MD
		END_CONCAT_BUF;
	} else {

		/* arrays */
		if (tam > 0)
			CONCAT_BUF 
				"%s %d DUP(?)\t\t;var. %s. em %dh", nome, tam, tipo, MD 
			END_CONCAT_BUF;
		

		CONCAT_BUF
			"%s %s\t\t\t; var. %s. em %dh", nome, valor, tipo, MD
		END_CONCAT_BUF;
	}

	/* incrementa a posicao de memoria com o numero de bytes utilizado */
	MD+=n_bytes;
}
