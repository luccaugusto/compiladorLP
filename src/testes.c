/* Testes automatizados do compilador */
#ifndef _TESTES
#define _TESTES

	/* HEADERS */
	#include <stdio.h>

	#include "ts.c"
	#include "lexan.c"
	
	/* DECLARACAO DE FUNCOES */
	/* testes */
	void teste_lexan(void);
	void teste_colisao(void);
	void teste_insercao(void);
	void teste_busca_vazia(void);
	void teste_busca_simples(void);
	void teste_busca_colisao(void);
	void testesTabelaSimbolos(void);
	
	/* DEFINICAO DE FUNCOES */
	/* testa a inserção de um registro na tabela */
	void
	teste_insercao(void)
	{
		printf("Testando inserção simples......");
		char *lex = "teste";
		struct Celula *inserido = adicionar_registro(lex, Identificador);
		if (inserido == NULL)
			printf("ERRO: Inserção falhou\n");
		else
			printf("OK\n");
	}
	
	/* testa a inserção de um registro que cause uma colisão na tabela */
	void
	teste_colisao(void)
	{
		printf("Testando inserção com colisão..");
		char *lex = "teste2";
		struct Celula *inserido = adicionar_registro(lex, Identificador);
		struct Celula *colisao = adicionar_registro(lex, Identificador);
		if (inserido->prox != colisao)
			printf("ERRO: Colisão falhou\n");
		else
			printf("OK\n");
	}
	
	/* testa a busca por um registro que está no início da lista, ou seja,
	 * sem colisões.
	 */
	void
	teste_busca_simples(void)
	{
		printf("Testando busca simples.........");
		char *lex = "teste3";
		struct Celula *inserido = adicionar_registro(lex, Identificador);
		struct Celula *encontrado = pesquisar_registro(lex);
		if (encontrado != inserido)
			printf("ERRO: Busca simples falhou\n");
		else
			printf("OK\n");
	}
	
	/* testa a busca por um registro que está no meio da lista, ou seja,
	 * quando houveram colisões.
	 */
	void
	teste_busca_colisao(void)
	{
		printf("Testando busca em colisão......");
		/* strings que colidem e são diferentes */
		char *lex = "not";
		char *lex2 = "%";
		/*guarda somente o registro que colidiu, que é o que buscamos*/
		adicionar_registro(lex, Identificador);
		struct Celula *inserido = adicionar_registro(lex2, Identificador);
		struct Celula *encontrado = pesquisar_registro(lex2);
		if (encontrado != inserido)
			printf("ERRO: Busca em colisão falhou.\n \
						  ponteiro inserido: %p\n \
					      ponteiro encontrado: %p\n", inserido, encontrado);
		else
			printf("OK\n");
	}
	
	/* testa a busca por um registro que não está na lista
	 */
	void
	teste_busca_vazia(void)
	{
		printf("Testando busca vazia...........");
		/* strings que colidem e são diferentes */
		char *lex = "esse lexema não está na tabela";
		struct Celula *encontrado = pesquisar_registro(lex);
		if (encontrado != NULL)
			printf("ERRO: Busca em colisão falhou.\n");
		else
			printf("OK\n");
	}
	
	/* roda todos os testes da tabela de simbolos */
	void
	testesTabelaSimbolos(void)
	{
		teste_insercao();
		teste_colisao();
		teste_busca_simples();
		teste_busca_colisao();
		teste_busca_vazia();
		limpar_tabela();
	}
	
	/* testa o analisador lexico lendo o arquivo todo
	 * e printando todos os lexemas encontrados
	 */
	void
	teste_lexan(void)
	{
		while(lex){
			lexan();
			printf("atual: %s\n",reg_lex.lexema);
		}
	}

#endif
