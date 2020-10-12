#define _LEXAN

#define ERRO_LEXICO -1
#define ERRO_LEXICO_EOF -2
#define ERRO_LEXICO_INV -3
#define ACEITACAO_LEX 11

#define DEBUG_LEX 1

extern struct registroLex tokenAtual;
extern FILE *progFonte;
extern int erro;
extern int lex;

/* Analisador léxico */
void lexan(void);
