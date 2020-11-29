/* **************************************************************************
 * Compilador desenvolvido para a diciplina de compiladores-2020-2 PUC minas
 * Este programa segue o estilo de código definido em: 
 * https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 *
 * Arquivo lexan:
 * Implementacao do automato do analisador léxico. Lê cada lexema e realiza
 * ações necessárias correspondentes.
 * Ao encontrar EOF marca a variável global not_eof como 0.
 * Em caso de erro aborta o programa com o código devido.
 * *************************************************************************/

#include "lexan.h"
#include "ts.c"

void
lexan(void)
{
	int estado = 0;
	int pos_atual = ftell(prog_fonte);

	/* zera o lexema_lido */
	lexema_lido = "";

	/* zera o token e o lexema */
	reg_lex.lexema = "";
	reg_lex.token = 0;

	while (estado != ACEITACAO_LEX && !erro && (letra = minusculo(lex_get_char())) != -1) { 

		/* \n é contabilizado sempre */
		if (letra == '\n') {
			linha++;
		} 

		if (estado == 0) {
			if (eh_branco(letra)) {
				continue;
			} else if (letra == '/') {
				/* comentário ou divisão */ 
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = 1;
			} else if (letra == '_' || letra == '.') {
				/* inicio de identificador */
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = 7;
			} else if (letra == '<') {
				/* menor ou menor ou igual ou diferente*/
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = 4;
			} else if (letra == '>') {
				/* maior ou maior ou igual */
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = 5;
			} else if (letra == '"') {
				/* inicio de string */
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				reg_lex.tamanho = 1;
				estado = 9;
			} else if (letra == '0') {
				/* possivel hexadecimal */
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = 10;
			} else if (eh_digito(letra)) {
				/* inicio de literal */ 
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = 6;
			} else if (letra == ',') {
				reg_lex.token = Virgula;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;
			} else if (letra == ';') {
				reg_lex.token = PtVirgula;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;

			} else if (letra == '+') {
				reg_lex.token = Mais;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;

			} else if (letra == '-') {
				reg_lex.token = Menos;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;

			} else if (letra == '*') {
				reg_lex.token = Vezes;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;

			} else if (letra == '(') {
				reg_lex.token = A_Parenteses;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;

			} else if (letra == ')') {
				reg_lex.token = F_Parenteses;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;

			} else if (letra == '{') {
				reg_lex.token = A_Chaves;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;

			} else if (letra == '}') {
				reg_lex.token = F_Chaves;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;

			} else if (letra == '[') {
				reg_lex.token = A_Colchete;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;

			} else if (letra == ']') {
				reg_lex.token = F_Colchete;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;

			} else if (letra == '%') {
				reg_lex.token = Porcento;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;

			} else if (letra == '=') {
				reg_lex.token = Igual;
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = ACEITACAO_LEX;
			} else if (letra == '_' || letra == '.') {
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = 7;
			} else if (eh_letra(letra)) {
				/*inicio palavra*/
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = 8;
			} else if (!letra) {
				estado = ACEITACAO_LEX;
			} else {
				/* lexema nao identificado */
				erro = ER_LEX_N_ID;
				erro_msg = "lexema nao identificado";
				lexema_lido = encurtar(lexema_lido);
				abortar();
			}

		} else if (estado == 1) {
			if (letra == '*') {
				/* de fato comentario */
				estado = 2;
			} else {
				/* simbolo '/' encontrado */
				estado = ACEITACAO_LEX;
				reg_lex.token = Barra;

				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
				 */
				if (! eh_branco(letra)) {
					fseek(prog_fonte, pos_atual, SEEK_SET);
					lexema_lido = encurtar(lexema_lido);
				}
			}
		} else if (estado == 2) {
			if (letra == '*') {
				/* inicio de fim de comentario */
				estado = 3;
			} else if (letra == -1) {
				/*EOF encontrado*/
				erro = ER_LEX_EOF;
				erro_msg = "fim de arquivo nao esperado";
				abortar();
			} else if (C_INVALIDO) {
				/* caractere inválido */
				erro = ER_LEX_INVD;
				erro_msg = "caractere invalido";
				abortar();

			} 
		} else if (estado == 3) {
			if (letra == '/') {
				/* de fato fim de comentario volta ao inicio para ignorar*/
				estado = 0;
				reg_lex.lexema = "";
			} else if (letra == '*') {
				/* ** no comentario, espera pela barra */
				estado = 3;
			} else if (letra == -1) {
				/*EOF encontrado*/
				erro = ER_LEX_EOF;
				erro_msg = "fim de arquivo nao esperado";
				abortar();
			} else {
				/* simbolo '*' dentro do comentario */
				estado = 2;
			}
		} else if (estado == 4) {
			if (letra == '=' || letra == '>') {
				/* lexemas de comparacao <= ou <> */
				estado = ACEITACAO_LEX;

				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);

				if (letra == '=')
					reg_lex.token = MenorIgual;
				else
					reg_lex.token = Diferente;

			} else if (eh_branco(letra) || eh_digito(letra) || eh_letra(letra)) {
				/* lexema de comparacao < */
				estado = ACEITACAO_LEX;

				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
				 */
				if (! eh_branco(letra)) {
					fseek(prog_fonte, pos_atual, SEEK_SET);
					lexema_lido = encurtar(lexema_lido);
				}

				reg_lex.token = Menor;
			} else if (letra == -1) {
				/*EOF encontrado, assume que encontrou <*/
				estado = ACEITACAO_LEX;
			}
		} else if (estado == 5) {
			if (letra == '=') {
				/* lexema de comparacao >= */
				estado = ACEITACAO_LEX;

				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				reg_lex.token = MaiorIgual;

			} else if (eh_branco(letra) || eh_digito(letra) || eh_letra(letra)) {
				/* lexema de comparacao > */
				estado = ACEITACAO_LEX;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
				 */
				if (! eh_branco(letra)) {
					fseek(prog_fonte, pos_atual,SEEK_SET);
					lexema_lido = encurtar(lexema_lido);
				}

				reg_lex.token = Maior;
			} else if (letra == -1) {
				/*EOF encontrado, assume que encontrou >*/
				estado = ACEITACAO_LEX;
			}

		} else if (estado == 6) {
			/* Inteiros */
			/* le ate encontrar diferente de numero */

			if (eh_digito(letra)) {
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
			} else {
				estado = ACEITACAO_LEX;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema */
				if (! eh_branco(letra)) {
					fseek(prog_fonte, pos_atual, SEEK_SET);
					lexema_lido = encurtar(lexema_lido);
				}

				reg_lex.token = Literal;
				reg_lex.tipo = TP_Integer;
			}
		} else if (estado == 7) {
			/*lexema identificador _ . 
			  concatena até achar uma letra ou numero */
			if (letra == '_' || letra == '.') {
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
			} else if (eh_letra(letra) || eh_digito(letra)) {
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = 8;
			}
		} else if (estado == 8) {
			/*lexema de identificador
			  concatena ate finalizar o identificador ou palavra reservada */
			if (eh_letra(letra) ||  letra == '_' || letra == '.' || eh_digito(letra)) {
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
			} else {
				estado = ACEITACAO_LEX;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema */
				if (! eh_branco(letra)) {
					fseek(prog_fonte, pos_atual, SEEK_SET);
					lexema_lido = encurtar(lexema_lido);
				}

				reg_lex.tamanho = 1;
				reg_lex.pos = -1;
				reg_lex.token = identifica_token(reg_lex.lexema);

				//adicionar novo token (identificador)
				reg_lex.endereco = pesquisar_registro(reg_lex.lexema);
				if (reg_lex.endereco == NULL) {
					reg_lex.endereco = adicionar_registro(reg_lex.lexema,reg_lex.token);
					reg_lex.endereco->simbolo.tipo = reg_lex.tipo;
				} else {
					/* palavras reservadas nao possuem tipo,
					 * portanto nao atualiza o tipo do registro
					 * lexico se nao tem tipo
					 */
					if (reg_lex.endereco->simbolo.tipo != 0)
						reg_lex.tipo = reg_lex.endereco->simbolo.tipo;
					reg_lex.tamanho = reg_lex.endereco->simbolo.tamanho;
				}
			} 
		} else if (estado == 9) {
			/*lexema de String
			  concatena até encontrar o fechamento das aspas */

			reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
			reg_lex.tamanho++;
			if (letra == '"') {
				reg_lex.token = Literal;
				reg_lex.tipo = TP_Char;
				estado = ACEITACAO_LEX;
			} else if (letra == EOF) {
				/*EOF encontrado*/
				erro = ER_LEX_EOF;
				erro_msg = "fim de arquivo nao esperado.";
				abortar();
			} else if (C_INVALIDO) {
				/* caractere inválido */
				erro = ER_LEX_INVD;
				erro_msg = "caractere invalido";
				abortar();

			}
		} else if (estado == 10) {
			/* hexadecimal */
			if (letra == 'x') {           /* de fato hexadecimal */
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = 11;
			} else if (eh_digito(letra)) {   /* numero começando com 0 */ 
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = 6;
			} else if (!eh_letra(letra) && !eh_digito(letra)) {
				/* valor 0 */

				reg_lex.token = Literal;
				reg_lex.tipo = TP_Integer;

				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
				 */
				if (! eh_branco(letra)) {
					fseek(prog_fonte, pos_atual, SEEK_SET);
					lexema_lido = encurtar(lexema_lido);
				}

				estado = ACEITACAO_LEX;

			} else {
				erro = ER_LEX_N_ID;
				erro_msg = "lexema nao identificado";
				lexema_lido = encurtar(lexema_lido);
				abortar();
			}

		} else if (estado == 11) {
			/* parte numerica do hexadecimal deve conter pelo menos um numero ou A-F */
			if (eh_digito(letra) || (97 <= letra && letra <= 102)) {
				/* leu 0x[A-F0-9] */
				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);
				estado = 12;
			} else {
				/* leu só 0x, invalido */
				erro = ER_LEX_N_ID;
				erro_msg = "lexema nao identificado";
				lexema_lido = encurtar(lexema_lido);
				abortar();
			}
		} else if (estado == 12) {
			/* resto do valor hexadecimal */
			if (eh_digito(letra) || (97 <= letra && letra <= 102)) {

				reg_lex.lexema = concatenar(reg_lex.lexema, &letra);

			} else {

				reg_lex.token = Literal;
				reg_lex.tipo = TP_Integer;

				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
				 */
				if (! eh_branco(letra)) {
					fseek(prog_fonte, pos_atual, SEEK_SET);
					lexema_lido = encurtar(lexema_lido);
				}

				estado = ACEITACAO_LEX;
			}
		}

		pos_atual = ftell(prog_fonte);
	}

	/* leu EOF */
	if (letra == -1) not_eof = 0;

	DEBUGLEX("LEX: lexema:%s token:%d tipo:%d tam: %d\n",reg_lex.lexema,reg_lex.token,reg_lex.tipo,reg_lex.tamanho);
}
