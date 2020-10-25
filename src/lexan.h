#define _LEXAN

#define ERRO_LEXICO -11
#define ERRO_LEXICO_EOF -12
#define ERRO_LEXICO_INV -13
#define ERRO_LEXICO_N_ID -14
#define ACEITACAO_LEX 13

#define DEBUG_LEX 0

extern struct registroLex tokenAtual;
extern FILE *progFonte;
extern int erro;
extern int lex;

/* Analisador léxico */
void lexan(void);
