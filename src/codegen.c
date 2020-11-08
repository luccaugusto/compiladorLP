/* Geração de código */

#include "codegen.h"
#include "LC.h"

/*
 * TODO
 * aumentar MD com o numero de bytes da variavel
 */

char *aux;    /* buffer auxiliar para criacao do asm */
char *buffer; /* buffer de criacao do codigo asm     */
int iniciouDec = 0;
int MD = 4000;      /* memoria de dados */

/* inicia o buffer */
void iniciarCodegen()
{
	buffer = malloc(sizeof(char));
	aux = malloc(sizeof(char));

}

/* concatena garantindo que o ultimo caractere eh o \n */
void buf_concatenar()
{
	int buf_size = strlen(buffer);

	/* se o buffer vai encher, escreve no arquivo e esvazia */
	if ((buf_size+strlen(aux)) >= MAX_BUF_SIZE)
		flush();

	buffer = concatenar(buffer, aux);

	buf_size += strlen(aux);

	/* verifica se buffer termina em \n se nao estiver vazio*/
	if (buf_size > 1 && buffer[buf_size-1] != '\n')
		buffer = concatenar(buffer,"\n");
}

/* escreve buffer no arquivo progAsm 
 * e em seguida limpa buffer
 */
void flush()
{
	if (DEBUG_GEN) printf("CODEGEN: flush\n");
	fprintf(progAsm, "%s",buffer);

	/* limpa o buffer */
	buffer = realloc(buffer, sizeof(char));
}

/* inicia o bloco de declaracoes asm */
void initDeclaracao(void)
{
	iniciouDec = 1;
	if (DEBUG_GEN) printf("CODEGEN: initDeclaracao\n");
	
	CONCAT_BUF("dseg SEGMENT PUBLIC\t\t;inicio seg. dados");
	
	CONCAT_BUF("byte %dh DUP(?)\t\t\t;temporarios",MD);
}

/* finaliza o bloco de declaracoes asm */
void fimDeclaracao(void)
{
	if (DEBUG_GEN) printf("CODEGEN: fiMDeclaracao\n");
		CONCAT_BUF("dseg ENDS\t\t\t;fim seg. dados");
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

		CONCAT_BUF("sword %s\t\t\t; const. %s. em %dh", val, tipo, MD);

	} else {
	/* variaveis */

		/* arrays */
		if (tam > 0)
			CONCAT_BUF("%s %d DUP(?)\t\t;var. %s. em %dh", nome, tam, tipo, MD);
		

		CONCAT_BUF(aux, "%s %s\t\t\t; var. %s. em %dh", nome, valor, tipo, MD);

	}

	/* incrementa a posicao de memoria com o numero de bytes utilizado */
	MD+=n_bytes;
}
