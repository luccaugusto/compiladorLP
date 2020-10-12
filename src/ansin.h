#define ERRO_SINTATICO -4
#define ERRO_SINTATICO_EOF -5
#define N_ACEITACAO_SIN -6 

#define ACEITACAO_SIN 12

#define DEBUG_SIN 1

#include "types.h"

extern int erro;
extern int estado_sin;
extern int lex;
extern int lido;

/* Analisador Sintatico */
void nulo(void);
void teste(void);
void ansin(void);
void teste1(void);
void leitura(void);
void escrita(void);
void variavel(void);
void listaIds(void);
void expressao(void);
void escritaLn(void);
void constante(void);
void comandos2(void);
void repeticao(void);
void expressao1(void);
void expressao2(void);
void expressao3(void);
void repeticao1(void);
void declaracao(void);
void atribuicao(void);
void blocoComandos(void);
void iniciarAnSin(void);
void fimDeArquivo(void);
void erroSintatico(int);
int casaToken(Tokens);

