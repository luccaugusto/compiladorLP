/* Implementacao da tabela de simbolos */
#ifndef _TS
#define _TS
	
	#include "types.h"
	#include "utils.c"
	#define TAM_TBL 254
	//define DEBUG_TS 0


	extern struct Celula *tabelaSimbolos[TAM_TBL];
	
	/* retorna o tipo do identificador */
	Tipo buscaTipo(char *identificador)
	{
		return pesquisarRegistro(identificador)->simbolo.tipo;
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
	
	struct Celula *adicionarRegistro(char *lexema, Tokens token)
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
		
		if (tabelaSimbolos[pos] == NULL) {
			tabelaSimbolos[pos] = cel;
		}else{
			tabelaSimbolos[pos]->prox = cel;
		}
		return cel;
	}
	
	struct Celula *pesquisarRegistro(char *procurado)
	{
		int encontrado = 0;
		unsigned int pos = hash(procurado,TAM_TBL);
		struct Celula *retorno = NULL;
		struct Celula *prox = tabelaSimbolos[pos];
	
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
	void mostrarTabelaSimbolos(void)
	{
		printf("=============TABELA DE SÍMBOLOS=============\n");
		for (int i=0; i<TAM_TBL; ++i) {
			if (tabelaSimbolos[i] != NULL) {
				printf("|\t%d\t|-> %s[%d]", i, tabelaSimbolos[i]->simbolo.lexema,tabelaSimbolos[i]->simbolo.tamanho);
				struct Celula *prox = tabelaSimbolos[i]->prox;
				while (prox != NULL){
					printf(" -> %s",prox->simbolo.lexema);
					prox = prox->prox;
				}
				printf("\n");
			}
		}
	}
	
	void inicializarTabela(void)
	{
	    adicionarReservados();
	}
	
	/* limpa a lista encadeada recursivamente.
	 * Utilizado somente para fins de testes
	 */
	void limparLista(struct Celula *cel)
	{
		if (cel != NULL) {
			limparLista(cel->prox);
			free(cel);
		}
	}
	
	/* limpa a tabela de símbolos
	 * usado para fins de testes
	 */
	void limparTabela(void)
	{
		for (int i=0; i<TAM_TBL; ++i) {
			if (tabelaSimbolos[i] != NULL) {
				limparLista(tabelaSimbolos[i]->prox);
				free(tabelaSimbolos[i]);
				tabelaSimbolos[i] = NULL;
			}
		}
	}

#endif
