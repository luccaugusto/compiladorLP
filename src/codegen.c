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

	/* operacoes aritmeticas, ADD, SUB, IMUL e IDIV */
	void aritmeticos(char* op, char *RD, char *RO, struct Fator *pai)
	{
		pai->endereco = novoTemp(TAM_INT);

		CONCAT_BUF(";inicio de operacao %s\n", op);
		if (op == "IMUL" || op == "IDIV") {

			CONCAT_BUF("%s %s\n",op, RD);

		} else if (op == "ADD" || op == "SUB") {

			CONCAT_BUF("%s %s, %s\n",op, RD, RO);
			RO = RD;

		}

		CONCAT_BUF("MOV DS:[%d], %s ;grava o resultado da operacao no endereco\n", pai->endereco, RO);
	}

	/* comparacoes nao string */
	void comp(char *op, struct Fator *pai)
	{
		rot verdadeiro = novoRot();
		rot falso = novoRot();
	
		CONCAT_BUF(";inicio de comparacao\n");
		CONCAT_BUF("CMP AX, BX\n");
		CONCAT_BUF("%s R%d ;comparacao verdadeiro\n", op, verdadeiro);
		CONCAT_BUF("MOV AX, 0\n");
		CONCAT_BUF("JMP R%d ;comparacao falso\n",falso);

		CONCAT_BUF("R%d: ;verdadeiro\n",verdadeiro);
		CONCAT_BUF("MOV AX, 1\n");
		CONCAT_BUF("R%d: ;falso\n",falso);
	
		pai->endereco = novoTemp(TAM_INT);
		pai->tipo = TP_Logico;
	
		CONCAT_BUF("MOV DS:[%d], AX ;guarda no endereco o resultado da expressao\n", pai->endereco);
	}

	void compChar(struct Fator *pai)
	{
		rot inicio = novoRot();
		rot verdadeiro = novoRot();
		rot falso = novoRot();
		rot fimStr = novoRot();
		rot iguais = novoRot();

		CONCAT_BUF(";inicio de comparacao de string\n");
		CONCAT_BUF("R%d: ;marca o inicio do loop\n",inicio);
		CONCAT_BUF("MOV CL, DS:[BX] ;move para CL o caractere da string em BX\n");

		CONCAT_BUF("MOV DX, BX ;Troca AX de lugar com BX\n");
		CONCAT_BUF("MOV BX, AX ;pois so BX pode ser utilizado\n");
		CONCAT_BUF("MOV AX, DX ;para acessar memoria\n");

		CONCAT_BUF("MOV CH, DS:[BX] ;move para CH o caractere da string em BX\n");
		CONCAT_BUF("CMP CH, CL ;compara as strings\n");
		CONCAT_BUF("JE R%d ;jmp verdadeiro\n", verdadeiro);
		CONCAT_BUF("MOV AX, 0\n");
		CONCAT_BUF("JMP R%D ;strings diferentes\n",fimStr);
		CONCAT_BUF("R%d: ;caracteres iguais\n",verdadeiro);
		CONCAT_BUF("CMP CH, 0Dh ;verifica se chegou no final da primeira string\n");
		CONCAT_BUF("JE R%d ;iguais\n",iguais);
		CONCAT_BUF("CMP CL, 0Dh ;verifica se chegou no final da segunda string\n");
		CONCAT_BUF("JE R%d ;iguais\n",iguais);
		CONCAT_BUF("; nao chegou fim de nenhuma string\n");
		CONCAT_BUF("ADD AX, 1 ;anda uma posicao no primeiro string\n");
		CONCAT_BUF("ADD BX, 1 ;anda uma posicao no segundo string\n");

		CONCAT_BUF("JMP R%d ;volta ao inicio do loop\n", inicio);

		CONCAT_BUF("R%d: ;strings iguais\n",iguais);
		CONCAT_BUF("MOV AX, 1\n");

		CONCAT_BUF("R%d: ;fim de string\n", fimStr);
	
		pai->endereco = novoTemp(TAM_INT);
		pai->tipo = TP_Logico;
	
		CONCAT_BUF("MOV DS:[%d], AX ;salva no endereco o resultado\n", pai->endereco);
	}

	/* move cursor para linha de baixo */
	void proxLinha()
	{
		CONCAT_BUF(";gera quebra de linha\n");
		CONCAT_BUF("MOV AH, 02h\n");
		CONCAT_BUF("MOV DL, 0Dh\n");
		CONCAT_BUF("INT 21h\n");
		CONCAT_BUF("MOV DL, 0Ah\n");
		CONCAT_BUF("INT 21h\n");
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
		CONCAT_BUF("DSEG ENDS\t\t\t\t\t;fim seg. dados\n");
		/* comandos */
		CONCAT_BUF("CSEG SEGMENT PUBLIC\t\t\t;inicio seg. codigo\n");
		CONCAT_BUF("     ASSUME CS:cseg, DS: dseg\n");
		CONCAT_BUF("STRT:\t\t\t\t\t\t;inicio do programa\n");
		CONCAT_BUF("    ;comandos\n");

	}

	void fimComandos(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fimComandos");

		CONCAT_BUF("interrompe o programa\n");
		CONCAT_BUF("MOV AH, 4Ch\n");
		CONCAT_BUF("INT 21h\n");
		CONCAT_BUF("CSEG ENDS\t\t\t\t\t;fim seg. codigo\n");
		CONCAT_BUF("END strt\t\t\t\t\t;fim programa\n");
	}

	/* gera codigo para acesso a array */
	void acessoArray(struct Fator *pai, struct Fator *filho)
	{
		CONCAT_BUF(";acesso a array\n");
		CONCAT_BUF("MOV AX, %d ;endereco base\n", pai->endereco);
		/* adiciona o resultado da expressao ao inicio do array para encontrar a posicao de acesso */
		CONCAT_BUF("ADD AX, DS:[%d] ;soma com offset\n", filho->endereco); 
		CONCAT_BUF("fim acesso a array");
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
			CONCAT_BUF(";atribuicao de inteiros e logicos\n");
			CONCAT_BUF("MOV AX, DS:[%d] ;endereco do resultado para ax\n", fator->endereco);
			CONCAT_BUF("MOV DS:[%d], AX ;ax para posicao de memoria da variavel\n", pai->endereco);
			CONCAT_BUF(";fim atribuicao de inteiros e logicos\n");
		} else {
			/* string, move caractere por caractere */
			rot inicio = novoRot();
			rot fim = novoRot();

			CONCAT_BUF(";atribuicao de strings\n");
			CONCAT_BUF("MOV BX, %d ;endereco da string para bx\n", fator->endereco);
			CONCAT_BUF("MOV DI, %d ;endereco do pai para concatenar\n", pai->endereco);

			CONCAT_BUF("R%d: ;inicio do loop\n", inicio);
			CONCAT_BUF("MOV CX, DS:[BX] ;joga caractere em CX\n");
			CONCAT_BUF("CMP CX, 0Dh ;verifica se chegou no fim\n");
			CONCAT_BUF("JE R%d ;fim da str\n", fim);
			CONCAT_BUF("MOV DS:[DI], CX ;transfere pro endereco a string\n");
			CONCAT_BUF("ADD DI, 1 ;avanca posicao a receber o proximo caractere\n");
			CONCAT_BUF("ADD BX, 1 ;proximo caractere\n");
			CONCAT_BUF("JMP R%d\n", inicio);
			CONCAT_BUF("R%d:\n", fim);
			CONCAT_BUF("MOV DS:[DI], 0Dh ;copia tambem o dolar\n");

			CONCAT_BUF(";fim atribuicao de strings\n");
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

		CONCAT_BUF(";inicio da repeticao\n");
		CONCAT_BUF("MOV CX, DS:[%d] ;move o valor de ID para cx \n",pai->endereco);
		CONCAT_BUF("R%d: ;inicio do loop \n", inicio);
		CONCAT_BUF("MOV BX, DS:[%d] ;move o resultado da TO EXP para BX \n", filho->endereco);
		CONCAT_BUF("CMP CX, BX ;compara os valores \n");
		CONCAT_BUF("JG R%d ;vai para o fim se id > exp \n", fim);
	}

	/* fim do loop de repeticao 
	 * incrementa e desvia
	 */
	void genFimRepeticao(struct Fator *pai, rot inicio, rot fim,char *step)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genFimRepeticao");

		CONCAT_BUF(";fim da repeticao\n");
		CONCAT_BUF("MOV CX, DS:[%d] ;move o valor de ID para cx \n",pai->endereco);
		CONCAT_BUF("ADD CX, %s ;soma o step\n", step);
		CONCAT_BUF("MOV DS:[%d], CX ;guarda o valor de id \n", pai->endereco);
		CONCAT_BUF("JMP R%d ;volta para o inicio\n", inicio);
		CONCAT_BUF("R%d: ;fim do loop\n", fim);
	}

	/* gera o inicio do comando de teste */
	void genTeste(struct Fator *filho, rot falso, rot fim)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genTeste");

		CONCAT_BUF(";inicio do teste\n");
		CONCAT_BUF("MOV AX, DS:[%d] ;move para ax o resultado da expressao logica\n", filho->endereco);
		CONCAT_BUF("CMP AX, 0 ;checa se eh falso \n");
		CONCAT_BUF("JE R%d ;vai para falso \n", falso);
	}

	/* gera a parte do else, que pode ser vazia */
	void genElseTeste(rot falso, rot fim)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genElseTeste");

		/* else */
		CONCAT_BUF("JMP R%d ;fim verdadeiro\n",fim);
		CONCAT_BUF("R%d: ;inicio else\n",falso);
	}

	/* gera o rotulo de saida do teste */
	void genFimTeste(rot fim)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("genFimTeste");

		CONCAT_BUF("R%d: ;fim do teste\n", fim);
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

		CONCAT_BUF(";inicio do comando de entrada\n");
		/* string */
		if (pai->tipo == TP_Char) {

			CONCAT_BUF(";entrada de strings\n");
			CONCAT_BUF("MOV DX, %d ;buffer para receber a string\n", buffer);
			CONCAT_BUF("MOV AL, %d ;tamanho do buffer \n", pai->tamanho);
			CONCAT_BUF("MOV DS:[%d], AL ;move tamanho do buffer para a primeira posicao do buffer\n",buffer);
			CONCAT_BUF("MOV AH, 0Ah ; interrupcao para leitura do teclado\n");
			CONCAT_BUF("INT 21h\n");

			CONCAT_BUF("MOV CX, DS:[%d] ;de caracteres lidos fica na segunda posicao do buffer\n", buffer+1);
			CONCAT_BUF("MOV DX, 0 ;contador de posicoes\n");
			CONCAT_BUF("MOV BX, %d ;contador de posicao no end pai\n", pai->endereco);

			CONCAT_BUF("; transfere para o endereco do pai o conteudo lido\n");
			
			CONCAT_BUF("R%d:\n",inicio); /* inicio do loop */
			CONCAT_BUF("CMP DX, CX ;compara o contador de caracteres lidos com quantas posicoes ja foram transferidas\n");
			CONCAT_BUF("JE R%d ;se for igual, ja leu tudo, vai pro fim\n",fim);
			CONCAT_BUF("MOV DI, %d ;move para DI o endereco base a partir da 3 posicao \n", buffer+2);
			CONCAT_BUF("ADD DI, DX ;soma offset ao endereco base \n");
			CONCAT_BUF("MOV DI, DS:[DI] ;move o caractere para DI\n");
			CONCAT_BUF("MOV DS:[BX], DI ;move DI para o endereco do pai\n");
			CONCAT_BUF("ADD DX, 1 ;soma 1 no offset\n");
			CONCAT_BUF("ADD BX, 1 ;soma 1 no indice do endereco\n");
			CONCAT_BUF("JMP R%d ;pula para o inicio\n", inicio);
			CONCAT_BUF("R%d: ;fim do loop\n", fim);
			CONCAT_BUF("MOV DS:[BX], 0Dh ;coloca $ no final \n");
			CONCAT_BUF(";fim entrada de strings\n");
		}

		/* inteiro */
		else if (pai->tipo == TP_Integer) {
			CONCAT_BUF("; inicio entrada de inteiros\n");
			CONCAT_BUF("MOV DI, %d ;posição do string\n",buffer+2);
			CONCAT_BUF("MOV AX, 0 ;acumulador\n");
			CONCAT_BUF("MOV BX, 10 ;base decimal\n");
			CONCAT_BUF("MOV DX, 1 ;valor sinal +\n");
			CONCAT_BUF("MOV BH, 0\n");
			CONCAT_BUF("MOV BL, DS:[DI] ;caractere\n");
			CONCAT_BUF("CMP CX, 2Dh ;verifica sinal\n");
			CONCAT_BUF("JNE R%d ;se não negativo jmp inicio\n", inicio);
			CONCAT_BUF("MOV DX, -1 ;valor sinal -\n");
			CONCAT_BUF("ADD DI, 1 ;incrementa base\n");
			CONCAT_BUF("MOV BL, DS:[DI] ;próximo caractere\n");
			CONCAT_BUF("R%d: ;inicio\n",inicio);
			CONCAT_BUF("PUSH DX ;empilha sinal\n");
			CONCAT_BUF("MOV DX, 0 ;reg. multiplicação\n");
			CONCAT_BUF("R%d: meio\n",meio);
			CONCAT_BUF("CMP CX, 0Dh ;verifica fim string\n");
			CONCAT_BUF("JE R%d ;salta fim se fim string\n",fim);
			CONCAT_BUF("IMUL BX ;mult. 10\n");
			CONCAT_BUF("ADD CX, -48 ;converte caractere\n");
			CONCAT_BUF("ADD AX, CX ;soma valor caractere\n");
			CONCAT_BUF("ADD DI, 1 ;incrementa base\n");
			CONCAT_BUF("MOV BH, 0\n");
			CONCAT_BUF("MOV BL, DS:[DI] ;próximo caractere\n");
			CONCAT_BUF("JMP R%d ;jmp meio\n",meio);
			CONCAT_BUF("R%d: ;fim\n", fim);
			CONCAT_BUF("POP BX ;desempilha sinal\n");
			CONCAT_BUF("IMUL BX ;mult. sinal\n");

 			CONCAT_BUF("; transfere resultado para o pai\n");
			CONCAT_BUF("MOV BX, %d\n", pai->endereco);
			CONCAT_BUF("MOV DS:[BX], AX\n");
			CONCAT_BUF("; fim entrada de inteiros\n");
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

		CONCAT_BUF(";inicio saida\n");
		/* conversao de inteiro para string */
		if (pai->tipo == TP_Integer) {
			CONCAT_BUF(";saida de inteiros\n");
			CONCAT_BUF("MOV DI, %d ;end. string temp.\n",endereco);
			CONCAT_BUF("MOV CX, 0 ;contador\n");
			CONCAT_BUF("CMP AX,0 ;verifica sinal\n");
			CONCAT_BUF("JGe R%d ;salta se número positivo\n", inicio);
			CONCAT_BUF("MOV BL, 2Dh ;senão, escreve sinal –\n");
			CONCAT_BUF("MOV DS:[DI], bl\n");
			CONCAT_BUF("ADD DI, 1 ;incrementa índice\n");
			CONCAT_BUF("neg AX ;toma módulo do número\n");
			CONCAT_BUF("R%d:\n",inicio);
			CONCAT_BUF("MOV BX, 10 ;divisor\n");
			CONCAT_BUF("R%d:\n",meio);
			CONCAT_BUF("ADD CX, 1 ;incrementa contador\n");
			CONCAT_BUF("MOV DX, 0 ;estende 32bits p/ div.\n");
			CONCAT_BUF("IDIV BX ;divide DXAX por BX\n");
			CONCAT_BUF("PUSH DX ;empilha valor do resto\n");
			CONCAT_BUF("CMP AX, 0 ;verifica se quoc. é 0\n");
			CONCAT_BUF("JNE R%d ;se não é 0, continua\n", meio);
			CONCAT_BUF(";agora, desemp. os valores e escreve o string\n");
			CONCAT_BUF("R%d:\n",fim);
			CONCAT_BUF("POP DX ;desempilha valor\n");
			CONCAT_BUF("ADD DX, 30h ;transforma em caractere\n");
			CONCAT_BUF("MOV DS:[DI],DL ;escreve caractere\n");
			CONCAT_BUF("ADD DI, 1 ;incrementa base\n");
			CONCAT_BUF("ADD CX, -1 ;decrementa contador\n");
			CONCAT_BUF("CMP CX, 0 ;verifica pilha vazia\n");
			CONCAT_BUF("JNE R%d ;se não pilha vazia, loop\n", fim);
			CONCAT_BUF(";grava fim de string\n");
			CONCAT_BUF("MOV DL, 024h ;fim de string\n");
			CONCAT_BUF("MOV DS:[DI], DL ;grava '$'\n");
			CONCAT_BUF(";fim saida de inteiros\n");
		}

		CONCAT_BUF(";exibe string\n");
		CONCAT_BUF("MOV DX, %d\n", endereco);
		CONCAT_BUF("MOV AH, 09h\n");
		CONCAT_BUF("INT 21h	\n");
		CONCAT_BUF(";fim saida\n");

		if (ln)
			proxLinha();
	}

	void fatorGeraLiteral(struct Fator *fator, char *val)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraConst");

		int tamTipo = (regLex.tipo == TP_Integer || regLex.tipo == TP_Logico) ? TAM_INT : TAM_CHA;
		
		CONCAT_BUF(";gera literal\n");
		/* string */
		if (fator->tipo == TP_Char) {
			A_SEG_PUB
				CONCAT_BUF("byte \"%s0Dh\"\n",removeAspas(val));
			F_SEG_PUB
			
			fator->endereco = CD;
			fator->tamanho = regLex.tamanho;
			fator->tipo = regLex.tipo;

			CD += fator->tamanho * tamTipo;

		/* nao string */
		} else {
			fator->endereco = novoTemp(tamTipo);
			CONCAT_BUF("MOV AX, %s ;literal para AX\n",val);
			CONCAT_BUF("MOV DS:[%d], AX ;ax para memoria\n",fator->endereco);
		}
		CONCAT_BUF(";fim gera literal\n");
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

		CONCAT_BUF(";acessar array\n");
		
		CONCAT_BUF("MOV BX, DS:[%d] ;move para BX o endereco da expressao que vai conter o indice \n", expr->endereco);

	
		if (regLex.tipo == TP_Integer)
			CONCAT_BUF("ADD BX, DS:[%d] ;int usa 2 bytes, portanto contamos a posicao\n", expr->endereco);


		CONCAT_BUF("ADD BX, %d ;soma o endereco do id indice + posicao = endereco real \n", pesquisarRegistro(id)->simbolo.memoria);


		CONCAT_BUF("MOV BX, DS:[BX] ;move para um registrador o valor na posicao de memoria calculada \n", fator->endereco);


		CONCAT_BUF("MOV DS:[%d], BX ;move o que estiver no endereco real para o temporario \n", fator->endereco);
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
		CONCAT_BUF(";nega expressao\n");
		CONCAT_BUF("MOV AX, DS:[%d]\n", filho->endereco);
		CONCAT_BUF(";expressao not simulada com expressoes aritmeticas\n");
		CONCAT_BUF("neg AX\n");
		CONCAT_BUF("ADD AX, 1\n");
		CONCAT_BUF("MOV DS:[%d], AX ;move o resultado para o endereco de memoria\n",pai->endereco);
		CONCAT_BUF(";fim nega expressao\n");
	}

	void fatorGeraMenos(struct Fator *pai, struct Fator *filho)
	{
		/* DEBUGGER E PILHA */
		DEBUGGEN("fatorGeraMenos");

		int tam = (regLex.tipo == TP_Integer || regLex.tipo == TP_Logico) ? TAM_INT : TAM_CHA;

		pai->endereco = novoTemp(tam);
		CONCAT_BUF("; - (exp)\n");
		CONCAT_BUF("MOV AX, DS:[%d]\n", filho->endereco);
		
		CONCAT_BUF("neg AX ;negacao aritmetica \n");
		CONCAT_BUF("MOV DS:[%d], AX\n",pai->endereco);
		CONCAT_BUF(";fim - (exp)\n");
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

		CONCAT_BUF("; inicio operacoes\n");
		CONCAT_BUF("MOV AX, %d ;operando 1 em AX\n",pai->endereco);
		CONCAT_BUF("MOV BX, %d ;operando 2 em BX\n",filho->endereco);

		char *op;        /* codigo assembly da operacao */
		char *RD = "AX"; /* registrador Destino         */
		char *RO = "BX"; /* registrador origem          */

		switch (pai->op) {
			case Vezes: /* fallthrough */
			case And: 
						op = "IMUL";
						RO = "AX";
						aritmeticos(op,RD,RO,pai);
						break;

			case Barra: op = "IDIV";
						RO = "AX";
						aritmeticos(op,RD,RO,pai);
						break;

			case Porcento: 
						op = "IDIV";
						RO = "DX";
						aritmeticos(op,RD,RO,pai);
						break;

			case Or:    /* fallthrough */
			case Mais: 
						op = "ADD";
						aritmeticos(op, RD, RO, pai);
						break;

			case Menos:
						op = "SUB";
						aritmeticos(op, RD, RO, pai);
						break;

			case Igual:
						op = "JE";
						if (pai->tipo == TP_Char)
							compChar(pai);
						else
							comp(op, pai);

						break;

			case Diferente:
						op = "JNE";
						comp(op, pai);
						break;

			case Maior: 
						op = "JG";
						comp(op, pai);
						break;

			case Menor:
						op = "JL";
						comp(op, pai);
						break;

			case MaiorIgual:
						op = "JGE";
						comp(op, pai);
						break;

			case MenorIgual:
						op = "JLE";
						comp(op, pai);
						break;

		}
		CONCAT_BUF("; fim operacao\n");

	}

#endif 
