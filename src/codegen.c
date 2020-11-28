/* **************************************************************************
 * Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 * Este programa segue o estilo de código definido em: 
 * https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 *
 * Arquivo codegen:
 * Este arquivo contém funções responsáveis pela geração de código.
 * Existem funções que gerenciam variáveis de contadores de memória 
 * e rótulos e funções que de fato geram código assembly.
 *
 * *************************************************************************/

/* Geração de código 
 * As funções que geram código de fato escrevem esse código em buffer, e
 * somente quando buffer atinge MAX_BUF_SIZE ocorre uma escrita em arquivo.
 */

/* HEADERS */
#include "codegen.h"

/* VARIAVEIS GLOBAIS */
char *buffer;       /* buffer de criacao do codigo asm     */
char *aux;          /* buffer auxiliar para criacao do asm */
int CD = CD_INICIAL;/* contator de dados em hexadecimal    */
int TP = TP_INICIAL;/* contador de temporários             */
rot RT = 1;         /* contador de rotulos                 */

/* declara novo temporario */
int
novo_temp(int t)
{
	TP += t;
	return (TP-t);
}

int
novo_rot(void)
{
	return RT++;
}

void
zera_temp(void)
{
	TP = TP_INICIAL;
}

/* inicia o buffer */
void
iniciar_codegen(void)
{ 
	/* DEBUGGER E PILHA */
	DEBUGGEN("iniciar_codegen");

	buffer = (char *)malloc(sizeof(char) * MAX_BUF_SIZE);
	aux = (char *)malloc(sizeof(char) * MAX_AUX_SIZE);

	/* Pilha */
	CONCAT_BUF("SSEG SEGMENT STACK\t\t\t\t\t\t\t\t\t\t\t;inicio seg. pilha\n");
	CONCAT_BUF("\tbyte %d DUP(?)\t\t\t\t\t\t\t\t\t\t;dimensiona pilha\n",CD);
	CONCAT_BUF("SSEG ENDS\t\t\t\t\t\t\t\t\t\t\t\t\t;fim seg. pilha\n");

}

/* operacoes aritmeticas, ADD, SUB, IMUL e IDIV
 * recebe a operacao op, o registrador destino RD, o registrador origem RO
 * o registrador resultado RR e o pai
 */
void
aritmeticos(char* op, char *RD, char *RO, char *RR, struct Fator *pai)
{
	pai->endereco = novo_temp(TAM_INT);

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;================inicio de operacao %s===================\n", op);
	if (op == "IMUL" || op == "IDIV") {

		CONCAT_BUF("\t%s %s\n",op, RO);

	} else if (op == "ADD" || op == "SUB") {

		CONCAT_BUF("\t%s %s, %s\n",op, RD, RO);

	}

	CONCAT_BUF("\tMOV DS:[%d], %s\t\t\t\t\t\t\t;grava o resultado da operacao no endereco\n", pai->endereco, RR);
}

/* comparacoes nao string */
void
comp(char *op, struct Fator *pai)
{
	rot verdadeiro = novo_rot();
	rot falso = novo_rot();

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;================inicio de comparacao %s===================\n", op);
	CONCAT_BUF("\tCMP AX, BX\n");
	CONCAT_BUF("\t%s R%d\t\t\t\t\t\t\t\t;comparacao verdadeiro\n", op, verdadeiro);
	CONCAT_BUF("\tMOV AX, 0\n");
	CONCAT_BUF("\tJMP R%d\t\t\t\t\t\t\t\t;comparacao falso\n",falso);

	CONCAT_BUF("R%d:\t\t\t\t\t\t\t\t;verdadeiro\n",verdadeiro);
	CONCAT_BUF("\tMOV AX, 1\n");
	CONCAT_BUF("R%d:\t\t\t\t\t\t\t\t;falso\n",falso);

	pai->endereco = novo_temp(TAM_INT);
	pai->tipo = TP_Logico;

	CONCAT_BUF("MOV DS:[%d], AX\t\t\t\t\t\t\t\t;guarda no endereco o resultado da expressao\n", pai->endereco);
}

