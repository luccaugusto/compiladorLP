/* Definicoes de tipos e estruturas utilizadas */

#ifndef _TYPES
#define _TYPES

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
	TP_Integer = 0,
	TP_Char,
} Tipo;

typedef enum {
	CL_Const = 0,
	CL_Var,
} Classe;

/*registro na tabela de símbolos*/
struct Simbolo {
	Tokens token;
	char *lexema;
	Tipo tipo;
	Classe classe;
	int tamanho;
};

/* Celulas da lista encadeada */
struct Celula {
	struct Celula *prox;
	struct Simbolo simbolo;
};

/* Registro léxico */
struct registroLex {
	Tokens token;
	char *lexema;
	struct Celula *endereco;
	Tipo tipo;
	Classe classe;
	int tamanho;
};
#endif
