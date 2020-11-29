/* **************************************************************************
 * Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 * Este programa segue o estilo de código definido em: 
 * https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 *
 * Arquivo LC:
 * Arquivo principal do compilador. Lê os parâmetros da linha de comando e
 * inicia as estruturas de dados e processo de compilação.
 *
 * *************************************************************************/

/* HEADERS */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "types.h"
#include "ts.h"
#include "LC.h"
#include "pilha.c"
#include "utils.c"
#include "testes.c"
#include "lexan.c"
#include "codegen.c"
#include "ansin.c"

/* VARIAVEIS GLOBAIS */
FILE *prog_fonte;
char *fonte_nome;
FILE *prog_asm;
char *asm_nome;

int not_eof = 1;
int erro = 0;
int linha = 0; /*linha do arquivo*/
int estado_sin = 0; /* estado de aceitacao ou nao do analisador sintatico */

char letra; /*posicao da proxima letra a ser lida no arquivo*/
char *erro_msg; /*Mensagem de erro a ser exibida*/
char *lexema_lido; /* lexema lido sem transformar em minusculo */

struct pilha_d *pilha;
struct registro_lex reg_lex; 
struct Celula *tabela_simbolos[TAM_TBL];

/* DEFINIÇÃO DE FUNÇÕES */

/* pára o programa e reporta o erro */
void
abortar(void)
{
#ifdef DEBUG_SIN
	print_pilha(pilha);
#endif
#ifdef DEBUG_TS
	mostrar_tabela_simbolos();
#endif

	/* remove o arquivo pois o codigo gerado eh invalido */
	remove(asm_nome);

	switch(erro) {
		case ER_LEX:
			printf("%d\n%s [%c].\n", linha+1, erro_msg, letra);
			break;

		case ER_SIN:            /* FALLTHROUGH */
		case ER_LEX_N_ID:       /* FALLTHROUGH */
		case ER_SIN_NDEC:       /* FALLTHROUGH */
		case ER_SIN_JADEC:      /* FALLTHROUGH */
		case ER_SIN_C_INC:
			printf("%d\n%s [%s].\n", linha+1, erro_msg, remove_branco(removeComentario(lexema_lido)));
			break;

		case ER_LEX_INVD:        /* FALLTHROUGH */
		case ER_LEX_EOF:         /* FALLTHROUGH */
		case ER_SIN_EOF:         /* FALLTHROUGH */
		case ER_SIN_TAMVET:      /* FALLTHROUGH */
		case ER_SIN_T_INC:
			printf("%d\n%s.\n", linha+1, erro_msg);
			break;
	}
	exit(-erro);
}

/* escreve o que estiver no buffer em prog_asm e
 * reporta linhas compiladas
 */
void
sucesso(void)
{
	flush();
	printf("%d linhas compiladas.\n", linha-1);
}


int
main(int argc, char *argv[])
{
	char c;

	while((c = getopt(argc,argv,"f:o:")) != -1) {
		switch(c) {
			case 'f':
				fonte_nome = optarg;
				prog_fonte = fopen(fonte_nome, "r");
				break;
			case 'o':
				asm_nome = optarg;
				prog_asm = fopen(asm_nome,"w");
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

	if (prog_fonte == NULL || prog_asm == NULL) {
		printf("Parametros não especificados\nUso: LC -f <programa fonte> -o <arquivo de saída>\n");
		return 1;
	}


	pilha = init_pilha();
	testesTabelaSimbolos();

	iniciar_codegen();
	iniciar_tabela();
	iniciar_ansin();

	return SUCESSO;
}
