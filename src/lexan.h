#define _LEXAN

#define ER_LEX -11
#define ER_LEX_EOF -12
#define ER_LEX_INVD -13
#define ER_LEX_N_ID -14
#define ACEITACAO_LEX 13

#define DEBUG_LEX 0

extern struct registroLex tokenAtual;
extern FILE *progFonte;
extern int erro;
extern int lex;

/* Analisador léxico */
void lexan(void);
