/* MACROS */
#define TAM_TBL 254
#define SEPARADOR "=-=-=-=-=-=-=-="
#define ERRO_LEXICO -1
#define ERRO_SINTATICO -2
#define ERRO_SINTATICO_EOF -3
#define ACEITACAO_LEX 11
#define ACEITACAO_SIN 12
#define N_ACEITACAO_SIN -4 

#define DEBUG_LEX 1
#define DEBUG_SIN 1


/*DECLARAÇÕES DE FUNÇÕES*/

/* utils */
int ehLetra(char l);
int ehDigito(char l);
int ehBranco(char l);
char minusculo(char l);
unsigned int hash(unsigned char *str);
char *concatenar(char *inicio, char *fim);

/* testes */
void testeLexan(void);
void testeColisao(void);
void testeInsercao(void);
void testeBuscaVazia(void);
void testeBuscaSimples(void);
void testeBuscaEmColisao(void);
void testesTabelaSimbolos(void);

/* Tabela de símbolos */
void limparTabela(void);
void inicializarTabela(void);
void adicionarReservados(void);
void mostrarTabelaSimbolos(void);
void limparLista(struct Celula *cel);
struct Celula *pesquisarRegistro(char *lexema);
struct Celula *adicionarRegistro(char *lexema, int token);

/* Analisador Sintatico */
void nulo(void);
void teste(void);
void ansin(void);
void teste1(void);
void leitura(void);
void escrita(void);
void abortar(void);
void variavel(void);
void listaIds(void);
void expressao(void);
void escritaLn(void);
void constante(void);
void comandos2(void);
void repeticao(void);
void expressao1(void);
void expressao2(void);
void repeticao1(void);
void declaracao(void);
void atribuicao(void);
void blocoComandos(void);
void iniciarAnSin(void);
void fimDeArquivo(void);
void erroSintatico(int tipo);
int casaToken(Tokens encontrado);


/* Analisador léxico */
int lexan(void);

/* Pilha */
void initPilha(void);
void printPilha(void);
void push(char *metodo);
struct elemento *pop(void);