void
comp_char(struct Fator *pai)
{
	rot inicio = novo_rot();
	rot verdadeiro = novo_rot();
	rot falso = novo_rot();
	rot fimStr = novo_rot();
	rot iguais = novo_rot();

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;================inicio de comparacao de string===================\n");
	CONCAT_BUF("R%d:\t\t\t\t\t\t\t\t;marca o inicio do loop\n",inicio);
	CONCAT_BUF("\tMOV CL, DS:[BX] \t\t;move para CL o caractere da string em BX\n");

	CONCAT_BUF("\tMOV DX, BX\t\t\t\t\t;Troca AX de lugar com BX\n");
	CONCAT_BUF("\tMOV BX, AX\t\t\t\t\t;pois so BX pode ser utilizado\n");
	CONCAT_BUF("\tMOV AX, DX\t\t\t\t\t;para acessar memoria\n");

	CONCAT_BUF("\tMOV CH, DS:[BX] \t\t;move para CH o caractere da string em BX\n");
	CONCAT_BUF("\tCMP CH, CL\t\t\t\t\t\t\t\t;compara as strings\n");
	CONCAT_BUF("\tJE R%d\t\t\t\t\t\t\t\t;jmp verdadeiro\n", verdadeiro);
	CONCAT_BUF("\tMOV AX, 0\n");
	CONCAT_BUF("\tJMP R%d\t\t\t\t\t\t\t\t;strings diferentes\n",fimStr);
	CONCAT_BUF("R%d:\t\t\t\t\t\t\t\t;caracteres iguais\n",verdadeiro);
	CONCAT_BUF("\tCMP CH, 24h\t\t\t\t\t\t\t\t;verifica se chegou no final da primeira string\n");
	CONCAT_BUF("\tJE R%d\t\t\t\t\t\t\t\t;iguais\n",iguais);
	CONCAT_BUF("\tCMP CL, 24h\t\t\t\t\t\t\t\t;verifica se chegou no final da segunda string\n");
	CONCAT_BUF("\tJE R%\t\t\t\t\t\t\t\t;iguais\n",iguais);
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t;================ nao chegou fim de nenhuma string===================\n");
	CONCAT_BUF("\tADD AX, 1\t\t\t\t\t\t\t\t;anda uma posicao no primeiro string\n");
	CONCAT_BUF("\tADD BX, 1\t\t\t\t\t\t\t\t;anda uma posicao no segundo string\n");

	CONCAT_BUF("\tJMP R%d\t\t\t\t\t\t\t\t;volta ao inicio do loop\n", inicio);

	CONCAT_BUF("R%d:\t\t\t\t\t\t\t\t;strings iguais\n",iguais);
	CONCAT_BUF("\tMOV AX, 1\n");

	CONCAT_BUF("R%d:\t\t\t\t\t\t\t\t;fim de string\n", fimStr);

	pai->endereco = novo_temp(TAM_INT);
	pai->tipo = TP_Logico;

	CONCAT_BUF("\tMOV DS:[%d], AX\t\t\t\t\t\t\t\t;salva no endereco o resultado\n", pai->endereco);
}

/* move cursor para linha de baixo */
void
prox_linha(void)
{
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;================gera quebra de linha===================\n");
	CONCAT_BUF("\tMOV AH, 02h\n");
	CONCAT_BUF("\tMOV DL, 0Dh\n");
	CONCAT_BUF("\tINT 21h\n");
	CONCAT_BUF("\tMOV DL, 0Ah\n");
	CONCAT_BUF("\tINT 21h\n");
}


/* concatena garantindo que o ultimo caractere eh o \n */
void
buf_concatenar(void)
{

	int buf_size = strlen(buffer);


	/* se o buffer vai encher, escreve no arquivo e esvazia */
	if ((buf_size+strlen(aux)) >= MAX_BUF_SIZE)
		flush();

	buffer = concatenar(buffer, aux);
	buf_size = strlen(buffer);

	/* limpa aux */
	free(aux);
	aux = (char *)malloc(sizeof(char) * MAX_AUX_SIZE);
	aux[0] = '\0';
}

/* escreve buffer no arquivo prog_asm 
 * e em seguida limpa buffer
 */
void
flush(void)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("flush");

	fprintf(prog_asm, "%s",buffer);

	/* limpa o buffer */
	buffer[0] = '\0';
}

/* inicia o bloco de declaracoes asm */
void
init_declaracao(void)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("init_declaracao");

	/* dados */
	CONCAT_BUF("dseg SEGMENT PUBLIC\t\t\t\t\t\t\t\t\t\t\t;inicio seg. dados\n");
	CONCAT_BUF("\tbyte %d DUP(?)\t\t\t\t\t\t\t\t\t\t;temporarios\n",CD);
}

/* finaliza o bloco de declaracoes asm 
 * e inicia o bloco de comandos asm
 */
