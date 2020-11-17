/* Geração de código */
#ifndef _CODEGEN
#define _CODEGEN

	#include "codegen.h"
	#include "LC.h"

	char *buffer;       /* buffer de criacao do codigo asm     */
	char *aux;          /* buffer auxiliar para criacao do asm */
	int CD = 0x4000;    /* contator de dados em hexadecimal    */
	int DS = 0x0;       /* contador de temporários             */
	rot RT = 1;         /* contador de rotulos                 */

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

			CONCAT_BUF("%s %s\n",op, RD);

		} else if (op == "add" || op == "sub") {

			CONCAT_BUF("%s %s %s\n",op, RD, RO);
			RO = RD;

		}

		CONCAT_BUF("mov DS:[%d], %s\n", pai->endereco, RO);
	}

	/* comparacoes nao string */
	void comp(char *op, struct Fator *pai)
	{
		rot verdadeiro = novoRot();
		rot falso = novoRot();
	
		CONCAT_BUF("cmp AX, BX\n");
		CONCAT_BUF("%s R%d\n", op, verdadeiro);
		CONCAT_BUF("mov AX, 0\n");
		CONCAT_BUF("jmp R%d\n",falso);

		CONCAT_BUF("R%d:\n",verdadeiro);
		CONCAT_BUF("mov AX, 1\n");
		CONCAT_BUF("R%d:\n",falso);
	
		pai->endereco = novoTemp(TAM_INT);
		pai->tipo = TP_Logico;
	
		CONCAT_BUF("mov DS:[%d] AX\n", pai->endereco);
	}

	void compChar(struct Fator *pai)
	{
		rot inicio = novoRot();
		rot verdadeiro = novoRot();
		rot falso = novoRot();
		rot fimStr = novoRot();
		rot iguais = novoRot();

		CONCAT_BUF("R%d:\n",inicio);        /* marca o inicio do loop */
		CONCAT_BUF("mov CH, DS:[AX]\n");  /* move para CH o caractere da string em AX*/
		CONCAT_BUF("mov CL, DS:[BX]\n");  /* move para CH o caractere da string em BX*/
		CONCAT_BUF("cmp CH, CL\n");       /* compara as strings */
		CONCAT_BUF("je R%d\n", verdadeiro);
		CONCAT_BUF("mov AX, 0\n");
		CONCAT_BUF("jmp R%D\n",fimStr);   /* strings diferentes */
		CONCAT_BUF("R%d:\n",verdadeiro);  /* caracteres iguais */
		CONCAT_BUF("cmp CH, $\n");        /* verifica se chegou no final da primeira string */
		CONCAT_BUF("je R%d\n",iguais);
		CONCAT_BUF("cmp CL, $\n");        /* verifica se chegou no final da segunda string */
		CONCAT_BUF("je R%d\n",iguais);
										/* nao chegou fim de nenhuma string */
		CONCAT_BUF("add AX, 1\n");        /* anda uma posicao no primeiro string */
		CONCAT_BUF("add BX, 1\n");        /* anda uma posicao no segundo string  */

		CONCAT_BUF("jmp R%d\n", inicio);    /* volta ao inicio do loop */

		CONCAT_BUF("R%d:\n",iguais);      /* strings iguais */
		CONCAT_BUF("mov AX, 1\n");

		CONCAT_BUF("R%d:\n", fimStr);     /* fim de string */
	
		pai->endereco = novoTemp(TAM_INT);
		pai->tipo = TP_Logico;
	
		CONCAT_BUF("mov DS:[%d] AX\n", pai->endereco);
	}

	/* move cursor para linha de baixo */
	void proxLinha()
	{
		CONCAT_BUF("mov AH, 02h\n");
		CONCAT_BUF("mov DL, 0Dh\n");
		CONCAT_BUF("int 21h\n");
		CONCAT_BUF("mov DL, 0Ah\n");
		CONCAT_BUF("int 21h\n");
	}

	/* inicia o buffer */
	void iniciarCodegen(void)
	{ 
		/* DEBUGGER E PILHA */
		DEBUGGEN("iniciarCodegen");

		buffer = malloc(sizeof(char) * MAX_BUF_SIZE);
		aux = malloc(sizeof(char) * MAX_AUX_SIZE);

		/* Pilha */
		CONCAT_BUF("sseg SEGMENT STACK\t\t\t;inicio seg. pilha\n");
		CONCAT_BUF("byte %Xh DUP(?)\t\t\t;dimensiona pilha\n",CD);
		CONCAT_BUF("sseg ENDS\t\t\t\t\t;fim seg. pilha\n");

	}

	/* concatena garantindo que o ultimo caractere eh o \n */
	void buf_concatenar(void)
	{

		int buf_size = strlen(buffer);


		/* se o buffer vai encher, escreve no arquivo e esvazia */
		if ((buf_size+strlen(aux)) >= MAX_BUF_SIZE)
			flush();

		buffer = concatenar(buffer, aux);
		buf_size = strlen(buffer);

		aux[0] = '\0'; /* limpa aux */
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
		CONCAT_BUF("dseg SEGMENT PUBLIC\t\t\t;inicio seg. dados\n");
		CONCAT_BUF("byte %Xh DUP(?)\t\t\t;temporarios\n",CD);
	}

	/* finaliza o bloco de declaracoes asm 
	 * e inicia o bloco de comandos asm
	 */
	void fimDecInitCom(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fimDecInitCom");

		/* fim declaracao */
		CONCAT_BUF("dseg ENDS\t\t\t\t\t;fim seg. dados\n");
		/* comandos */
		CONCAT_BUF("cseg SEGMENT PUBLIC\t\t\t;inicio seg. codigo\n");
		CONCAT_BUF("     ASSUME CS:cseg, DS: dseg\n");
		CONCAT_BUF("strt:\t\t\t\t\t\t;inicio do programa\n");
		CONCAT_BUF("    ;comandos \n");

	}

	void fimComandos(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fimComandos");

		CONCAT_BUF("cseg ENDS\t\t\t\t\t;fim seg. codigo\n");
		CONCAT_BUF("mov ah, 4Ch\n");
		CONCAT_BUF("int 21h\n");
		CONCAT_BUF("END strt\t\t\t\t\t;fim programa\n");
	}

	/* gera codigo para acesso a array */
	void acessoArray(struct Fator *pai, struct Fator *filho)
	{
		CONCAT_BUF("mov AX, %d\n", pai->endereco);
		/* adiciona o resultado da expressao ao inicio do array para encontrar a posicao de acesso */
		CONCAT_BUF("add AX, DS:[%d]\n", filho->endereco); 
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
		regLex.endereco->simbolo.memoria = CD;

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
			CONCAT_BUF("%s %d DUP(?)\t\t\t;var. Vet %s. em %Xh\n", nome, tam, tipo, CD);
		} else {
			CONCAT_BUF("%s %s\t\t\t\t\t\t; %s. %s. em %Xh\n", nome, valor, classe, tipo, CD);
		}

		/* incrementa a posicao de memoria com o numero de bytes utilizado */
		CD+=n_bytes;
	}

	void genAtribuicao(struct Fator *pai, struct Fator *fator)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genAtribuicao");

		/* int e logico */
		if (toLogico(regLex.tipo) == TP_Logico) {
			CONCAT_BUF("mov AX, DS:[%d]\n", fator->endereco);
			CONCAT_BUF("mov %d, AX\n", pai->endereco);
		} else {
			/* string, move caractere por caractere */
			rot inicio = novoRot();
			rot fim = novoRot();
			rot endereco = novoRot();

			CONCAT_BUF("mov AX, %d\n", fator->endereco);
			CONCAT_BUF("mov DX, %d\n", endereco); /* concatena em endereco*/

			CONCAT_BUF("R%d:\n", inicio); /* inicio do loop */
			CONCAT_BUF("mov BX, DS:[AX]\n"); /* caractere em BX */
			CONCAT_BUF("cmp BX, $\n"); /* verifica se chegou no fim */
			CONCAT_BUF("jmp R%d\n", fim); /* fim da str */
			CONCAT_BUF("mov DS:[DX], BX\n"); /* transfere pro endereco a string */
			CONCAT_BUF("add DX, 1\n"); /* avanca posicao a receber o proximo caractere */
			CONCAT_BUF("add AX, 1\n"); /* proximo caractere */
			CONCAT_BUF("jmp R%d\n", inicio);
			CONCAT_BUF("R%d:\n", fim);
			CONCAT_BUF("mov DS:[DX], BX\n"); /* copia tambem o dolar */

			/* transfere pro endereco do id o endereco da string */
			CONCAT_BUF("mov %d, DX\n", pai->endereco);
		}
	}

	/* segunda acao do for
	 * gera a declaracao e o fim
	 * for ID=EXP TO EXP
	 */
	void genRepeticao(struct Fator *pai, struct Fator *filho, rot inicio, rot fim)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genRepeticao");

		CONCAT_BUF("mov CX, DS:[%d]\n",pai->endereco); /* move o valor de ID para cx */
		CONCAT_BUF("R%d:\n", inicio); /* inicio do loop */
		CONCAT_BUF("mov BX, DS:[%d]\n", filho->endereco); /* move o resultado da TO EXP para BX */
		CONCAT_BUF("cmp CX, BX\n"); /* compara os valores */
		CONCAT_BUF("jg R%d\n", fim); /* vai para o fim se id > exp */
	}

	/* fim do loop de repeticao 
	 * incrementa e desvia
	 */
	void genFimRepeticao(struct Fator *pai, rot inicio, rot fim,char *step)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genFimRepeticao");

		CONCAT_BUF("mov CX, DS:[%d]\n",pai->endereco); /* move o valor de ID para cx */
		CONCAT_BUF("add CX, %s\n", step);
		CONCAT_BUF("mov DS:[%d], CX\n", pai->endereco); /* guarda o valor de id */
		CONCAT_BUF("jmp R%d\n", inicio);
		CONCAT_BUF("R%d:\n", fim);
	}

	/* gera o inicio do comando de teste */
	void genTeste(struct Fator *filho, rot falso, rot fim)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genTeste");

		CONCAT_BUF("mov AX, DS:[%d]\n", filho->endereco);
		CONCAT_BUF("cmp AX, 0\n"); /* checa se eh falso */
		CONCAT_BUF("je R%d\n", falso); /* vai para falso */
	}

	/* gera a parte do else, que pode ser vazia */
	void genElseTeste(rot falso, rot fim)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genElseTeste");

		/* else */
		CONCAT_BUF("j R%d\n",fim);
		CONCAT_BUF("R%d:\n",falso);
	}

	/* gera o rotulo de saida do teste */
	void genFimTeste(rot fim)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genFimTeste");

		CONCAT_BUF("R%d:\n", fim);
	}

	void genEntrada(struct Fator *pai)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genEntrada");

		/* buffer para entrada do teclado, tam max = 255 */
		int buffer = novoTemp(pai->tamanho);
		rot inicio = novoRot();
		rot meio = novoRot();
		rot fim = novoRot();

		/* string */
		if (pai->tipo == TP_Char) {

			CONCAT_BUF("mov DX, %d\n", buffer);
			CONCAT_BUF("mov AL, %d\n", pai->tamanho); /* tamanho do buffer */
			CONCAT_BUF("mov DS:[%d], AL\n",buffer);
			CONCAT_BUF("mov AH, 0Ah\n");
			CONCAT_BUF("int 21h\n");

			CONCAT_BUF("mov BX, DS:[%d]\n", buffer+1); /*numero de caracteres lidos */
			CONCAT_BUF("mov DX, 0\n"); /* contador de posicoes */
			CONCAT_BUF("mov CX, %d\n", pai->endereco); /* contador de posicao no end pai */

			/* transfere para o endereco do pai o conteudo lido */
			/* inicio do loop */
			CONCAT_BUF("R%d:\n",inicio);
			CONCAT_BUF("cmp DX, BX\n"); /* enquanto nao transferir todos os caracteres */
			CONCAT_BUF("je R%d\n",fim);
			CONCAT_BUF("mov AX, %d\n", buffer+2);/* carga util a partir da 3 posicao */
			CONCAT_BUF("add AX, DX\n"); /* soma offset ao endereco base */
			CONCAT_BUF("mov AX, DS:[AX]\n");
			CONCAT_BUF("mov DS:[CX], AX\n");
			CONCAT_BUF("add DX, 1\n");
			CONCAT_BUF("add CX, 1\n");
			CONCAT_BUF("jmp R%d\n", inicio);
			CONCAT_BUF("R%d:\n", fim);
			CONCAT_BUF("mov DS:[CX], $\n"); /* coloca $ no final */
		}

		/* inteiro */
		else if (pai->tipo == TP_Integer) {
			CONCAT_BUF("mov di, %d ;posição do string\n",buffer+2);
			CONCAT_BUF("mov ax, 0 ;acumulador\n");
			CONCAT_BUF("mov cx, 10 ;base decimal\n");
			CONCAT_BUF("mov dx, 1 ;valor sinal +\n");
			CONCAT_BUF("mov bh, 0\n");
			CONCAT_BUF("mov bl, ds:[di] ;caractere\n");
			CONCAT_BUF("cmp bx, 2Dh ;verifica sinal\n");
			CONCAT_BUF("jne R%d ;se não negativo\n", inicio);
			CONCAT_BUF("mov dx, -1 ;valor sinal -\n");
			CONCAT_BUF("add di, 1 ;incrementa base\n");
			CONCAT_BUF("mov bl, ds:[di] ;próximo caractere\n");
			CONCAT_BUF("R%d:\n",inicio);
			CONCAT_BUF("push dx ;empilha sinal\n");
			CONCAT_BUF("mov dx, 0 ;reg. multiplicação\n");
			CONCAT_BUF("R%d:\n",meio);
			CONCAT_BUF("cmp bx, 0dh ;verifica fim string\n");
			CONCAT_BUF("je R%d ;salta se fim string\n",fim);
			CONCAT_BUF("imul cx ;mult. 10\n");
			CONCAT_BUF("add bx, -48 ;converte caractere\n");
			CONCAT_BUF("add ax, bx ;soma valor caractere\n");
			CONCAT_BUF("add di, 1 ;incrementa base\n");
			CONCAT_BUF("mov bh, 0\n");
			CONCAT_BUF("mov bl, ds:[di] ;próximo caractere\n");
			CONCAT_BUF("jmp R%d ;loop\n",meio);
			CONCAT_BUF("R%d:\n", fim);
			CONCAT_BUF("pop cx ;desempilha sinal\n");
			CONCAT_BUF("imul cx ;mult. sinal\n");

 			/* transfere resultado para o pai */
			CONCAT_BUF("mov CX, %d\n", pai->endereco);
			CONCAT_BUF("mov DS:[CX], AX\n");
		}

		proxLinha();
	}

	void genSaida(struct Fator *pai, int ln)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genSaida");

		int endereco= novoTemp(pai->tamanho);
		rot inicio = novoRot();
		rot meio = novoRot();
		rot fim = novoRot();

		/* conversao de inteiro para string */
		if (pai->tipo == TP_Integer) {
			CONCAT_BUF("mov di, %d ;end. string temp.\n",endereco);
			CONCAT_BUF("mov cx, 0 ;contador\n");
			CONCAT_BUF("cmp ax,0 ;verifica sinal\n");
			CONCAT_BUF("jge R%d ;salta se número positivo\n", inicio);
			CONCAT_BUF("mov bl, 2Dh ;senão, escreve sinal –\n");
			CONCAT_BUF("mov ds:[di], bl\n");
			CONCAT_BUF("add di, 1 ;incrementa índice\n");
			CONCAT_BUF("neg ax ;toma módulo do número\n");
			CONCAT_BUF("R%d:\n",inicio);
			CONCAT_BUF("mov bx, 10 ;divisor\n");
			CONCAT_BUF("R%d:\n",meio);
			CONCAT_BUF("add cx, 1 ;incrementa contador\n");
			CONCAT_BUF("mov dx, 0 ;estende 32bits p/ div.\n");
			CONCAT_BUF("idiv bx ;divide DXAX por BX\n");
			CONCAT_BUF("push dx ;empilha valor do resto\n");
			CONCAT_BUF("cmp ax, 0 ;verifica se quoc. é 0\n");
			CONCAT_BUF("jne R%d ;se não é 0, continua\n", meio);
			CONCAT_BUF(";agora, desemp. os valores e escreve o string\n");
			CONCAT_BUF("R%d:\n",fim);
			CONCAT_BUF("pop dx ;desempilha valor\n");
			CONCAT_BUF("add dx, 30h ;transforma em caractere\n");
			CONCAT_BUF("mov ds:[di],dl ;escreve caractere\n");
			CONCAT_BUF("add di, 1 ;incrementa base\n");
			CONCAT_BUF("add cx, -1 ;decrementa contador\n");
			CONCAT_BUF("cmp cx, 0 ;verifica pilha vazia\n");
			CONCAT_BUF("jne R%d ;se não pilha vazia, loop\n", fim);
			CONCAT_BUF(";grava fim de string\n");
			CONCAT_BUF("mov dl, 024h ;fim de string\n");
			CONCAT_BUF("mov ds:[di], dl ;grava '$'\n");
		}

		CONCAT_BUF(";exibe string\n");
		CONCAT_BUF("mov dx, %d\n", endereco);
		CONCAT_BUF("mov ah, 09h\n");
		CONCAT_BUF("int 21h	\n");

		if (ln)
			proxLinha();
	}

	void fatorGeraLiteral(struct Fator *fator, char *val)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraConst");

		int tamTipo = (regLex.tipo == TP_Integer || regLex.tipo == TP_Logico) ? TAM_INT : TAM_CHA;
		
		/* string */
		if (fator->tipo == TP_Char) {
			A_SEG_PUB
				CONCAT_BUF("byte \"%s$\"\n",removeAspas(val));
			F_SEG_PUB
			
			fator->endereco = CD;
			fator->tamanho = regLex.tamanho;
			fator->tipo = regLex.tipo;

			CD += fator->tamanho * tamTipo;

		/* nao string */
		} else {
			fator->endereco = novoTemp(tamTipo);
			CONCAT_BUF("mov AX, %s\n",val);
			CONCAT_BUF("mov %d, AX\n",fator->endereco);
		}
	}

	void fatorGeraId(struct Fator *fator, char *id)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraId");

		struct Celula *registro = pesquisarRegistro(id);

		fator->endereco = registro->simbolo.tipo;
		fator->tipo = registro->simbolo.tipo;
		fator->tamanho = registro->simbolo.tamanho;
	}

	void fatorGeraArray(struct Fator *fator, struct Fator *expr, char *id)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraArray");

		fator->endereco = novoTemp((regLex.tipo == TP_Integer) ? TAM_INT : TAM_CHA);

		fator->tamanho = 1;

		/* move para AX o endereco da expressao que vai conter o indice */
		CONCAT_BUF("mov AX, DS:[%d]\n", expr->endereco);

		/* int usa 2 bytes, portanto contamos a posicao*2 */
		if (regLex.tipo == TP_Integer)
			CONCAT_BUF("add AX, DS:[%d]\n", expr->endereco);

		/* soma o endereco do id indice + posicao = endereco real */
		CONCAT_BUF("add  AX, %d\n", pesquisarRegistro(id)->simbolo.memoria);

		/* move para um registrador o valor na posicao de memoria calculada */
		CONCAT_BUF("mov AX, DS:[AX]\n", fator->endereco);

		/* move o que estiver no endereco real para o temporario */
		CONCAT_BUF("mov %d, AX\n", fator->endereco);
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
		CONCAT_BUF("mov AX, DS:[%d]\n", filho->endereco);
		/* expressao not simulada com expressoes aritmeticas */
		CONCAT_BUF("neg AX\n");
		CONCAT_BUF("add AX, 1\n");
		CONCAT_BUF("mov DS:[%d], AX\n",pai->endereco);
	}

	void fatorGeraMenos(struct Fator *pai, struct Fator *filho)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraMenos");

		int tam = (regLex.tipo == TP_Integer || regLex.tipo == TP_Logico) ? TAM_INT : TAM_CHA;

		pai->endereco = novoTemp(tam);
		CONCAT_BUF("mov AX, DS:[%d]\n", filho->endereco);
		/* negacao aritmetica */
		CONCAT_BUF("neg AX\n");
		CONCAT_BUF("mov DS:[%d], AX\n",pai->endereco);

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

		CONCAT_BUF("mov AX, %d\n",pai->endereco);
		CONCAT_BUF("mov BX, %d\n",filho->endereco);

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

			case Or:    /* fallthrough */
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
