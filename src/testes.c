/* **************************************************************************
 * Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 * Este programa segue o estilo de código definido em: 
 * https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 *
 * Arquivo testes:
 * testes automatizados do lexan e da tabela de simbolos.
 *
 * *************************************************************************/
/* Testes automatizados do compilador */
/* HEADERS */
#include <assert.h>

#include "ts.h"
#include "lexan.h"

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
	DEBUGTS("Testando inserção simples......");
	char *lex = "teste";
	struct Celula *inserido = adicionar_registro(lex, Identificador);
	assert(inserido != NULL);
}

/* testa a inserção de um registro que cause uma colisão na tabela */
void
teste_colisao(void)
{
	DEBUGTS("Testando inserção com colisão..");
	char *lex = "teste2";
	struct Celula *inserido = adicionar_registro(lex, Identificador);
	struct Celula *colisao = adicionar_registro(lex, Identificador);
	assert(inserido->prox == colisao);
}

/* testa a busca por um registro que está no início da lista, ou seja,
 * sem colisões.
 */
void
teste_busca_simples(void)
{
	DEBUGTS("Testando busca simples.........");
	char *lex = "teste3";
	struct Celula *inserido = adicionar_registro(lex, Identificador);
	struct Celula *encontrado = pesquisar_registro(lex);
	assert(encontrado == inserido);
}

/* testa a busca por um registro que está no meio da lista, ou seja,
 * quando houveram colisões.
 */
void
teste_busca_colisao(void)
{
	DEBUGTS("Testando busca em colisão......");
	/* strings que colidem e são diferentes */
	char *lex = "not";
	char *lex2 = "%";
	/*guarda somente o registro que colidiu, que é o que buscamos*/
	adicionar_registro(lex, Identificador);
	struct Celula *inserido = adicionar_registro(lex2, Identificador);
	struct Celula *encontrado = pesquisar_registro(lex2);
	assert(encontrado == inserido);
}

/* testa a busca por um registro que não está na lista
 */
void
teste_busca_vazia(void)
{
	DEBUGTS("Testando busca vazia...........");
	/* strings que colidem e são diferentes */
	char *lex = "esse lexema não está na tabela";
	struct Celula *encontrado = pesquisar_registro(lex);
	assert(encontrado == NULL);
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
	char *x;
	while(lex){
		lexan();
		sprintf(x,"atual: %s\n",reg_lex.lexema);
		DEBUGTS(x);
	}
}