void
fim_dec_init_com(void)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("fim_dec_init_com");

	/* fim declaracao */
	CONCAT_BUF("DSEG ENDS\t\t\t\t\t\t\t\t\t\t\t\t\t;fim seg. dados\n");

	/* adiciona o offset da posicao de memória
	 * por algum motivo o dosbox está alocando memória 0x4100 posições
	 * depois do CD
	 */
	CD+=OFFSET;

	/* comandos */
	CONCAT_BUF("CSEG SEGMENT PUBLIC\t\t\t\t\t\t\t\t\t\t\t;inicio seg. codigo\n");
	CONCAT_BUF("\tASSUME CS:CSEG, DS: DSEG\n");
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;================inicio do programa===================\nSTRT:\n");

}

void
fim_comandos(void)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("fim_comandos");

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;================interrompe o programa===================\n");
	CONCAT_BUF("\tMOV AH, 4Ch\n");
	CONCAT_BUF("\tINT 21h\n");
	CONCAT_BUF("\tCSEG ENDS\t\t\t\t\t\t\t\t\t;fim seg. codigo\n");
	CONCAT_BUF("END STRT\t\t\t\t\t\t\t\t\t\t;fim programa\n");
}

/* gera codigo para acesso a array */
void
acesso_array(struct Fator *pai, struct Fator *filho)
{
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;================acesso a array===================\n");
	CONCAT_BUF("\tMOV AX, %d \t\t\t\t\t\t;endereco base\n", pai->endereco);
	/* adiciona o resultado da expressao ao inicio do array para encontrar a posicao de acesso */
	CONCAT_BUF("\tADD AX, DS:[%d] \t\t\t\t\t\t\t\t;soma com offset\n", filho->endereco);
	CONCAT_BUF("\tfim acesso a array");
}

/* gera o asm da declaracao de uma variavel ou constante 
 * e retorna o endereco que foi alocado 
 */
void
gen_declaracao(Tipo t, Classe c, int tam, char *val, int negativo)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("gen_declaracao");

	char *tipo;  /* string de tipo        */
	char *classe;/* const ou var          */
	char *nome;  /* string sword ou byte  */
	char *valor; /* string de valor ou ?  */
	int n_bytes; /* numero de bytes usado */

	/* marca o endereco de memoria, tipo e tamanho na tabela de simbolos */
	reg_lex.endereco->simbolo.memoria = CD+OFFSET; /* offset para acessar a posicao correta de memoria, por que eu nao sei */
	reg_lex.endereco->simbolo.tipo = t;
	reg_lex.endereco->simbolo.tamanho = tam;

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

	if (c == CL_Const)
		classe = "const";
	else
		classe = "var";

	/* string de valor se existir */
	if (val != NULL) {

		valor = val;

		if (negativo)
			valor = concatenar("-",valor);

		/* adiciona $ ao fim da string */
		if (t == TP_Char) {
			valor = concatenar(remove_aspas(valor),"$");
			valor = concatenar("\"",valor);
			valor = concatenar(valor,"\"");
		}

	} else {
		valor = "?";
	}

	/* arrays */
	if (tam > 1) {
		CONCAT_BUF("\t%s %d DUP(?)\t\t\t\t\t\t\t\t\t;var. Vet %s. em %d\n", nome, tam, tipo, reg_lex.endereco->simbolo.memoria);
	} else {
		CONCAT_BUF("\t%s %s\t\t\t\t\t\t\t\t\t\t\t\t; %s. %s. em %d\n", nome, valor, classe, tipo, reg_lex.endereco->simbolo.memoria);
	}

	/* incrementa a posicao de memoria com o numero de bytes utilizado */
	CD+=n_bytes;
}

