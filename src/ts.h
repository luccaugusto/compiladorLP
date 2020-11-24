/* **************************************************************************
 * Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 * Este programa segue o estilo de código definido em: 
 * https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 *
 * Arquivo ts:
 * Implementação da tabela de simbolos utilizada pelo compilador.
 * Define os tokens e tipos, e implementa funções para manipular a tabela.
 *
 * *************************************************************************/

#ifndef _TS_H
#define _TS_H

	/* MACROS */
	#define TAM_TBL 254
	//define DEBUG_TS 0

	/* DECLARACAO DE FUNCOES */
	/* Tabela de símbolos */
	Tipo busca_tipo(char *identificador);
	void limpar_tabela(void);
	void iniciar_tabela(void);
	void adicionar_reservados(void);
	void mostrar_tabela_simbolos(void);
	void limpar_lista(struct Celula *cel);
	struct Celula *pesquisar_registro(char *procurado);
	struct Celula *adicionar_registro(char *lexema, Tokens);

	/* VARIAVEIS GLOBAIS */
	extern struct Celula *tabela_simbolos[TAM_TBL];

#endif
