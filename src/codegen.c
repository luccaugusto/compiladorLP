/* Geração de código */
#ifndef _CODEGEN
#define _CODEGEN

	#include "codegen.h"
	#include "LC.h"

	/*
	 * TODO
	 * fazer comparacao de string
	 * fazer simulacao de And e Or com aritmeticos
	 * Fazer loop EM ASSEMBLY para carregar posicao a posicao da string para o id
	 */

	char *buffer;       /* buffer de criacao do codigo asm     */
	char *aux;          /* buffer auxiliar para criacao do asm */
	int CD = 0x4000;    /* contator de dados em hexadecimal    */
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

	/* operacoes aritmeticas, add, sub, imul e idiv */
	void aritmeticos(char* op, char *RD, char *RO, struct Fator *pai)
	{
		pai->endereco = novoTemp(TAM_INT);

		if (op == "imul" || op == "idiv") {

			CONCAT_BUF("%s %s",op, RD);

		} else if (op == "add" || op == "sub") {

			CONCAT_BUF("%s %s %s",op, RD, RO);
			RO = RD;

		}

		CONCAT_BUF("mov DS:[%d], %s", pai->endereco, RO);
	}

	/* comparacoes nao string */
	void comp(char *op, struct Fator *pai)
	{
		int verdadeiro = novoRot();
		int falso = novoRot();
	
		CONCAT_BUF("cmp AX, BX");
		CONCAT_BUF("%s R%d", op, verdadeiro);
		CONCAT_BUF("mov AX, 0");
		CONCAT_BUF("jmp R%D",falso);

		CONCAT_BUF("R%d:",verdadeiro);
		CONCAT_BUF("mov AX, 1");
		CONCAT_BUF("R%d:",falso);
	
		pai->endereco = novoTemp(TAM_INT);
		pai->tipo = TP_Logico;
	
		CONCAT_BUF("mov DS:[%d] AX", pai->endereco);
	}

	void compChar(struct Fator *pai)
	{
		int init = novoRot();
		int verdadeiro = novoRot();
		int falso = novoRot();
		int fimStr = novoRot();

		CONCAT_BUF("R%d:",init);        /* marca o inicio do loop */
		CONCAT_BUF("mov CH, DS:[AX]");  /* move para CH o caractere da string em AX*/
		CONCAT_BUF("mov CL, DS:[BX]");  /* move para CH o caractere da string em BX*/
		CONCAT_BUF("cmp CH, CL");       /* compara as strings */
		CONCAT_BUF("je R%d", verdadeiro);
		CONCAT_BUF("mov AX, 0");
		CONCAT_BUF("jmp R%D",fimStr);   /* strings diferentes */
		CONCAT_BUF("R%d:",verdadeiro);  /* caracteres iguais */
		CONCAT_BUF("cmp CH, $");        /* verifica se chegou no final da primeira string */
		CONCAT_BUF("je R%d",iguais);
		CONCAT_BUF("cmp CL, $");        /* verifica se chegou no final da segunda string */
		CONCAT_BUF("je R%d",iguais);
										/* nao chegou fim de nenhuma string */
		CONCAT_BUF("add AX, 1");        /* anda uma posicao no primeiro string */
		CONCAT_BUF("add BX, 1");        /* anda uma posicao no segundo string  */

		CONCAT_BUF("jmp R%d", init);    /* volta ao inicio do loop */

		CONCAT_BUF("R%d:",iguais);      /* strings iguais */
		CONCAT_BUF("mov AX, 1");

		CONCAT_BUF("R%d:", fimStr);     /* fim de string */
	
		pai->endereco = novoTemp(TAM_INT);
		pai->tipo = TP_Logico;
	
		CONCAT_BUF("mov DS:[%d] AX", pai->endereco);
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
		CONCAT_BUF("byte %Xh DUP(?)\t\t\t;dimensiona pilha",CD);
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
		CONCAT_BUF("byte %Xh DUP(?)\t\t\t;temporarios",CD);
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
		CONCAT_BUF("mov ah, 4Ch");
		CONCAT_BUF("int 21h");
		CONCAT_BUF("END strt\t\t\t\t\t;fim programa");
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
		END_ID = CD;

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
			CONCAT_BUF("%s %d DUP(?)\t\t\t;var. Vet %s. em %Xh", nome, tam, tipo, CD);
		 }else {
			CONCAT_BUF("%s %s\t\t\t\t\t\t; %s. %s. em %Xh", nome, valor, classe, tipo, CD);
		 }

		/* incrementa a posicao de memoria com o numero de bytes utilizado */
		CD+=n_bytes;
	}

	void genAtribuicao(struct Fator *fator)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genAtribuicao");

		/* int e logico */
		if (toLogico(regLex.tipo) == TP_Logico) {
			CONCAT_BUF("mov AX, DS:[%d]", fator->endereco);
			CONCAT_BUF("mov %d, AX", regLex.endereco->simbolo.memoria);
		} else {
			/* string, move caractere por caractere */
			int init = novoRot();
			int fim = novoRot();
			int endereco = novoTemp();

			CONCAT_BUF("mov AX, %d", fator->endereco);
			CONCAT_BUF("mov DX, %d", endereco); /* concatena em endereco*/

			CONCAT_BUF("R%d", init); /* inicio do loop */
			CONCAT_BUF("mov CX, DS:[AX]"); /* caractere em CX */
			CONCAT_BUF("cmp CX, $"); /* verifica se chegou no fim */
			CONCAT_BUF("jmp R%d", fim); /* fim da str */
			CONCAT_BUF("mov DS:[DX], CX"); /* transfere pro endereco a string */
			CONCAT_BUF("add DX, 1"); /* avanca posicao a receber o proximo caractere */
			CONCAT_BUF("add AX, 1"); /* proximo caractere */
			CONCAT_BUF("jmp R%d", init);
			CONCAT_BUF("R%d", fim);

			/* transfere pro endereco do id o endereco da string */
			CONCAT_BUF("mov %d, DX", regLex.endereco->simbolo.memoria);
		}
	}

	void fatorGeraLiteral(struct Fator *fator, char *val)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraConst");

		int tamTipo = (regLex.tipo == TP_Integer || regLex.tipo == TP_Logico) ? TAM_INT : TAM_CHA;
		
		/* string */
		if (fator->tipo == TP_Char) {
			A_SEG_PUB
				CONCAT_BUF("byte \"%s$\"",removeAspas(val));
			F_SEG_PUB
			
			fator->endereco = CD;
			fator->tamanho = regLex.tamanho;
			fator->tipo = regLex.tipo;

			CD += fator->tamanho * tamTipo;

		/* nao string */
		} else {
			fator->endereco = novoTemp(tamTipo);
			CONCAT_BUF("mov AX, %s",val);
			CONCAT_BUF("mov %d, AX",fator->endereco);
		}
	}

	void fatorGeraId(struct Fator *fator)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraId");

		/* acao semantica */
		verificaDeclaracao(regLex.endereco->simbolo.lexema);

		fator->endereco = regLex.endereco->simbolo.tipo;
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
		CONCAT_BUF("mov AX, DS:[%d]", expr->endereco);

		/* int usa 2 bytes, portanto contamos a posicao*2 */
		if (regLex.tipo == TP_Integer)
			CONCAT_BUF("add AX, DS:[%d]", expr->endereco);

		/* soma o endereco do id indice + posicao = endereco real */
		CONCAT_BUF("add  AX, %d", END_ID);

		/* move para um registrador o valor na posicao de memoria calculada */
		CONCAT_BUF("mov AX, DS:[AX]", fator->endereco);

		/* move o que estiver no endereco real para o temporario */
		CONCAT_BUF("mov %d, AX", fator->endereco);
	}

	void fatorGeraExp(struct Fator *fator, struct Fator *expr)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraExp");

		fator->endereco = expr->endereco;
		fator->tamanho = expr->tamanho;
		fator->tipo = expr->tipo;
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

		int tam = (regLex.tipo == TP_Integer || regLex.tipo == TP_Logico) ? TAM_INT : TAM_CHA;

		pai->endereco = novoTemp(tam);
		CONCAT_BUF("mov AX, DS:[%d]", filho->endereco);
		/* negacao aritmetica */
		CONCAT_BUF("neg AX");
		CONCAT_BUF("mov DS:[%d], AX",pai->endereco);

	}

	/* repassa dados do filho para o pai */
	void atualizaPai(struct Fator *pai, struct Fator *filho)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("atualizaPai");

		pai->endereco = filho->endereco;
		pai->tipo = filho->tipo;
		pai->tamanho = filho->tamanho;
	}

	/* salva o operador */
	void guardaOp(struct Fator *pai)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("guardaOp");

		pai->op = regLex.token;
	}

	/* operacoes entre termos */
	void genOpTermos(struct Fator *pai, struct Fator *filho)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genOpTermos");

		CONCAT_BUF("mov AX, %d",pai->endereco);
		CONCAT_BUF("mov BX, %d",filho->endereco);

		char *op;        /* codigo assembly da operacao */
		char *RD = "AX"; /* registrador Destino         */
		char *RO = "BX"; /* registrador origem          */

		switch (pai->op) {
			case Vezes: /* fallthrough */
			case And: 
						op = "imul";
						RO = "AX";
						aritmeticos(op,RD,RO,pai);
						break;

			case Barra: op = "idiv";
						RO = "AX";
						aritmeticos(op,RD,RO,pai);
						break;

			case Porcento: 
						op = "idiv";
						RO = "DX";
						aritmeticos(op,RD,RO,pai);
						break;

			case Mais: 
						op = "add";
						aritmeticos(op, RD, RO, pai);
						break;

			case Menos:
						op = "sub";
						aritmeticos(op, RD, RO, pai);
						break;

			case Igual:
						op = "je";
						if (pai->tipo == TP_Char)
							compChar(pai);
						else
							comp(op, pai);

						break;

			case Diferente:
						op = "jne";
						comp(op, pai);
						break;

			case Maior: 
						op = "jg";
						comp(op, pai);
						break;

			case Menor:
						op = "jl";
						comp(op, pai);
						break;

			case MaiorIgual:
						op = "jge";
						comp(op, pai);
						break;

			case MenorIgual:
						op = "jle";
						comp(op, pai);
						break;

		}

	}

#endif 
