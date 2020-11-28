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

/* HEADERS */
#include "ansin.h"

/* DEFINICAO DE FUNCOES */

void
def_tipo(Tipo tipo)
{
	reg_lex.tipo = tipo;
}

void
def_classe(Classe classe)
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
tamanho_valido(int tam)
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
tipos_validos(Tipo A, Tipo B)
{

	if ( A != B) {
		erro_sintatico(ER_SIN_T_INC);
	}
	
}

void
eh_id_nao_declarado(char* lex, Tipo tipo)
{
	reg_lex.endereco = pesquisar_registro(lex);

	if (reg_lex.endereco->simbolo.classe == 0) {
		reg_lex.endereco->simbolo.tipo = tipo;
		reg_lex.endereco->simbolo.classe = reg_lex.classe;
	} else {
		erro_sintatico(ER_SIN_JADEC);
	}
}

void
eh_id_ja_declarado(char *identificador)
{
	if ((pesquisar_registro(identificador))->simbolo.classe == 0)
		erro_sintatico(ER_SIN_NDEC);
}

/* verifica se o identificador eh constante */
void
eh_const(char *identificador)
{
	if ((pesquisar_registro(identificador))->simbolo.classe == CL_Const)
		erro_sintatico(ER_SIN_C_INC);
}

/* operacoes sobre vetores so podem acontecer posicao a posicao */
void
atr_vetor_valida(void)
{
	/* ATENCAO:
	 * Erro em tempo de execucao?
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
