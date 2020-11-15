/* Definicoes de tipos e estruturas utilizadas */

#ifndef _TYPES
#define _TYPES

	#define NOVO_FATOR(s) struct Fator *s = (struct Fator *)malloc(sizeof(struct Fator)); \
											s->endereco = novoTemp(2); \
											s->tipo = regLex.tipo; \
											s->op = 0
	#define NOVO_TERMO(s) struct Termo *s = (struct Termo *)malloc(sizeof(struct Termo)); \
											s->endereco = novoTemp(2); \
											s->tipo = regLex.tipo

	typedef enum {
		Identificador = 1,
		Const,
		Var,
		Integer,
		Char,
		For,
		If,
		Else,
		And,
		Or,
		Not,
		Igual,
		To,
		A_Parenteses,
		F_Parenteses,
		Menor,
		Maior,
		Diferente,
		MaiorIgual,
		MenorIgual,
		Virgula,
		Mais,
		Menos,
		Vezes,
		Barra,
		PtVirgula,
		A_Chaves,
		F_Chaves,
		Then,
		Readln,
		Step,
		Write,
		Writeln,
		Porcento,
		A_Colchete,
		F_Colchete,
		Do,
		Literal
	} Tokens;
	
	typedef enum {
		TP_Integer = 1,
		TP_Char,
		TP_Logico,
	} Tipo;
	
	typedef enum {
		CL_Const = 1,
		CL_Var,
	} Classe;
	
	/*registro na tabela de símbolos*/
	struct Simbolo {
		int tamanho;    /* tamanho do array    */
		char *lexema;   /* lexema              */
		int memoria;    /* endereco na memoria */
		Tipo tipo;      /* int char ou logico  */
		Tokens token;   /* token do simbolo    */
		Classe classe;  /* const ou var        */
	};
	
	/* Celulas da lista encadeada */
	struct Celula {
		struct Celula *prox;
		struct Simbolo simbolo;
	};
	
	/* Registro léxico 
	 * O registro lexico eh mantido para cada comando
	 * token,lexema, pos e tamanho se alteram a cada chamada do lexan()
	 * os demais atributos se alteram conforme o comando 
	 * vai sendo identificado pelo analisador sintatico
	 */
	struct registroLex {
		int pos;                     /* posicao a ser acessada no array, 0 se qualquer outra coisa */
		int tamanho;                 /* tamanho do array, arrays de tamanho 1 sao variaveis        */
		char *lexema;                /* lexema atual do registro do comando atual                  */
		Tipo tipo;                   /* Tipo atual da expressao ,identificador ou constante atual  */
		Tokens token;                /* token atual do registro do comando atual                   */
		Classe classe;               /* Classe atual da expressao ,identificador ou constante      */
		struct Celula *endereco;     /* endereco na tabela de simbolos do identificador atual      */
	};
	
	/* Fator
	 * um fator contém informações
	 * sobre o que a expressão gera
	 */
	struct Fator {
		struct Fator *fator; /* caso gere mais um fator                 */
		struct Termo *termo; /* todo fator pertence a um termo          */
		int endereco;        /* endereco do valor gerado pela expressao */
		int tamanho;         /* tamanho do valor gerado pela expressao  */
		Tokens op;           /* operacao realizada pela expressao       */
		Tipo tipo;           /* tipo do valor gerado pela expressao     */
	};

	/* Termo
	 * um termo contem informacoes sobre a expressao
	 * eh formado por dois fatores e uma operacao
	 */
	struct Termo {
		struct Fator *fator; /* fator gerado pelo termo                 */
		int n_termos;        /* numero de termos da expressao           */
		int endereco;        /* endereco do valor gerado pelo termo     */
		int tamanho;         /* tamanho do valor gerado pela expressao  */
		Tipo tipo;           /* tipo do valor gerado pela expressao     */
	};


#endif
