/* Implementacao da tabela de simbolos */
#ifndef _TS
#define _TS
	
	#include "types.h"
	#include "utils.c"

	#define TAM_TBL 254
	//define DEBUG_TS 0

	/* Tabela de símbolos */
	Tipo busca_tipo(char *identificador);
	void limpar_tabela(void);
	void iniciar_tabela(void);
	void adicionar_reservados(void);
	void mostrar_tabela_simbolos(void);
	void limpar_lista(struct Celula *cel);
	struct Celula *pesquisar_registro(char *procurado);
	struct Celula *adicionar_registro(char *lexema, Tokens);

	extern struct Celula *tabela_simbolos[TAM_TBL];
	
	/* retorna o tipo do identificador */
	Tipo
	busca_tipo(char *identificador)
	{
		return pesquisar_registro(identificador)->simbolo.tipo;
	}
	
	void
	adicionar_reservados(void)
	{
		adicionar_registro("const",Const);
		adicionar_registro("var",Var);
		adicionar_registro("integer",Integer);
		adicionar_registro("char",Char);
		adicionar_registro("for",For);
		adicionar_registro("if",If);
		adicionar_registro("else",Else);
		adicionar_registro("and",And);
		adicionar_registro("or",Or);
		adicionar_registro("not",Not);
		adicionar_registro("=",Igual);
		adicionar_registro("to",To);
		adicionar_registro("(",A_Parenteses);
		adicionar_registro(")",F_Parenteses);
		adicionar_registro("<",Menor);
		adicionar_registro(">",Maior);
		adicionar_registro("<>",Diferente);
		adicionar_registro(">=",MaiorIgual);
		adicionar_registro("<=",MenorIgual);
		adicionar_registro(",",Virgula);
		adicionar_registro("+",Mais);
		adicionar_registro("-",Menos);
		adicionar_registro("*",Vezes);
		adicionar_registro("/",Barra);
		adicionar_registro(";",PtVirgula);
		adicionar_registro("{",A_Chaves);
		adicionar_registro("}",F_Chaves);
		adicionar_registro("then",Then);
		adicionar_registro("readln",Readln);
		adicionar_registro("step",Step);
		adicionar_registro("write",Write);
		adicionar_registro("writeln",Writeln);
		adicionar_registro("%",Porcento);
		adicionar_registro("[",A_Colchete);
		adicionar_registro("]",F_Colchete);
		adicionar_registro("do",Do);
	}
	
	struct Celula
	*adicionar_registro(char *lexema, Tokens token)
	{
		unsigned int pos = hash(lexema,TAM_TBL);
		struct Celula *cel = (struct Celula *) malloc(sizeof(struct Celula));
		struct Simbolo *simb = (struct Simbolo *) malloc(sizeof(struct Simbolo));
		simb->token = token;
		simb->lexema = lexema;
		simb->classe = 0;
		simb->tipo = 0;
		simb->tamanho=1;
		cel->prox = NULL;
		cel->simbolo = *simb;
		
		if (tabela_simbolos[pos] == NULL) {
			tabela_simbolos[pos] = cel;
		}else{
			tabela_simbolos[pos]->prox = cel;
		}
		return cel;
	}
	
	struct Celula
	*pesquisar_registro(char *procurado)
	{
		int encontrado = 0;
		unsigned int pos = hash(procurado,TAM_TBL);
		struct Celula *retorno = NULL;
		struct Celula *prox = tabela_simbolos[pos];
	
		while (!encontrado && prox != NULL) {
			if (prox->simbolo.lexema != NULL && strcmp(prox->simbolo.lexema, procurado) == 0) {
				encontrado = 1;
				retorno = prox;
			} else {
				prox = prox->prox;
			}
		}
		return retorno;
	}
	
	/* printa a tabela de simbolos */
	void
	mostrar_tabela_simbolos(void)
	{
		printf("=============TABELA DE SÍMBOLOS=============\n");
		for (int i=0; i<TAM_TBL; ++i) {
			if (tabela_simbolos[i] != NULL) {
				printf("|\t%d\t|-> %s[%d]", i, tabela_simbolos[i]->simbolo.lexema,tabela_simbolos[i]->simbolo.tamanho);
				struct Celula *prox = tabela_simbolos[i]->prox;
				while (prox != NULL){
					printf(" -> %s",prox->simbolo.lexema);
					prox = prox->prox;
				}
				printf("\n");
			}
		}
	}
	
	void
	iniciar_tabela(void)
	{
	    adicionar_reservados();
	}
	
	/* limpa a lista encadeada recursivamente.
	 * Utilizado somente para fins de testes
	 */
	void
	limpar_lista(struct Celula *cel)
	{
		if (cel != NULL) {
			limpar_lista(cel->prox);
			free(cel);
		}
	}
	
	/* limpa a tabela de símbolos
	 * usado para fins de testes
	 */
	void
	limpar_tabela(void)
	{
		for (int i=0; i<TAM_TBL; ++i) {
			if (tabela_simbolos[i] != NULL) {
				limpar_lista(tabela_simbolos[i]->prox);
				free(tabela_simbolos[i]);
				tabela_simbolos[i] = NULL;
			}
		}
	}

#endif
