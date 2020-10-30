/* Acoes semanticas 
 * Cada metodo implementa a verificacao de uma acao semantica,
 * em caso de erro, chama a funcao erroSintatico e aborta o programa
 */

#include "types.h"
#include "ansin.h"

void defTipo(Tipo);
Tipo toLogico(Tipo);
void defClasse(Classe);
void verificaTam(void);
void verificaTipo(Tipo, Tipo);
void verificaClasse(void);
void verificaDeclaracao(char *);

void defTipo(Tipo tipo)
{
	tokenAtual.tipo = tipo;
}

void defClasse(Classe classe)
{
	tokenAtual.classe = classe;
}

/* converte tipo integer para logico 
 * tipo logico é implicito, 0 é falso,
 * diferente de 0 é verdadeiro.
 * caracteres não são válidos
 */
Tipo toLogico(Tipo tipo)
{
	Tipo t = tipo;
	if (tipo == TP_Integer) t = TP_Logico;
	return t;
}

/* verifica se o tamanho dos arrays eh valido */
void verificaTam(void)
{
	int tam = str2int(tokenAtual.lexema);
	/* tipo char ocupa 1 byte portanto o array pode ter 4k posicoes */
	if (tokenAtual.tipo == TP_Char && tam > 4000) erroSintatico(ER_SIN_TAMVET);

	/* tipo integer ocupa 2 bytes portanto o array pode ter 4k/2 posicoes */
	if (tokenAtual.tipo == TP_Integer && tam > 2000) erroSintatico(ER_SIN_TAMVET);

	/* else 
	 * atualiza o registro lexico e tabela de simbolos com o tamanho
	 */
	tokenAtual.tamanho = tam;
	tokenAtual.endereco->simbolo.tamanho = tam;
}

/* Verificacao de tipo
 * verifica se tipo A == tipo B
 */
void verificaTipo(Tipo A, Tipo B)
{

	if ( A != B) {
		erroSintatico(ER_SIN_T_INC);
	}
	
}

/* Verificacao de classe
 * atualiza na tabela de simbolos o tipo e a classe do elemento,
 * caso a classe ja esteja definida, significa que a variavel
 * ou constante ja foi declarada
 */
void verificaClasse(void)
{
	tokenAtual.endereco = pesquisarRegistro(tokenAtual.lexema);

	if (tokenAtual.endereco->simbolo.classe == 0) {
		tokenAtual.endereco->simbolo.tipo = tokenAtual.tipo;
		tokenAtual.endereco->simbolo.classe = tokenAtual.classe;
	} else {
		erroSintatico(ER_SIN_JADEC);
	}
}

/* Verificacao de declaracao
 * verifica se o identificador ja foi declarado
 * ou se é constante
 */
void verificaDeclaracao(char *identificador)
{
	tokenAtual.endereco = pesquisarRegistro(identificador);

	if (tokenAtual.endereco->simbolo.classe == 0)
		erroSintatico(ER_SIN_NDEC);

	if (tokenAtual.endereco->simbolo.classe == CL_Const)
		erroSintatico(ER_SIN_C_INC);

}

/* Verificacao de atribuicao a vetor 
 * operacoes sobre vetores so podem acontecer posicao a posicao
 * operacoes na forma vet1 = vet2 nao sao permitidas
 */
void verificaAtrVetor(void)
{
	if (tokenAtual.endereco->simbolo.tamanho > 0 && tokenAtual.pos == -1)
		erroSintatico(ER_SIN_T_INC);
}