void
atribuicao_string(int end1, int end2, int fimStr, int tamMax)
{
	rot inicio = novo_rot();
	rot fim = novo_rot();

	/* trata tamanho invalido */ 
	tamMax = tamMax == 0 ? 255 : tamMax;

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================atribuicao de strings===================\n");
	CONCAT_BUF("\tMOV BX, %d \t\t\t\t\t\t\t\t\t;endereco da string para bx\n", end1);
	CONCAT_BUF("\tMOV DI, %d \t\t\t\t\t\t\t\t;endereco do pai para concatenar\n", end2);

	CONCAT_BUF("R%d: \t\t\t\t\t\t\t\t\t\t;inicio do loop\n", inicio);
	CONCAT_BUF("\tMOV CH, 0 \t\t\t\t\t\t\t\t;0 no registrador alto\n");
	CONCAT_BUF("\tMOV CL, DS:[BX] \t\t\t\t\t\t\t;joga caractere em CL\n");
	CONCAT_BUF("\tMOV DS:[DI], CL \t\t\t\t\t\t\t;transfere pro endereco a string\n");
	CONCAT_BUF("\tCMP CL, %d \t\t\t\t\t\t\t\t\t;verifica se chegou no fim\n", fimStr);
	CONCAT_BUF("\tJE R%d \t\t\t\t\t\t\t\t\t\t;fim da str\n", fim);
	CONCAT_BUF("\tCMP BX, %d \t\t\t\t\t\t\t\t\t;verifica se atingiu o tamanho maximo\n", tamMax);
	CONCAT_BUF("\tJE R%d \t\t\t\t\t\t\t\t\t\t;fim da str\n", fim);
	CONCAT_BUF("\tADD DI, 1 \t\t\t\t\t\t\t\t\t;avanca posicao a receber o proximo caractere\n");
	CONCAT_BUF("\tADD BX, 1 \t\t\t\t\t\t\t\t\t;proximo caractere\n");
	CONCAT_BUF("\tJMP R%d\n", inicio);
	CONCAT_BUF("R%d:\n", fim);
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================fim atribuicao de strings===================\n");

}

void
gen_atribuicao(struct Fator *pai, struct Fator *fator)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("gen_atribuicao");

	/* int e logico */
	if (pai->tipo == TP_Integer || pai->tipo == TP_Logico) {
		CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================atribuicao de inteiros e logicos===================\n");
		CONCAT_BUF("\tMOV AX, DS:[%d] \t\t\t\t\t\t\t;endereco do resultado para ax\n", fator->endereco);
		CONCAT_BUF("\tMOV DS:[%d], AX \t\t\t\t\t\t;ax para posicao de memoria da variavel\n", pai->endereco);
		CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================fim atribuicao de inteiros e logicos===================\n");
	} else {
		/* string, move caractere por caractere */
		atribuicao_string(fator->endereco, pai->endereco, 0x24, (pai->tamanho == 0 ? 255 : pai->tamanho));
	}
}

/* segunda acao do for
 * gera a declaracao e o fim
 * for ID=EXP TO EXP
 */
void
gen_repeticao(struct Fator *pai, struct Fator *filho, rot inicio, rot fim)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("gen_repeticao");

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================inicio da repeticao===================\n");
	CONCAT_BUF("R%d: \t\t\t\t\t\t\t\t\t\t;inicio do loop \n", inicio);
	CONCAT_BUF("\tMOV CX, DS:[%d] \t\t\t\t\t\t;move o valor de ID para cx \n",pai->endereco);
	CONCAT_BUF("\tMOV BX, DS:[%d] \t\t\t\t\t\t;move o resultado da TO EXP para BX \n", filho->endereco);
	CONCAT_BUF("\tCMP CX, BX \t\t\t\t\t\t\t\t\t;compara os valores \n");
	CONCAT_BUF("\tJG R%d \t\t\t\t\t\t\t\t\t\t;vai para o fim se id > exp \n", fim);
}

/* fim do loop de repeticao 
 * incrementa e desvia
 */
void
gen_fim_repeticao(struct Fator *pai, rot inicio, rot fim, struct Fator *step)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("gen_fim_repeticao");

	int end;
	char *val_step = "1";

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================fim da repeticao===================\n");
	CONCAT_BUF("\tMOV CX, DS:[%d] \t\t\t\t\t\t;move o valor de ID para cx \n",pai->endereco);

	/* se nao for o step padrao 
	 * coloca o valor da exp em BX
	 */
	if (step != NULL) {
		CONCAT_BUF("\tMOV BX, DS:[%d]\n",step->endereco);
		val_step = "BX";
	}

	CONCAT_BUF("\tADD CX, %s \t\t\t\t\t\t\t\t\t;soma o step\n", val_step);
	CONCAT_BUF("\tMOV DS:[%d], CX \t\t\t\t\t\t;guarda o valor de id \n", pai->endereco);
	CONCAT_BUF("\tJMP R%d \t\t\t\t\t\t\t\t\t\t;volta para o inicio\n", inicio);
	CONCAT_BUF("R%d: \t\t\t\t\t\t\t\t\t\t;fim do loop\n", fim);
}

/* gera o inicio do comando de teste */
void
gen_teste(struct Fator *filho, rot falso, rot fim)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("gen_teste");

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================inicio do teste===================\n");
	CONCAT_BUF("\tMOV AX, DS:[%d] \t\t\t\t\t\t;move para ax o resultado da expressao logica\n", filho->endereco);
	CONCAT_BUF("\tCMP AX, 0 \t\t\t\t\t\t\t\t;checa se eh falso \n");
	CONCAT_BUF("\tJE R%d \t\t\t\t\t\t\t\t;vai para falso \n", falso);
}

