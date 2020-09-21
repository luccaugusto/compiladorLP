/* Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 *
 * Este programa segue a maioria das recomendações do estilo de código definido em: https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 */


/* TODO:
 * Juntar tudo num arquivo so substituindo os includes pelo conteudo do arquivo
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* MACROS */
#define TAM_TBL 254
#define SEPARADOR "=-=-=-=-="
#define ERRO_LEXICO -1
#define ERRO_SINTATICO -2
#define ERRO_SINTATICO_EOF -3
#define ACEITACAO_LEX 11
#define ACEITACAO_SIN 11

/* DECLARAÇÕES */
#include "types.h"

#include "LC.h"

/* VARIÁVEIS GLOBAIS */

/* parametros da linha de comando */
FILE *progFonte;
FILE *progAsm;

int linha = 0; /*linha do arquivo*/
int erro = 0;
int estado_sin = 0;
int lex = 1;
char letra; /*posicao da proxima letra a ser lida no arquivo*/
char *erroMsg /*Mensagem de erro a ser exibida*/;
struct registroLex tokenAtual; 
struct Celula *tabelaSimbolos[TAM_TBL];


/* DEFINIÇÃO DE FUNÇÕES */

#include "utils.h"
#include "testes.h"
#include "ts.h"
#include "lexan.h"
#include "ansin.h"

/*
 * pára o programa e reporta o erro
 */
void abortar(void)
{
	switch(erro) {
		case ERRO_LEXICO: 
			printf("%d\n %s [%c] \n", linha, erroMsg, lex);
			break;
		case ERRO_SINTATICO:
			printf("%d\n %s [%s] \n", linha, erroMsg, tokenAtual.lexema);
			break;
	}
	exit(erro);
}

void adicionarReservados(void)
{
	adicionarRegistro("const",Const);
	adicionarRegistro("var",Var);
	adicionarRegistro("integer",Integer);
	adicionarRegistro("char",Char);
	adicionarRegistro("for",For);
	adicionarRegistro("if",If);
	adicionarRegistro("else",Else);
	adicionarRegistro("and",And);
	adicionarRegistro("or",Or);
	adicionarRegistro("not",Not);
	adicionarRegistro("=",Igual);
	adicionarRegistro("to",To);
	adicionarRegistro("(",A_Parenteses);
	adicionarRegistro(")",F_Parenteses);
	adicionarRegistro("<",Menor);
	adicionarRegistro(">",Maior);
	adicionarRegistro("<>",Diferente);
	adicionarRegistro(">=",MaiorIgual);
	adicionarRegistro("<=",MenorIgual);
	adicionarRegistro(",",Virgula);
	adicionarRegistro("+",Mais);
	adicionarRegistro("-",Menos);
	adicionarRegistro("*",Vezes);
	adicionarRegistro("/",Barra);
	adicionarRegistro(";",PtVirgula);
	adicionarRegistro("{",A_Chaves);
	adicionarRegistro("}",F_Chaves);
	adicionarRegistro("then",Then);
	adicionarRegistro("readln",Readln);
	adicionarRegistro("step",Step);
	adicionarRegistro("write",Write);
	adicionarRegistro("writeln",Writeln);
	adicionarRegistro("%",Porcento);
	adicionarRegistro("[",A_Colchete);
	adicionarRegistro("]",F_Colchete);
	adicionarRegistro("do",Do);
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
    
	iniciarAnSin();

	printf("%d linhas compiladas.\n", linha);

	return 0;

}
