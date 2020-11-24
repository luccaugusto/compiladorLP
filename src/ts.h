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