/* gera a parte do else, que pode ser vazia */
void
gen_else_teste(rot falso, rot fim)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("gen_else_teste");

	/* else */
	CONCAT_BUF("\tJMP R%d \t\t\t\t\t\t\t\t;fim verdadeiro\n",fim);
	CONCAT_BUF("R%d: \t\t\t\t\t\t\t\t;inicio else\n",falso);
}

/* gera o rotulo de fim do teste */
void
gen_fim_teste(rot fim)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("gen_fim_teste");

	CONCAT_BUF("R%d: \t\t\t\t\t\t\t\t;fim do teste\n", fim);
}

void
gen_entrada(struct Fator *pai)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("gen_entrada");

	int tam;

	/* trata tamanhos 0 
	 * e inteiros sempre tem tamanho 255 */
	if (pai->tipo == TP_Char)
		tam = (pai->tamanho < 1 ? 255 : (pai->tamanho + 3));
	else
		tam = 255;

	/* buffer para entrada do teclado, tam max = 255 */
	int buffer = novo_temp(tam);
	rot inicio = novo_rot();
	rot meio = novo_rot();
	rot fim = novo_rot();

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================inicio do comando de entrada===================\n");
	CONCAT_BUF("\tMOV DX, %d \t\t\t\t\t\t\t\t\t;buffer para receber a string\n", buffer);
	CONCAT_BUF("\tMOV AX, %d \t\t\t\t\t\t\t\t\t;tamanho do buffer \n", tam);
	CONCAT_BUF("\tMOV DS:[%d], AX \t\t\t\t\t\t\t\t;move tamanho do buffer para a primeira posicao do buffer\n",buffer);
	CONCAT_BUF("\tMOV AH, 0Ah \t\t\t\t\t\t\t\t;interrupcao para leitura do teclado\n");
	CONCAT_BUF("\tINT 21h\n");
	CONCAT_BUF("\tMOV CH, 0\n");
	CONCAT_BUF("\tMOV CL, DS:[%d] \t\t\t\t\t\t\t\t;numero de caracteres lidos fica na segunda posicao do buffer\n", buffer+1);
	CONCAT_BUF("\tMOV DX, 0 \t\t\t\t\t\t\t\t\t;contador de posicoes\n");
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;troca o enter por '$'\n");
	CONCAT_BUF("\tMOV BX, CX\t\t\t\t\t\t\t\t\t\t;coloca em BX o numero de caracteres lidos\n");
	CONCAT_BUF("\tADD BX, %d\t\t\t\t\t\t\t\t\t\t;adiciona o endereco+2 para chegar na posicao da quebra de linha\n", buffer+2);
	CONCAT_BUF("\tMOV DL, 24h \t\t\t\t\t\t\t\t;fim de string\n");
	CONCAT_BUF("\tMOV DS:[BX], DL \t\t\t\t\t\t\t;grava '$'\n");

	prox_linha(); /* proxima linha */

	/* string */
	if (pai->tipo == TP_Char) {

		CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================entrada de strings===================\n");
		CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== transfere para o endereco do pai o conteudo lido===================\n");

		atribuicao_string(buffer+2, pai->endereco, 0x0A, tam-3);

		CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================fim entrada de strings===================\n");
	}

	/* inteiro */
	else if (pai->tipo == TP_Integer) {
		CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== inicio entrada de inteiros===================\n");
		CONCAT_BUF("\tMOV DI, %d \t\t\t\t\t\t\t\t;posição do string\n",buffer+2);
		CONCAT_BUF("\tMOV AX, 0 \t\t\t\t\t\t\t\t\t;acumulador\n");
		CONCAT_BUF("\tMOV CX, 10 \t\t\t\t\t\t\t\t\t;base decimal\n");
		CONCAT_BUF("\tMOV DX, 1 \t\t\t\t\t\t\t\t\t;valor sinal +\n");
		CONCAT_BUF("\tMOV BH, 0\n");
		CONCAT_BUF("\tMOV BL, DS:[DI] \t\t\t\t\t\t\t;caractere\n");
		CONCAT_BUF("\tCMP BX, 2Dh \t\t\t\t\t\t\t\t;verifica sinal\n");
		CONCAT_BUF("\tJNE R%d \t\t\t\t\t\t\t\t\t\t;se não negativo jmp inicio\n", inicio);
		CONCAT_BUF("\tMOV DX, -1 \t\t\t\t\t\t\t\t\t;valor sinal -\n");
		CONCAT_BUF("\tADD DI, 1 \t\t\t\t\t\t\t\t\t;incrementa base\n");
		CONCAT_BUF("\tMOV BL, DS:[DI] \t\t\t\t\t\t\t;próximo caractere\n");
		CONCAT_BUF("R%d: \t\t\t\t\t\t\t\t\t\t;inicio\n",inicio);
		CONCAT_BUF("\tPUSH DX \t\t\t\t\t\t\t\t\t;empilha sinal\n");
		CONCAT_BUF("\tMOV DX, 0 \t\t\t\t\t\t\t\t\t;reg. multiplicação\n");
		CONCAT_BUF("R%d: \t\t\t\t\t\t\t\t\t\t;meio\n",meio);
		CONCAT_BUF("\tCMP BX, 24h \t\t\t\t\t\t\t\t;verifica fim string\n");
		CONCAT_BUF("\tJE R%d \t\t\t\t\t\t\t\t\t\t;salta fim se fim string\n",fim);
		CONCAT_BUF("\tIMUL CX \t\t\t\t\t\t\t\t\t;mult. 10\n");
		CONCAT_BUF("\tADD BX, -48 \t\t\t\t\t\t\t\t;converte caractere\n");
		CONCAT_BUF("\tADD AX, BX \t\t\t\t\t\t\t\t\t;soma valor caractere\n");
		CONCAT_BUF("\tADD DI, 1 \t\t\t\t\t\t\t\t\t;incrementa base\n");
		CONCAT_BUF("\tMOV BH, 0\n");
		CONCAT_BUF("\tMOV BL, DS:[DI] \t\t\t\t\t\t\t;próximo caractere\n");
		CONCAT_BUF("\tJMP R%d \t\t\t\t\t\t\t\t\t\t;jmp meio\n",meio);
		CONCAT_BUF("R%d: \t\t\t\t\t\t\t\t\t\t;fim\n", fim);
		CONCAT_BUF("\tPOP CX \t\t\t\t\t\t\t\t\t\t;desempilha sinal\n");
		CONCAT_BUF("\tIMUL CX \t\t\t\t\t\t\t\t\t;mult. sinal\n");

		CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== transfere resultado para o pai===================\n");
		CONCAT_BUF("\tMOV BX, %d\n", pai->endereco);
		CONCAT_BUF("\tMOV DS:[BX], AX\n");
		CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== fim entrada de inteiros===================\n");
	}

}

