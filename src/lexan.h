#ifndef _LEXAN_H
#define _LEXAN_H

	#define C_INVALIDO letra != '/' &&\
			!eh_branco(letra)&&\
			!eh_digito(letra)&&\
			!eh_letra(letra) &&\
			letra != '_'    &&\
			letra != '.'    &&\
			letra != '<'    &&\
			letra != '>'    &&\
			letra != '"'    &&\
			letra != ','    &&\
			letra != ';'    &&\
			letra != '+'    &&\
			letra != '-'    &&\
			letra != '('    &&\
			letra != ')'    &&\
			letra != '{'    &&\
			letra != '}'    &&\
			letra != '['    &&\
			letra != ']'    &&\
			letra != '%'    &&\
			letra != '='    &&\
			letra != ':'    &&\
			letra != '\''   &&\
			letra != '.'    \
	
	//define DEBUG_LEX
	#ifdef DEBUG_LEX
		#define DEBUGLEX(...) printf(__VA_ARGS__);
	#else
		#define DEBUGLEX(...)
	#endif

	/* TIPOS */
	/* ERROS */
	enum {
		ACEITACAO_LEX = 13,
		ER_LEX,
		ER_LEX_EOF,
		ER_LEX_INVD,
		ER_LEX_N_ID
	};
	
	/* VARIAVEIS GLOBAIS */
	extern struct registro_lex reg_lex;
	extern FILE *prog_fonte;
	extern int erro;
	extern int lex;
	
	/* DECLARACAO DE FUNCOES */
	void lexan(void);
#endif
