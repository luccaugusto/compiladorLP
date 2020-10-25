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

#include "utils.c"
#include "ts.c"
#include "LC.h"
#include "testes.c"
#include "lexan.c"
#include "ansin.c"

/* DEFINIÇÃO DE FUNÇÕES */

/*
 * pára o programa e reporta o erro
 */
void abortar(void)
{
	if (DEBUG_SIN) printPilha(pilha);
	switch(erro) {
		case ERRO_LEXICO: 
			printf("%d\n%s [%c].\n", linha+1, erroMsg, letra);
			break;

		case ERRO_SINTATICO:              /* Fallthrough */
		case ERRO_LEXICO_N_ID:            /* Fallthrough */
		case ERRO_SINTATICO_N_DECL:       /* Fallthrough */
		case ERRO_SINTATICO_JA_DECL:      /* Fallthrough */
		case ERRO_SINTATICO_CL_INCOMP:
			printf("%d\n%s [%s].\n", linha+1, erroMsg, lexemaLido);
			break;

		case ERRO_LEXICO_INV:             /* Fallthrough */
		case ERRO_LEXICO_EOF:             /* Fallthrough */
		case ERRO_SINTATICO_EOF:          /* Fallthrough */
		case ERRO_SINTATICO_TAM_VET:      /* Fallthrough */
		case ERRO_SINTATICO_TP_INCOMP:
			printf("%d\n%s.\n", linha+1, erroMsg);
			break;
	}
	exit(erro);
}

/* pára o programa e reporta linhas compiladas */
void sucesso(void)
{
	printf("%d linhas compiladas.\n", linha-1);
	exit(SUCESSO);
}


int main(int argc, char *argv[])
{
	char c;

	while((c = getopt(argc,argv,"f:o:")) != -1) {
		switch(c) {
			case 'f':
				progFonte = fopen(optarg, "r");
				break;
			case 'o':
				progAsm = fopen(optarg,"w");
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

	/*testesTabelaSimbolos();*/
	inicializarTabela();

    /*mostrarTabelaSimbolos();*/
	/* testeLexan();*/
    
	pilha = initPilha();

	iniciarAnSin();


	return 0;

}
