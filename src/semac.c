/* **************************************************************************
 * Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 * Este programa segue o estilo de código definido em: 
 * https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 *
 * Arquivo semac:
 * Implementação de ações semânticas da gramática.
 * Verificações de tipos, classes e atualização de valores das estruturas.
 *
 * *************************************************************************/
/* Acoes semanticas 
 * Cada metodo implementa a verificacao de uma acao semantica,
 * em caso de erro, chama a funcao erro_sintatico e aborta o programa
 */
#ifndef _SEMAC
#define _SEMAC

	/* HEADERS */
	#include "ansin.h"

	/* DEFINICAO DE FUNCOES */
	
	void
	def_tipo(Tipo tipo)
	{
		reg_lex.tipo = tipo;
	}
	
	void
	defClasse(Classe classe)
	{
		reg_lex.classe = classe;
	}
	
	/* converte tipo integer para logico 
	 * tipo logico é implicito, 0 é falso,
	 * diferente de 0 é verdadeiro.
	 * caracteres não são válidos
	 */
	Tipo
	to_logico(Tipo tipo)
	{
		Tipo t = tipo;
		if (tipo == TP_Integer) t = TP_Logico;
		return t;
	}
	
	/* verifica se o tamanho dos arrays eh valido */
	void
	verifica_tam(int tam)
	{
		/* tipo char ocupa 1 byte portanto o array pode ter 4k posicoes */
		if (reg_lex.tipo == TP_Char && tam > 4000) erro_sintatico(ER_SIN_TAMVET);
	
		/* tipo integer ocupa 2 bytes portanto o array pode ter 4k/2 posicoes */
		if (reg_lex.tipo == TP_Integer && tam > 2000) erro_sintatico(ER_SIN_TAMVET);
	
		/* else 
		 * atualiza o registro lexico e tabela de simbolos com o tamanho
		 */
		reg_lex.tamanho = tam;
		reg_lex.endereco->simbolo.tamanho = tam;
	}
	
	/* Verificacao de tipo
	 * verifica se tipo A == tipo B
	 */
	void
	verifica_tipo(Tipo A, Tipo B)
	{
	
		if ( A != B) {
			erro_sintatico(ER_SIN_T_INC);
		}
		
	}
	
	/* Verificacao de classe
	 * atualiza na tabela de simbolos o tipo e a classe do elemento,
	 * caso a classe ja esteja definida, significa que a variavel
	 * ou constante ja foi declarada
	 */
	void
	verifica_classe(char* lex, Tipo tipo)
	{
		reg_lex.endereco = pesquisar_registro(lex);
	
		if (reg_lex.endereco->simbolo.classe == 0) {
			reg_lex.endereco->simbolo.tipo = tipo;
			reg_lex.endereco->simbolo.classe = reg_lex.classe;
		} else {
			erro_sintatico(ER_SIN_JADEC);
		}
	}
	
	/* Verificacao de declaracao
	 * verifica se o identificador ja foi declarado
	 * ou se é constante
	 */
	void
	verifica_declaracao(char *identificador)
	{
		if (pesquisar_registro(identificador)->simbolo.classe == 0)
			erro_sintatico(ER_SIN_NDEC);
	}

	/* verifica se o identificador eh constante */
	void
	verifica_const(char *identificador)
	{
		if (pesquisar_registro(identificador)->simbolo.classe == CL_Const)
			erro_sintatico(ER_SIN_C_INC);
	}
	
	/* Verificacao de atribuicao a vetor 
	 * operacoes sobre vetores so podem acontecer posicao a posicao
	 * operacoes na forma vet1 = vet2 nao sao permitidas
	 */
	void
	verifica_atr_vetor(void)
	{
		/* ATENCAO:
		 * Este erro poderia ter sido deixado para tempo de execucao
		 *
		 * atribuicao de strings 
		 * a string deve ter tamanho menor que o tamanho do vetor -1
		 * pois ainda deve ser acrescentado o $ para encerrar a string
		 */
		if (reg_lex.endereco->simbolo.tipo == TP_Char) {
			if (reg_lex.endereco->simbolo.tamanho < reg_lex.tamanho+1)
				erro_sintatico(ER_SIN_TAMVET);
	
		} 

		/* atribuicoes a vetores nao string */
		else if (reg_lex.endereco->simbolo.tamanho > 1 && reg_lex.pos == -1) {
			erro_sintatico(ER_SIN_T_INC);
		}
	}
#endif
