/* Geração de código */
#ifndef _CODEGEN
#define _CODEGEN

	#include "codegen.h"
	#include "LC.h"

	/*
	 * TODO
	 * revisar sintaxe do assembly em TODOS os comandos
	 */

	char *buffer;       /* buffer de criacao do codigo asm     */
	char *aux;          /* buffer auxiliar para criacao do asm */
	int MD = 0x4000;    /* memoria de dados em hexadecimal     */
	int DS = 0x0;       /* contador de temporários             */
	int RT = 1;         /* contador de rotulos                 */

	/* declara novo temporario */
	int novoTemp(int t)
	{
		DS += t;
		return (DS-t);
	}

	int novoRot()
	{
		return RT++;
	}

	void zeraTemp(void)
	{
		DS = 0x0;
	}

	/* inicia o buffer */
	void iniciarCodegen(void)
	{ 
		/* DEBUGGER E PILHA */
		DEBUGGEN("iniciarCodegen");

		buffer = malloc(sizeof(char) * MAX_BUF_SIZE);
		aux = malloc(sizeof(char) * MAX_AUX_SIZE);

		/* Pilha */
		CONCAT_BUF("sseg SEGMENT STACK\t\t\t;inicio seg. pilha");
		CONCAT_BUF("byte %Xh DUP(?)\t\t\t;dimensiona pilha",MD);
		CONCAT_BUF("sseg ENDS\t\t\t\t\t;fim seg. pilha");

	}

	/* concatena garantindo que o ultimo caractere eh o \n */
	void buf_concatenar(void)
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
	void flush(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("flush");

		fprintf(progAsm, "%s",buffer);

		/* limpa o buffer */
		buffer[0] = '\0';
	}

	/* inicia o bloco de declaracoes asm */
	void initDeclaracao(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("initDeclaracao");

		/* dados */
		CONCAT_BUF("dseg SEGMENT PUBLIC\t\t\t;inicio seg. dados");
		CONCAT_BUF("byte %Xh DUP(?)\t\t\t;temporarios",MD);
	}

	/* finaliza o bloco de declaracoes asm 
	 * e inicia o bloco de comandos asm
	 */
	void fimDecInitCom(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fimDecInitCom");

		/* fim declaracao */
		CONCAT_BUF("dseg ENDS\t\t\t\t\t;fim seg. dados");
		/* comandos */
		CONCAT_BUF("cseg SEGMENT PUBLIC\t\t\t;inicio seg. codigo");
		CONCAT_BUF("     ASSUME CS:cseg, DS: dseg");
		CONCAT_BUF("strt:\t\t\t\t\t\t;inicio do programa");
		CONCAT_BUF("    ;comandos ");

	}

	void fimComandos(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fimComandos");

		CONCAT_BUF("cseg ENDS\t\t\t\t\t;fim seg. codigo");
		CONCAT_BUF("END strt\t\t\t\t\t;fim programa");
		CONCAT_BUF("mov ah, 4Ch");
		CONCAT_BUF("int 21h");
	}

	/* gera o asm da declaracao de uma variavel ou constante 
	 * e retorna o endereco que foi alocado 
	 */
	void genDeclaracao(Tipo t, Classe c, int tam, char *val, int negativo)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genDeclaracao");

		char *tipo;  /* string de tipo        */
		char *classe;/* const ou var          */
		char *nome;  /* string sword ou byte  */
		char *valor; /* string de valor ou ?  */
		int n_bytes; /* numero de bytes usado */

		/* marca o endereco de memoria na tabela de simbolos */
		END_ID = MD;

		/* string de tipo para o comentario */
		if (t == TP_Integer) {
			tipo = "int";
			nome = "sword";
			n_bytes = TAM_INT*tam;
		} else if (t == TP_Char) {
			if (tam == 0)
				tipo = "caract";
			else
				tipo = "string";
	
			nome = "byte";
			n_bytes = TAM_CHA*tam;
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

	void genAtribuicao(struct Fator *fator)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genAtribuicao");

		/* int e logico */
		if (toLogico(regLex.tipo) == TP_Logico) {
			CONCAT_BUF("mov AX, DS:[%d]", fator->endereco);
			CONCAT_BUF("mov %d, AX", END_ID);
		} else {
			/* string, move caractere por caractere */
			/* TODO: Fazer loop EM ASSEMBLY para carregar posicao a posicao da string para o id */
			for (int i=0; i<fator->tamanho; ++i) {
				CONCAT_BUF("mov AX, DS:[%d]", (fator->endereco + i));
				CONCAT_BUF("mov %d, AX", (END_ID + i));
			}
		}
	}

	void fatorGeraConst(struct Fator *fator, char *val)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraConst");

		int tamTipo = (regLex.tipo == TP_Integer || regLex.tipo == TP_Logico) ? TAM_INT : TAM_CHA;
		
		/* string */
		if (fator->tipo == TP_Char) {
			A_SEG_PUB
				CONCAT_BUF("byte \"%s$\"",removeAspas(val));
			F_SEG_PUB
			
			fator->endereco = MD;
			fator->tamanho = regLex.tamanho;
			fator->tipo = regLex.tipo;

			MD += fator->tamanho * tamTipo;

		/* nao string */
		} else {
			fator->endereco = novoTemp(tamTipo*tamTipo);
			CONCAT_BUF("mov AX, %s",val);
			CONCAT_BUF("mov %d, AX",fator->endereco);
		}
	}

	void fatorGeraId(struct Fator *fator)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraId");

		fator->endereco = END_ID;
		fator->tipo = regLex.endereco->simbolo.tipo;
		fator->tamanho = regLex.endereco->simbolo.tamanho;
	}

	void fatorGeraArray(struct Fator *fator, struct Fator *expr)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraArray");

		fator->endereco = novoTemp((regLex.tipo == TP_Integer) ? TAM_INT : TAM_CHA);

		fator->tamanho = 1;

		/* move para AX o endereco da expressao que vai conter o indice */
		CONCAT_BUF("mov AX, %d", expr->endereco);

		/* int usa 2 bytes, portanto contamos a posicao*2 */
		if (regLex.tipo == TP_Integer)
			CONCAT_BUF("add AX, DS:[%d]", expr->endereco);

		/* soma o endereco do id indice + posicao = endereco real */
		CONCAT_BUF("add AX, AX, %d", END_ID);

		/* move para um registrador */
		CONCAT_BUF("mov AX, DS:[AX]", fator->endereco);

		/* move o que estiver no endereco real para o temporario */
		CONCAT_BUF("mov %d, AX", fator->endereco);

	}

	void fatorGeraExp(struct Fator *fator,struct Fator *filho)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraExp");

		fator->endereco = filho->endereco;
		fator->tamanho = filho->tamanho;
		fator->tipo = filho->tipo;
	}

	void fatorGeraNot(struct Fator *pai, struct Fator *filho)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraNot");

		int tam = (regLex.tipo == TP_Integer || regLex.tipo == TP_Logico) ? TAM_INT : TAM_CHA;

		pai->endereco = novoTemp(tam);
		CONCAT_BUF("mov AX, DS:[%d]", filho->endereco);
		/* expressao not simulada com expressoes aritmeticas */
		CONCAT_BUF("neg AX");
		CONCAT_BUF("add AX, 1");
		CONCAT_BUF("mov DS:[%d], AX",pai->endereco);
	}

	void fatorGeraMenos(struct Fator *pai, struct Fator *filho)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraMenos");

		pai->endereco = novoTemp(2);
		CONCAT_BUF("mov AX, DS:[%d]", filho->endereco);
		/* negacao aritmetica, muda o sinal */
		CONCAT_BUF("neg AX");
		CONCAT_BUF("mov DS:[%d], AX",pai->endereco);
	}

	void acaoTermoFator1(struct Fator *pai)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("acaoTermoFator1");

		pai->termo->endereco = pai->endereco;
		pai->termo->tipo = pai->tipo;
		pai->termo->tamanho = pai->tamanho;
	}

	void acaoTermoFator2(struct Fator *pai)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("acaoTermoFator2");

		pai->op = regLex.token;
	}

	void acaoTermoFator3(struct Fator *pai)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("acaoTermoFator3");

		CONCAT_BUF("mov AX, %d",pai->termo->endereco);
		CONCAT_BUF("mov BX, %d",pai->endereco);

		/* TODO fazer imul e idiv certo */
		if (pai->op == Vezes || pai->op == And) {
			CONCAT_BUF("imul");
		} else if (pai->op == Barra || pai->op == Porcento) {
			CONCAT_BUF("idiv");
		}

		pai->termo->endereco = novoTemp(TAM_INT);
		CONCAT_BUF("mov AX, %d",pai->termo->endereco);
	}

#endif 
