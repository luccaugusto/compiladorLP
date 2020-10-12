/* Testes automatizados do compilador */

#include "ts.c"
#include "lexan.c"

/* testes */
void testeLexan(void);
void testeColisao(void);
void testeInsercao(void);
void testeBuscaVazia(void);
void testeBuscaSimples(void);
void testeBuscaEmColisao(void);
void testesTabelaSimbolos(void);

/* testa a inserção de um registro na tabela */
void testeInsercao(void)
{
	printf("Testando inserção simples......");
	char *lex = "teste";
	struct Celula *inserido = adicionarRegistro(lex, Identificador);
	if (inserido == NULL)
		printf("ERRO: Inserção falhou\n");
	else
		printf("OK\n");
}

/* testa a inserção de um registro que cause uma colisão na tabela */
void testeColisao(void)
{
	printf("Testando inserção com colisão..");
	char *lex = "teste2";
	struct Celula *inserido = adicionarRegistro(lex, Identificador);
	struct Celula *colisao = adicionarRegistro(lex, Identificador);
	if (inserido->prox != colisao)
		printf("ERRO: Colisão falhou\n");
	else
		printf("OK\n");
}

/* testa a busca por um registro que está no início da lista, ou seja,
 * sem colisões.
 */
void testeBuscaSimples(void)
{
	printf("Testando busca simples.........");
	char *lex = "teste3";
	struct Celula *inserido = adicionarRegistro(lex, Identificador);
	struct Celula *encontrado = pesquisarRegistro(lex);
	if (encontrado != inserido)
		printf("ERRO: Busca simples falhou\n");
	else
		printf("OK\n");
}

/* testa a busca por um registro que está no meio da lista, ou seja,
 * quando houveram colisões.
 */
void testeBuscaEmColisao(void)
{
	printf("Testando busca em colisão......");
	/* strings que colidem e são diferentes */
	char *lex = "not";
	char *lex2 = "%";
	/*guarda somente o registro que colidiu, que é o que buscamos*/
	adicionarRegistro(lex, Identificador);
	struct Celula *inserido = adicionarRegistro(lex2, Identificador);
	struct Celula *encontrado = pesquisarRegistro(lex2);
	if (encontrado != inserido)
		printf("ERRO: Busca em colisão falhou.\n \
					  ponteiro inserido: %p\n \
				      ponteiro encontrado: %p\n", inserido, encontrado);
	else
		printf("OK\n");
}

/* testa a busca por um registro que não está na lista
 */
void testeBuscaVazia(void)
{
	printf("Testando busca vazia...........");
	/* strings que colidem e são diferentes */
	char *lex = "esse lexema não está na tabela";
	struct Celula *encontrado = pesquisarRegistro(lex);
	if (encontrado != NULL)
		printf("ERRO: Busca em colisão falhou.\n");
	else
		printf("OK\n");
}

/* roda todos os testes da tabela de simbolos */
void testesTabelaSimbolos(void)
{
	testeInsercao();
	testeColisao();
	testeBuscaSimples();
	testeBuscaEmColisao();
	testeBuscaVazia();
	limparTabela();
}

/* testa o analisador lexico lendo o arquivo todo
 * e printando todos os lexemas encontrados
 */
void testeLexan(void)
{
	while(lex){
		lexan();
		printf("atual: %s\n",tokenAtual.lexema);
	}
}