/* geracao de codigo para saida de texto */
void
gen_saida(struct Fator *pai, int ln)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("gen_saida");

	int endereco = pai->endereco;
	rot inicio = novo_rot();
	rot meio = novo_rot();
	rot fim = novo_rot();

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================inicio saida===================\n");
	/* conversao de inteiro para string */
	if (pai->tipo == TP_Integer) {
		endereco = novo_temp(pai->tamanho);

		CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================saida de inteiros===================\n");
		CONCAT_BUF("\tMOV AX, DS:[%d]\t\t\t\t\t\t\t;carrega para AX o valor\n", pai->endereco);
		CONCAT_BUF("\tMOV DI, %d \t\t\t\t\t\t\t\t;end. string temp.\n",endereco);
		CONCAT_BUF("\tMOV CX, 0 \t\t\t\t\t\t\t\t\t;contador\n");
		CONCAT_BUF("\tCMP AX, 0 \t\t\t\t\t\t\t\t\t;verifica sinal\n");
		CONCAT_BUF("\tJGE R%d \t\t\t\t\t\t\t\t\t\t;salta se número positivo\n", inicio);
		CONCAT_BUF("\tMOV BL, 2Dh \t\t\t\t\t\t\t\t;senão, escreve sinal –\n");
		CONCAT_BUF("\tMOV DS:[DI], BL\n");
		CONCAT_BUF("\tADD DI, 1 \t\t\t\t\t\t\t\t\t;incrementa índice\n");
		CONCAT_BUF("\tneg AX \t\t\t\t\t\t\t\t\t\t;toma módulo do número\n");
		CONCAT_BUF("R%d:\n",inicio);
		CONCAT_BUF("\tMOV BX, 10 \t\t\t\t\t\t\t\t\t;divisor\n");
		CONCAT_BUF("R%d:\n",meio);
		CONCAT_BUF("\tADD CX, 1 \t\t\t\t\t\t\t\t\t;incrementa contador\n");
		CONCAT_BUF("\tMOV DX, 0 \t\t\t\t\t\t\t\t\t;estende 32bits p/ div.\n");
		CONCAT_BUF("\tIDIV BX \t\t\t\t\t\t\t\t\t;divide DXAX por BX\n");
		CONCAT_BUF("\tPUSH DX \t\t\t\t\t\t\t\t\t;empilha valor do resto\n");
		CONCAT_BUF("\tCMP AX, 0 \t\t\t\t\t\t\t\t\t;verifica se quoc. é 0\n");
		CONCAT_BUF("\tJNE R%d \t\t\t\t\t\t\t\t\t\t;se não é 0, continua\n", meio);
		CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================agora, desemp. os valores e escreve o string===================\n");
		CONCAT_BUF("R%d:\n",fim);
		CONCAT_BUF("\tPOP DX \t\t\t\t\t\t\t\t\t\t;desempilha valor\n");
		CONCAT_BUF("\tADD DX, 30h \t\t\t\t\t\t\t\t;transforma em caractere\n");
		CONCAT_BUF("\tMOV DS:[DI], DL \t\t\t\t\t\t\t\t;escreve caractere\n");
		CONCAT_BUF("\tADD DI, 1 \t\t\t\t\t\t\t\t\t;incrementa base\n");
		CONCAT_BUF("\tADD CX, -1 \t\t\t\t\t\t\t\t\t;decrementa contador\n");
		CONCAT_BUF("\tCMP CX, 0 \t\t\t\t\t\t\t\t\t;verifica pilha vazia\n");
		CONCAT_BUF("\tJNE R%d \t\t\t\t\t\t\t\t\t\t;se não pilha vazia, loop\n", fim);
		CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================grava fim de string===================\n");
		CONCAT_BUF("\tMOV DL, 24h \t\t\t\t\t\t\t\t;fim de string\n");
		CONCAT_BUF("\tMOV DS:[DI], DL \t\t\t\t\t\t\t;grava '$'\n");
		CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================fim saida de inteiros===================\n");
	}

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================exibe string===================\n");
	CONCAT_BUF("\tMOV DX, %d\n", endereco);
	CONCAT_BUF("\tMOV AH, 09h\n");
	CONCAT_BUF("\tINT 21h	\n");
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================fim saida===================\n");

	if (ln)
		prox_linha();
}

