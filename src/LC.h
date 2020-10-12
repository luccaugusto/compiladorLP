/* MACROS */
#define SUCESSO 0

/* VARIÁVEIS GLOBAIS */

/* parametros da linha de comando */
FILE *progFonte;
FILE *progAsm;

int lex = 1;
int erro = 0;
int lido = 0;
int linha = 0; /*linha do arquivo*/
int estado_sin = 0; /* estado de aceitacao ou nao do analisador sintatico */

char letra; /*posicao da proxima letra a ser lida no arquivo*/
char *erroMsg; /*Mensagem de erro a ser exibida*/

struct pilha_d *pilha = NULL;
struct registroLex tokenAtual; 
struct Celula *tabelaSimbolos[TAM_TBL];

/*DECLARAÇÕES DE FUNÇÕES*/
/* Fluxo de execução geral */
void abortar(void);
void sucesso(void);
