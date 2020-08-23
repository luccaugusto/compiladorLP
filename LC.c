#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

 /*parametros da linha de comando*/
char *prog_fonte;
char *prog_asm;

struct simb_info
{
	char nome[50];
	char tipo[7];
	int mem; /*local na memória*/
};

struct celula
{
	struct celula *prox;
	struct simb_info simbolo;
};

struct celula tabela_simbolos[200];


int main(int argc, char *argv[])
{
	char c;

	while((c = getopt(argc,argv,"f:o:")) != -1) {
		switch(c) {
			case 'f':
				prog_fonte = optarg;
				break;
			case 'o':
				prog_asm = optarg;
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

	printf("%s; %s\n",prog_fonte,prog_asm);

	return 0;
}
