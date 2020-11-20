#define TAM_TBL 254
//define DEBUG_TS 0

#include "types.h"

/* Tabela de símbolos */
Tipo buscaTipo(char *);
void limparTabela(void);
void inicializarTabela(void);
void adicionarReservados(void);
void mostrarTabelaSimbolos(void);
void limparLista(struct Celula *);
struct Celula *pesquisarRegistro(char *);
struct Celula *adicionarRegistro(char *, Tokens);

extern struct Celula *tabelaSimbolos[TAM_TBL];
