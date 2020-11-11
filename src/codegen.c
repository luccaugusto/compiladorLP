/* Geração de código */
#ifndef _CODEGEN
#define _CODEGEN

	#include "codegen.h"
	#include "LC.h"

	/*
	 * TODO
	 * aumentar MD com o numero de bytes da variavel
	 */

	char *buffer;       /* buffer de criacao do codigo asm     */
	char *aux;          /* buffer auxiliar para criacao do asm */
	int MD = 0x4000;    /* memoria de dados em hexadecimal     */

	/* inicia o buffer */
	void iniciarCodegen()
	{
		buffer = malloc(sizeof(char) * MAX_BUF_SIZE);
		aux = malloc(sizeof(char) * MAX_AUX_SIZE);

		/* Pilha */
		CONCAT_BUF("sseg SEGMENT STACK\t\t\t;inicio seg. pilha");
		CONCAT_BUF("byte %Xh DUP(?)\t\t\t;dimensiona pilha",MD);
		CONCAT_BUF("sseg ENDS\t\t\t\t\t;fim seg. pilha");

	}

	/* concatena garantindo que o ultimo caractere eh o \n */
	void buf_concatenar()
	{

		int buf_size = strlen(buffer);

		/* impede strings invalidas */
		if (strlen(aux) < 4) return;

		/* se o buffer vai encher, escreve no arquivo e esvazia */
		if ((buf_size+strlen(aux)) >= MAX_BUF_SIZE)
			flush();

		buffer = concatenar(buffer, aux);
		buf_size = strlen(buffer);

		/* verifica se buffer termina em \n se nao estiver vazio*/
		if (buf_size > 1 && buffer[buf_size-1] != '\n')
			buffer = concatenar(buffer,"\n");

		aux[0] = '\0';
	}

	/* escreve buffer no arquivo progAsm 
	 * e em seguida limpa buffer
	 */
	void flush()
	{
		if (DEBUG_GEN) printf("CODEGEN: flush\n");
		fprintf(progAsm, "%s",buffer);

		/* limpa o buffer */
		buffer[0] = '\0';
	}

	/* inicia o bloco de declaracoes asm */
	void initDeclaracao(void)
	{
		if (DEBUG_GEN) printf("CODEGEN: initDeclaracao\n");
		/* dados */
		CONCAT_BUF("dseg SEGMENT PUBLIC\t\t\t;inicio seg. dados");
		CONCAT_BUF("byte %Xh DUP(?)\t\t\t;temporarios",MD);
	}

	/* finaliza o bloco de declaracoes asm */
	void fimDeclaracao(void)
	{
		if (DEBUG_GEN) printf("CODEGEN: fimDeclaracao\n");

		/* fim declaracao */
		CONCAT_BUF("dseg ENDS\t\t\t\t\t;fim seg. dados");

	}

	void initComandos(void)
	{
		if (DEBUG_GEN) printf("CODEGEN: initComandos\n");
	
		/* comandos */
		CONCAT_BUF("cseg SEGMENT PUBLIC\t\t\t;inicio seg. codigo");
		CONCAT_BUF("     ASSUME CS:cseg, DS: dseg");
		CONCAT_BUF("strt:\t\t\t\t\t\t;inicio do programa");
		CONCAT_BUF("    ;comandos ");
	}

	void fimComandos(void)
	{
		if (DEBUG_GEN) printf("CODEGEN: fimComandos\n");

		CONCAT_BUF("cseg ENDS\t\t\t\t\t;fim seg. codigo");
		CONCAT_BUF("END strt\t\t\t\t\t;fim programa");
	
	}

	/* gera o asm da declaracao de uma variavel ou constante 
	 * e retorna o endereco que foi alocado 
	 */
	void genDeclaracao(Tipo t, Classe c, int tam, char *val, int negativo)
	{
		char *tipo;  /* string de tipo        */
		char *classe;/* const ou var          */
		char *nome;  /* string sword ou byte  */
		char *valor; /* string de valor ou ?  */
		int n_bytes; /* numero de bytes usado */

		/* marca o endereco de memoria na tabela de simbolos */
		tokenAtual.endereco->simbolo.memoria = MD;

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

		if (c== CL_Const)
			classe = "const";
		else
			classe = "var";

		/* string de valor se existir */
		if (val != NULL) {
			valor = val;
			if (negativo)
				valor = concatenar("-",valor);
		} else {
			valor = "?";
		}

		/* arrays */
		if (tam > 1) {
			CONCAT_BUF("%s %d DUP(?)\t\t\t;var. Vet %s. em %Xh", nome, tam, tipo, MD);
		 }else {
			CONCAT_BUF("%s %s\t\t\t\t\t\t; %s. %s. em %Xh", nome, valor, classe, tipo, MD);
		 }

		/* incrementa a posicao de memoria com o numero de bytes utilizado */
		MD+=n_bytes;
	}
#endif 
