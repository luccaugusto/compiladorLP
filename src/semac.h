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

#ifndef _SEMAC_H
#define _SEMAC_H

/* DEFINICAO DE FUNCOES */
void def_tipo(Tipo tipo);
void def_classe(Classe classe);
Tipo to_logico(Tipo tipo);
void tamanho_valido(int tam);
void tipos_compativeis(Tipo A, Tipo B);
void eh_id_nao_declarado(char* lex, Tipo tipo);
void eh_id_ja_declarado(char *identificador);
void eh_const(char *identificador);
void atr_vetor_valida(void);

#endif
