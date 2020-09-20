/* Definicoes de tipos e estruturas utilizadas */

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

/*
typedef enum {
	S = 0,
	D,
	V,
	T,
	I,
	I2,
	C,
	B,
	A,
	X,
	R,
	P,
	R1,
	E,
	E1,
	E2,
	G,
	G1,
	G2,
	G3,
	Q,
	Y,
	Z,
	L,
	L2,
	W,
	W2,
	M,
	M2,
	ACEITACAO_SIN
} EstadoSin;
*/

/*registro na tabela de símbolos*/
struct Simbolo {
	Tokens token;
	char *lexema;
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
	int tamanho;
};
