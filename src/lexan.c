#ifndef _LEXAN
#define _LEXAN
	#include "lexan.h"
	
	/* Implementacao do automato do analisador lexico
	 * grava lex como 1 caso de sucesso,
	 *         0 caso EOF encontrado,
	 *         letra caso erro lexico
	 */
	
	
	void lexan(void)
	{
		int estado = 0;
		int posAtual = ftell(progFonte);
	
		/* zera o lexemaLido */
		lexemaLido = "";
	
		/* zera o token e o lexema */
		regLex.lexema = "";
		regLex.token = 0;
	
		while (estado != ACEITACAO_LEX && !erro && (letra = minusculo(getChar())) != -1) { 
	
	        /* \n é contabilizado sempre */
			if (letra == '\n') {
				linha++;
			} 
	
			if (estado == 0) {
				if (ehBranco(letra)) {
					continue;
				} else if (letra == '/') {
					/* comentário ou divisão */ 
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = 1;
				} else if (letra == '_' || letra == '.') {
					/* inicio de identificador */
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = 7;
				} else if (letra == '<') {
					/* menor ou menor ou igual ou diferente*/
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = 4;
				} else if (letra == '>') {
					/* maior ou maior ou igual */
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = 5;
				} else if (letra == '"') {
					/* inicio de string */
					regLex.lexema = concatenar(regLex.lexema, &letra);
					regLex.tamanho = 1;
					estado = 9;
				} else if (letra == '0') {
					/* possivel hexadecimal */
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = 10;
				} else if (ehDigito(letra)) {
					/* inicio de literal */ 
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = 6;
				} else if (letra == ',') {
					regLex.token = Virgula;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
				} else if (letra == ';') {
					regLex.token = PtVirgula;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
					
				} else if (letra == '+') {
					regLex.token = Mais;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
					
				} else if (letra == '-') {
					regLex.token = Menos;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
					
				} else if (letra == '*') {
					regLex.token = Vezes;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
					
				} else if (letra == '(') {
					regLex.token = A_Parenteses;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
					
				} else if (letra == ')') {
					regLex.token = F_Parenteses;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
					
				} else if (letra == '{') {
					regLex.token = A_Chaves;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
					
				} else if (letra == '}') {
					regLex.token = F_Chaves;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
					
				} else if (letra == '[') {
					regLex.token = A_Colchete;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
					
				} else if (letra == ']') {
					regLex.token = F_Colchete;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
					
				} else if (letra == '%') {
					regLex.token = Porcento;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
					
				} else if (letra == '=') {
					regLex.token = Igual;
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = ACEITACAO_LEX;
				} else if (letra == '_' || letra == '.') {
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = 7;
				} else if (ehLetra(letra)) {
	                /*inicio palavra*/
	                regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = 8;
				} else if (!letra) {
					estado = ACEITACAO_LEX;
				} else {
					/* lexema nao identificado */
					erro = ER_LEX_N_ID;
					erroMsg = "lexema nao identificado";
					lexemaLido = encurtar(lexemaLido);
					abortar();
				}
	            
			} else if (estado == 1) {
				if (letra == '*') {
					/* de fato comentario */
					estado = 2;
				} else {
					/* simbolo '/' encontrado */
					estado = ACEITACAO_LEX;
					regLex.token = Barra;
				}
			} else if (estado == 2) {
				if (letra == '*') {
					/* inicio de fim de comentario */
					estado = 3;
				} else if (letra == -1) {
					/*EOF encontrado*/
					erro = ER_LEX_EOF;
					erroMsg = "fim de arquivo nao esperado";
					abortar();
				} else if (letra != '/' &&
						!ehBranco(letra)&&
						!ehDigito(letra)&&
						!ehLetra(letra) &&
						letra != '_'    &&
						letra != '.'    &&
						letra != '<'    &&
						letra != '>'    &&
						letra != '"'    &&
						letra != ','    &&
						letra != ';'    &&
						letra != '+'    &&
						letra != '-'    &&
						letra != '('    &&
						letra != ')'    &&
						letra != '{'    &&
						letra != '}'    &&
						letra != '['    &&
						letra != ']'    &&
						letra != '%'    &&
						letra != '='    &&
						letra != ':'    &&
						letra != '\''   &&
						letra != '.'    )
				{
					/* caractere inválido */
					erro = ER_LEX_INVD;
					erroMsg = "caractere invalido";
					abortar();
					
				} 
			} else if (estado == 3) {
				if (letra == '/') {
					/* de fato fim de comentario volta ao inicio para ignorar*/
					estado = 0;
					regLex.lexema = "";
				} else if (letra == '*') {
					/* ** no comentario, espera pela barra */
					estado = 3;
				} else if (letra == -1) {
					/*EOF encontrado*/
					erro = ER_LEX_EOF;
					erroMsg = "fim de arquivo nao esperado";
					abortar();
				} else {
					/* simbolo '*' dentro do comentario */
					estado = 2;
				}
			} else if (estado == 4) {
				if (letra == '=' || letra == '>') {
					/* lexemas de comparacao <= ou <> */
					estado = ACEITACAO_LEX;
	
					regLex.lexema = concatenar(regLex.lexema, &letra);
	
					if (letra == '=')
						regLex.token = MenorIgual;
					else
						regLex.token = Diferente;
	
				} else if (ehBranco(letra) || ehDigito(letra) || ehLetra(letra)) {
					/* lexema de comparacao < */
					estado = ACEITACAO_LEX;
	
					/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
					 * um caractere de um possivel proximo lexema
				 	 */
					if (! ehBranco(letra)) {
						fseek(progFonte, posAtual, SEEK_SET);
						lexemaLido = encurtar(lexemaLido);
					}
	
					regLex.token = Menor;
				} else if (letra == -1) {
					/*EOF encontrado, assume que encontrou <*/
					estado = ACEITACAO_LEX;
				}
			} else if (estado == 5) {
				if (letra == '=') {
					/* lexema de comparacao >= */
					estado = ACEITACAO_LEX;
	
					regLex.lexema = concatenar(regLex.lexema, &letra);
					regLex.token = MaiorIgual;
	
				} else if (ehBranco(letra) || ehDigito(letra) || ehLetra(letra)) {
					/* lexema de comparacao > */
					estado = ACEITACAO_LEX;
					/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
					 * um caractere de um possivel proximo lexema
					 */
					if (! ehBranco(letra)) {
						fseek(progFonte, posAtual,SEEK_SET);
						lexemaLido = encurtar(lexemaLido);
					}
	
					regLex.token = Maior;
				} else if (letra == -1) {
					/*EOF encontrado, assume que encontrou >*/
					estado = ACEITACAO_LEX;
				}
	
			} else if (estado == 6) {
				/* Inteiros */
				/* le ate encontrar diferente de numero */
	
				if (ehDigito(letra)) {
					regLex.lexema = concatenar(regLex.lexema, &letra);
				} else {
					estado = ACEITACAO_LEX;
					/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
					 * um caractere de um possivel proximo lexema */
					if (! ehBranco(letra)) {
						fseek(progFonte, posAtual, SEEK_SET);
						lexemaLido = encurtar(lexemaLido);
					}
	
	                regLex.token = Literal;
					regLex.tipo = TP_Integer;
				}
			} else if (estado == 7) {
	            /*lexema identificador _ . 
	            concatena até achar uma letra ou numero */
	            if (letra == '_' || letra == '.') {
	                regLex.lexema = concatenar(regLex.lexema, &letra);
	            } else if (ehLetra(letra) || ehDigito(letra)) {
	                regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = 8;
	            }
	        } else if (estado == 8) {
	            /*lexema de identificador
	            concatena ate finalizar o identificador ou palavra reservada */
	            if (ehLetra(letra) ||  letra == '_' || letra == '.' ) {
	                regLex.lexema = concatenar(regLex.lexema, &letra);
	            } else {
					estado = ACEITACAO_LEX;
					/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
					 * um caractere de um possivel proximo lexema */
					if (! ehBranco(letra)) {
						fseek(progFonte, posAtual, SEEK_SET);
						lexemaLido = encurtar(lexemaLido);
					}
	
					regLex.tamanho = 1;
					regLex.pos = -1;
	                regLex.token = identificaToken(regLex.lexema);
	
	                //adicionar novo token (identificador)
					regLex.endereco = pesquisarRegistro(regLex.lexema);
	                if (regLex.endereco == NULL) {
	                   regLex.endereco = adicionarRegistro(regLex.lexema,regLex.token);
					   regLex.endereco->simbolo.tipo = regLex.tipo;
					} else {
						/* palavras reservadas nao possuem tipo,
						 * portanto nao atualiza o tipo do registro
						 * lexico se nao tem tipo
						 */
						if (regLex.tipo != 0)
							regLex.tipo = regLex.endereco->simbolo.tipo;
						regLex.tamanho = regLex.endereco->simbolo.tamanho;
					}
				} 
	        } else if (estado == 9) {
	            /*lexema de String
	            concatena até encontrar o fechamento das aspas */
	
				regLex.lexema = concatenar(regLex.lexema, &letra);
				regLex.tamanho++;
	            if (letra == '"') {
	            	regLex.token = Literal;
					regLex.tipo = TP_Char;
	                estado = ACEITACAO_LEX;
	            } else if (letra == EOF) {
					/*EOF encontrado*/
					erro = ER_LEX_EOF;
					erroMsg = "fim de arquivo nao esperado.";
					abortar();
				} else if (letra != '/' &&
						!ehBranco(letra)&&
						!ehDigito(letra)&&
						!ehLetra(letra) &&
						letra != '_'    &&
						letra != ':'    &&
						letra != '.'    &&
						letra != '<'    &&
						letra != '>'    &&
						letra != '"'    &&
						letra != ','    &&
						letra != ';'    &&
						letra != '+'    &&
						letra != '-'    &&
						letra != '('    &&
						letra != ')'    &&
						letra != '{'    &&
						letra != '}'    &&
						letra != '['    &&
						letra != ']'    &&
						letra != '%'    &&
						letra != '='    &&
						letra != '\''   &&
						letra != '.'    )
				{
					/* caractere inválido */
					erro = ER_LEX_INVD;
					erroMsg = "caractere invalido";
					abortar();
					
				}
	        } else if (estado == 10) {
				/* hexadecimal */
				if (letra == 'x') {           /* de fato hexadecimal */
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = 11;
				} else if (ehDigito(letra)) {   /* numero começando com 0 */ 
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = 6;
				} else if (!ehLetra(letra) && !ehDigito(letra)) {
					/* valor 0 */
	
					regLex.token = Literal;
					regLex.tipo = TP_Char;
	
					/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
					 * um caractere de um possivel proximo lexema
				 	 */
					if (! ehBranco(letra)) {
						fseek(progFonte, posAtual, SEEK_SET);
						lexemaLido = encurtar(lexemaLido);
					}
	
					estado = ACEITACAO_LEX;
					
				} else {
					erro = ER_LEX_N_ID;
					erroMsg = "lexema nao identificado";
					lexemaLido = encurtar(lexemaLido);
					abortar();
				}
	
			} else if (estado == 11) {
				/* parte numerica do hexadecimal deve conter pelo menos um numero ou A-F */
				if (ehDigito(letra) || (97 <= letra && letra <= 102)) {
					/* leu 0x[A-F0-9] */
					regLex.lexema = concatenar(regLex.lexema, &letra);
					estado = 12;
				} else {
					/* leu só 0x, invalido */
					erro = ER_LEX_N_ID;
					erroMsg = "lexema nao identificado";
					lexemaLido = encurtar(lexemaLido);
					abortar();
				}
			} else if (estado == 12) {
				/* resto do valor hexadecimal */
				if (ehDigito(letra) || (97 <= letra && letra <= 102)) {
	
					regLex.lexema = concatenar(regLex.lexema, &letra);
	
				} else {
	
					regLex.token = Literal;
					regLex.tipo = TP_Char;
	
					/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
					 * um caractere de um possivel proximo lexema
				 	 */
					if (! ehBranco(letra)) {
						fseek(progFonte, posAtual, SEEK_SET);
						lexemaLido = encurtar(lexemaLido);
					}
	
					estado = ACEITACAO_LEX;
				}
			}
	
			posAtual = ftell(progFonte);
		}

		/* leu EOF */
		if (letra == -1) lex = 0;
	
		if (DEBUG_LEX) printf("LEX: lexema:%s token:%d tipo:%d tam: %d\n",regLex.lexema,regLex.token,regLex.tipo,regLex.tamanho);
	}
#endif
