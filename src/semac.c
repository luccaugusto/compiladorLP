/* Acoes semanticas 
 * Cada metodo implementa a verificacao de uma acao semantica,
 * em caso de erro, chama a funcao erroSintatico e aborta o programa
 */
#ifndef _SEMAC
#define _SEMAC

	#include "types.h"
	#include "ansin.h"
	
	void defTipo(Tipo);
	Tipo toLogico(Tipo);
	void verificaTam(int);
	void defClasse(Classe);
	void verificaTipo(Tipo, Tipo);
	void verificaClasse(char *);
	void verificaDeclaracao(char *);
	
	void defTipo(Tipo tipo)
	{
		regLex.tipo = tipo;
	}
	
	void defClasse(Classe classe)
	{
		regLex.classe = classe;
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
	void verificaTam(int tam)
	{
		/* tipo char ocupa 1 byte portanto o array pode ter 4k posicoes */
		if (regLex.tipo == TP_Char && tam > 4000) erroSintatico(ER_SIN_TAMVET);
	
		/* tipo integer ocupa 2 bytes portanto o array pode ter 4k/2 posicoes */
		if (regLex.tipo == TP_Integer && tam > 2000) erroSintatico(ER_SIN_TAMVET);
	
		/* else 
		 * atualiza o registro lexico e tabela de simbolos com o tamanho
		 */
		regLex.tamanho = tam;
		regLex.endereco->simbolo.tamanho = tam;
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
	void verificaClasse(char* lex)
	{
		regLex.endereco = pesquisarRegistro(lex);
	
		if (regLex.endereco->simbolo.classe == 0) {
			regLex.endereco->simbolo.tipo = regLex.tipo;
			regLex.endereco->simbolo.classe = regLex.classe;
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
		if (pesquisarRegistro(identificador)->simbolo.classe == 0)
			erroSintatico(ER_SIN_NDEC);
	}

	/* verifica se o identificador eh constante */
	void verificaConst(char *identificador)
	{
		if (pesquisarRegistro(identificador)->simbolo.classe == CL_Const)
			erroSintatico(ER_SIN_C_INC);
	}
	
	/* Verificacao de atribuicao a vetor 
	 * operacoes sobre vetores so podem acontecer posicao a posicao
	 * operacoes na forma vet1 = vet2 nao sao permitidas
	 */
	void verificaAtrVetor(void)
	{
		/* ATENCAO:
		 * Este erro poderia ter sido deixado para tempo de execucao
		 *
		 * atribuicao de strings 
		 * a string deve ter tamanho menor que o tamanho do vetor -1
		 * pois ainda deve ser acrescentado o $ para encerrar a string
		 */
		if (regLex.endereco->simbolo.tipo == TP_Char) {
			if (regLex.endereco->simbolo.tamanho < regLex.tamanho+1)
				erroSintatico(ER_SIN_TAMVET);
	
		} 

		/* atribuicoes a vetores nao string */
		else if (regLex.endereco->simbolo.tamanho > 1 && regLex.pos == -1) {
			erroSintatico(ER_SIN_T_INC);
		}
	}
#endif