void
fator_gera_literal(struct Fator *fator, char *val)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("fator_gera_literal");

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================gera literal===================\n");
	/* string */
	if (fator->tipo == TP_Char) {
		A_SEG_PUB
			CONCAT_BUF("\tbyte \"%s$\" \t\t\t\t\t\t\t\t;string %s em %d\n",remove_aspas(val),val,CD);
		F_SEG_PUB
		
		fator->endereco = CD;
		CD += (fator->tamanho+1) * TAM_CHA;

	/* nao string */
	} else {
		fator->endereco = novo_temp(TAM_INT);
		fator->tamanho = 1;
		CONCAT_BUF("\tMOV AX, %s \t\t\t\t\t\t\t\t\t;literal para AX\n",val);
		CONCAT_BUF("\tMOV DS:[%d], AX \t\t\t\t\t\t\t;ax para memoria\n",fator->endereco);
	}
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================fim gera literal===================\n");
}

void
fator_gera_id(struct Fator *fator, char *id)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("fator_gera_id");

	struct Celula *registro = pesquisar_registro(id);

	fator->endereco = registro->simbolo.memoria;
	fator->tipo = registro->simbolo.tipo;
	fator->tamanho = registro->simbolo.tamanho;
}

void
fator_gera_array(struct Fator *fator, struct Fator *expr, char *id)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("fator_gera_array");

	fator->endereco = novo_temp((expr->tipo == TP_Integer) ? TAM_INT : TAM_CHA);

	fator->tamanho = 1;

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================acessar array===================\n");
	
	CONCAT_BUF("\tMOV BX, DS:[%d] \t\t\t\t\t\t;move para BX o endereco da expressao que vai conter o indice \n", expr->endereco);


	if (fator->tipo == TP_Integer)
		CONCAT_BUF("\tADD BX, DS:[%d] \t\t\t\t\t\t\t\t;int usa 2 bytes, portanto contamos a posicao\n", expr->endereco);


	CONCAT_BUF("\tADD BX, %d \t\t\t\t\t\t\t\t;soma o endereco do id indice + posicao = endereco real \n", pesquisar_registro(id)->simbolo.memoria);


	CONCAT_BUF("\tMOV BX, DS:[BX] \t\t\t\t\t\t;move para um registrador o valor na posicao de memoria calculada \n", fator->endereco);


	CONCAT_BUF("\tMOV DS:[%d], BX \t\t\t\t\t\t;move o que estiver no endereco real para o temporario \n", fator->endereco);
}

