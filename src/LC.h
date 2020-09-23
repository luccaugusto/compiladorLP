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
void ansin(void);
void abortar(void);
void variavel(void);
void listaIds(void);
void constante(void);
void declaracao(void);
void blocoComando(void);
void iniciarAnSin(void);
void erroSintatico(int tipo);
int casaToken(Tokens encontrado);

/* Analisador léxico */
int lexan(void);
