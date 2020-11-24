/* Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 * * Este programa segue a maioria das recomendações do estilo de código definido em: https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "pilha.c"
#include "utils.c"
#include "ts.c"
#include "codegen.c"
#include "testes.c"
#include "lexan.c"
#include "ansin.c"
#include "LC.h"

FILE *progFonte;
char *fonteNome;
FILE *progAsm;
char *asmNome;

/* VARIAVEIS GLOBAIS */
int lex = 1;
int erro = 0;
int lido = 0;
int linha = 0; /*linha do arquivo*/
int estado_sin = 0; /* estado de aceitacao ou nao do analisador sintatico */

char letra; /*posicao da proxima letra a ser lida no arquivo*/
char *erroMsg; /*Mensagem de erro a ser exibida*/
char *lexemaLido; /* lexema lido sem transformar em minusculo */

struct pilha_d *pilha;
struct registroLex regLex; 
struct Celula *tabelaSimbolos[TAM_TBL];

/* DEFINIÇÃO DE FUNÇÕES */

/*
 * pára o programa e reporta o erro
 */
void abortar(void)
{
#ifdef DEBUG_SIN
	printPilha(pilha);
#endif
#ifdef DEBUG_TS
	mostrarTabelaSimbolos();
#endif

	/* remove o arquivo pois o codigo gerado eh invalido */
	remove(asmNome);

	switch(erro) {
		case ER_LEX:
			printf("%d\n%s [%c].\n", linha+1, erroMsg, letra);
			break;

		case ER_SIN:            /* Fallthrough */
		case ER_LEX_N_ID:       /* Fallthrough */
		case ER_SIN_NDEC:       /* Fallthrough */
		case ER_SIN_JADEC:      /* Fallthrough */
		case ER_SIN_C_INC:
			printf("%d\n%s [%s].\n", linha+1, erroMsg, removeBranco(removeComentario(lexemaLido)));
			break;

		case ER_LEX_INVD:        /* Fallthrough */
		case ER_LEX_EOF:         /* Fallthrough */
		case ER_SIN_EOF:         /* Fallthrough */
		case ER_SIN_TAMVET:      /* Fallthrough */
		case ER_SIN_T_INC:
			printf("%d\n%s.\n", linha+1, erroMsg);
			break;
	}
	exit(erro);
}

/* escreve o que estiver no buffer em progAsm e
 * reporta linhas compiladas
 */
void sucesso(void)
{
	flush();
	printf("%d linhas compiladas.\n", linha-1);
}


int main(int argc, char *argv[])
{
	char c;

	while((c = getopt(argc,argv,"f:o:")) != -1) {
		switch(c) {
			case 'f':
				fonteNome = optarg;
				progFonte = fopen(fonteNome, "r");
				break;
			case 'o':
				asmNome = optarg;
				progAsm = fopen(asmNome,"w");
				break;
			case '?':
				if (optopt == 'f')
					printf("Programa fonte não especificado.");
				else if (optopt == 'o')
					printf("Arquivo de saída não especificado.");
				else
					printf("Parametros não especificados");
			default:
					printf("\nUso: LC -f <programa fonte> -o <arquivo de saída>\n");
					return 1;
		}
	}

	if (progFonte == NULL || progAsm == NULL) {
		printf("Parametros não especificados\nUso: LC -f <programa fonte> -o <arquivo de saída>\n");
		return 1;
	}


	pilha = initPilha();

	iniciarCodegen();
	inicializarTabela();
	iniciarAnSin();

	return SUCESSO;
}
