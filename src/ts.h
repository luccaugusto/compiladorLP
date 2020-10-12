#define TAM_TBL 254

#include "types.h"

/* Tabela de símbolos */
void limparTabela(void);
void inicializarTabela(void);
void adicionarReservados(void);
void mostrarTabelaSimbolos(void);
void limparLista(struct Celula *);
struct Celula *pesquisarRegistro(char *);
struct Celula *adicionarRegistro(char *, int);

extern struct Celula *tabelaSimbolos[TAM_TBL];
