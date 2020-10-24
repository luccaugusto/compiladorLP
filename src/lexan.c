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
	tokenAtual.lexema = "";
	tokenAtual.token = 0;

	while (estado != ACEITACAO_LEX && !erro && (letra = getChar()) != -1) { 
		letra = minusculo(letra);

        /* \n é contabilizado sempre */
		if (letra == '\n') {
			linha++;
		} 

		if (estado == 0) {
			if (letra == '/') {
				/* comentário ou divisão */ 
				estado = 1;
			} else if (ehBranco(letra)) {
				continue;
			} else if (letra == '_' || letra == '.') {
				/* inicio de identificador */
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 7;
			} else if (letra == '<') {
				/* menor ou menor ou igual ou diferente*/
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 4;
			} else if (letra == '>') {
				/* maior ou maior ou igual */
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 5;
			} else if (letra == '"') {
				/* inicio de string */
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 9;
			} else if (ehDigito(letra)) {
				/* inicio de literal */ 
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 6;
			} else if (letra == ',') {
				tokenAtual.token = Virgula;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
			} else if (letra == ';') {
				tokenAtual.token = PtVirgula;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '+') {
				tokenAtual.token = Mais;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '-') {
				tokenAtual.token = Menos;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '*') {
				tokenAtual.token = Vezes;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '(') {
				tokenAtual.token = A_Parenteses;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == ')') {
				tokenAtual.token = F_Parenteses;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '{') {
				tokenAtual.token = A_Chaves;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '}') {
				tokenAtual.token = F_Chaves;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '[') {
				tokenAtual.token = A_Colchete;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == ']') {
				tokenAtual.token = F_Colchete;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '%') {
				tokenAtual.token = Porcento;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
				
			} else if (letra == '=') {
				tokenAtual.token = Igual;
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(&letra);
				estado = ACEITACAO_LEX;
			} else if (letra == '_' || letra == '.') {
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 7;
			} else if (ehLetra(letra)) {
                /*inicio palavra*/
                tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 8;
			} else if (ehDigito(letra)) {
                /*inicio inteiro*/
                tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 10;
			} else if ( ! letra) {
				estado = ACEITACAO_LEX;
			} else {
				/* lexema nao identificado */
				erro = ERRO_LEXICO;
				erroMsg = "lexema nao identificado";
				abortar();
			}
            
		} else if (estado == 1) {
			if (letra == '*') {
				/* de fato comentario */
				estado = 2;
			} else {
				/* simbolo '/' encontrado */
				estado = ACEITACAO_LEX;
			}
		} else if (estado == 2) {
			if (letra == '*') {
				/* inicio de fim de comentario */
				estado = 3;
			} else if (letra == -1) {
				/*EOF encontrado*/
				erro = ERRO_LEXICO_EOF;
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
				erro = ERRO_LEXICO_INV;
				erroMsg = "caractere invalido";
				abortar();
				
			} 
		} else if (estado == 3) {
			if (letra == '/') {
				/* de fato fim de comentario volta ao inicio para ignorar*/
				estado = 0;
			} else if (letra == '*') {
				/* ** no comentario, espera pela barra */
				estado = 3;
			} else if (letra == -1) {
				/*EOF encontrado*/
				erro = ERRO_LEXICO_EOF;
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

				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.endereco = pesquisarRegistro(tokenAtual.lexema);

				if (letra == '=')
					tokenAtual.token = MenorIgual;
				else
					tokenAtual.token = Diferente;

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

				tokenAtual.token = Menor;
				tokenAtual.endereco = pesquisarRegistro(tokenAtual.lexema);
			} else if (letra == -1) {
				/*EOF encontrado, assume que encontrou <*/
				estado = ACEITACAO_LEX;
			}
		} else if (estado == 5) {
			if (letra == '=') {
				/* lexema de comparacao >= */
				estado = ACEITACAO_LEX;

				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				tokenAtual.token = Identificador;
				tokenAtual.endereco = pesquisarRegistro(tokenAtual.lexema);

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

				tokenAtual.token = Maior;
				tokenAtual.endereco = pesquisarRegistro(tokenAtual.lexema);
			} else if (letra == -1) {
				/*EOF encontrado, assume que encontrou >*/
				estado = ACEITACAO_LEX;
			}

		} else if (estado == 6) {
			/* le ate encontrar diferente de numero */
			if (ehDigito(letra)) {
				tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
			} else {
				estado = ACEITACAO_LEX;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema */
				if (! ehBranco(letra)) {
					fseek(progFonte, posAtual, SEEK_SET);
					lexemaLido = encurtar(lexemaLido);
				}

                tokenAtual.token = Literal;
				tokenAtual.tipo = TP_Integer;
				tokenAtual.endereco = NULL;
			}
		} else if (estado == 7) {
            /*lexema identificador _ . 
            concatena até achar uma letra ou numero */
            if (letra == '_' || letra == '.') {
                tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
            } else if (ehLetra(letra) || ehDigito(letra)) {
                tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
				estado = 8;
            }
        } else if (estado == 8) {
            /*lexema de identificador
            concatena ate finalizar o identificador ou palavra reservada */
            if (ehLetra(letra) ||  letra == '_' || letra == '.' ) {
                tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
            } else {
				estado = ACEITACAO_LEX;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema */
				if (! ehBranco(letra)) {
					fseek(progFonte, posAtual, SEEK_SET);
					lexemaLido = encurtar(lexemaLido);
				}

                tokenAtual.token = identificaTipo(tokenAtual.lexema);
				tokenAtual.endereco = pesquisarRegistro(tokenAtual.lexema);
                if (tokenAtual.endereco == NULL) {
                   //adicionar novo token (identificador)
                    tokenAtual.endereco = adicionarRegistro(tokenAtual.lexema,tokenAtual.token);
                }
			} 
        } else if (estado == 9) {
            /*lexema de String
            concatena até encontrar o fechamento das aspas */

			tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
            if (letra == '"') {
            	tokenAtual.token = Literal;
				tokenAtual.tipo = TP_Char;
                estado = ACEITACAO_LEX;
            } else if (letra == EOF) {
				/*EOF encontrado*/
				erro = ERRO_LEXICO_EOF;
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
				erro = ERRO_LEXICO_INV;
				erroMsg = "caractere invalido";
				abortar();
				
			}
        } else if (estado == 10) {
            /*lexema de Inteiro
            concatena até finalizar o numero */
            if (ehDigito(letra)) {
                tokenAtual.lexema = concatenar(tokenAtual.lexema, &letra);
            } else {
				estado = ACEITACAO_LEX;
				/* retorna o ponteiro do arquivo para a posicao anterior pois consumiu
				 * um caractere de um possivel proximo lexema
			 	 */
				if (! ehBranco(letra)) {
					fseek(progFonte, posAtual, SEEK_SET);
					lexemaLido = encurtar(lexemaLido);
				}

				tokenAtual.token = Literal;
				tokenAtual.tipo = TP_Integer;

			} 
        }

		posAtual = ftell(progFonte);
	}

	/* leu EOF */
	if (letra == -1) lex = 0;

	if (DEBUG_LEX) printf("LEX: lexema:%s token:%d\n",tokenAtual.lexema,tokenAtual.token);
}
#endif