void
fator_gera_exp(struct Fator *fator, struct Fator *expr)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("fator_gera_exp");

	fator->endereco = expr->endereco;
	fator->tamanho = expr->tamanho;
	fator->tipo = expr->tipo;
}

void
fator_gera_not(struct Fator *pai, struct Fator *filho)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("fator_gera_not");

	int tam = (reg_lex.tipo == TP_Integer || reg_lex.tipo == TP_Logico) ? TAM_INT : TAM_CHA;

	pai->endereco = novo_temp(tam);
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================nega expressao===================\n");
	CONCAT_BUF("\tMOV AX, DS:[%d]\n", filho->endereco);
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================expressao not simulada com expressoes aritmeticas===================\n");
	CONCAT_BUF("\tneg AX\n");
	CONCAT_BUF("\tADD AX, 1\n");
	CONCAT_BUF("\tMOV DS:[%d], AX \t\t\t\t\t\t;move o resultado para o endereco de memoria\n",pai->endereco);
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================fim nega expressao===================\n");
}

void
fator_gera_menos(struct Fator *pai, struct Fator *filho)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("fator_gera_menos");

	pai->endereco = novo_temp(TAM_INT);
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== - (exp)===================\n");
	CONCAT_BUF("\tMOV AX, DS:[%d]\n", filho->endereco);
	
	CONCAT_BUF("\tneg AX \t\t\t\t\t\t\t\t\t\t;negacao aritmetica \n");
	CONCAT_BUF("\tMOV DS:[%d], AX\n",pai->endereco);
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;===================fim - (exp)===================\n");
}

/* repassa dados do filho para o pai */
void
atualiza_pai(struct Fator *pai, struct Fator *filho)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("atualiza_pai");

	pai->endereco = filho->endereco;
	pai->tipo = filho->tipo;
	pai->tamanho = filho->tamanho;
}

/* salva o operador */
void
guarda_op(struct Fator *pai)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("guarda_op");

	pai->op = reg_lex.token;
}

/* operacoes entre termos */
void
gen_op_termos(struct Fator *pai, struct Fator *filho)
{
	/* DEBUGGER E PILHA */
	DEBUGGEN("gen_op_termos");

	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== inicio operacoes===================\n");
	CONCAT_BUF("\tMOV AX, DS:[%d] \t\t\t\t\t\t\t;operando 1 em AX\n",pai->endereco);
	CONCAT_BUF("\tMOV BX, DS:[%d] \t\t\t\t\t\t\t;operando 2 em BX\n",filho->endereco);

	char *op;        /* codigo assembly da operacao */
	char *RD = "AX"; /* registrador Destino         */
	char *RO = "BX"; /* registrador origem          */
	char *RR = "AX"; /* registrador origem          */

	switch (pai->op) {
		case And:  /* FALLTHROUGH */
					CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== And===================\n");
		case Vezes:
					op = "IMUL";
					aritmeticos(op,RD,RO,RR,pai);
					break;

		case Barra: op = "IDIV";
					aritmeticos(op,RD,RO,RR,pai);
					break;

		case Porcento: 
					op = "IDIV";
					RR = "DX";
					aritmeticos(op,RD,RO,RR,pai);
					break;

		case Or:    /* FALLTHROUGH */
					CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== Or===================\n");
		case Mais: 
					op = "ADD";
					aritmeticos(op, RD, RO,RR, pai);
					break;

		case Menos:
					op = "SUB";
					aritmeticos(op, RD, RO,RR, pai);
					break;

		case Igual:
					CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== igual===================\n");
					op = "JE";
					if (pai->tipo == TP_Char)
						comp_char(pai);
					else
						comp(op, pai);

					break;

		case Diferente:
					CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== Diferente===================\n");
					op = "JNE";
					comp(op, pai);
					break;

		case Maior: 
					CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== Maior===================\n");
					op = "JG";
					comp(op, pai);
					break;

		case Menor:
					CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== Menor===================\n");
					op = "JL";
					comp(op, pai);
					break;

		case MaiorIgual:
					CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== MaiorIgual===================\n");
					op = "JGE";
					comp(op, pai);
					break;

		case MenorIgual:
					CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== MenorIgual===================\n");
					op = "JLE";
					comp(op, pai);
					break;

	}
	CONCAT_BUF("\t\t\t\t\t\t\t\t\t\t\t\t;=================== fim operacao===================\n");

}
