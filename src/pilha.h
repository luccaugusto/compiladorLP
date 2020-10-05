/*
 * Implementacao de uma pilha das chamadas dos métodos.
 * Atualmente utilizada para Debugar o analisador sintatico
 */

struct elemento {
	char *metodo; /* nome do metodo na pilha */
};

struct pilha_d {
	struct elemento *elementos;
	int tam;
};

extern struct pilha_d *pilha;

/* inicia a pilha com um elemento apenas */
void initPilha(void)
{
	pilha = malloc(sizeof(struct pilha_d));
	pilha->elementos = malloc(sizeof(struct elemento));
	pilha->tam = 1;
}

/* insere na pilha */
void push(char *metodo)
{
	struct elemento elem;
	elem.metodo = metodo;
	/* pilha vazia, cria uma posicao e insere o elemento nela */
	if (pilha == NULL) {

		initPilha();
		pilha->elementos[0] = elem;

	} else {

		/* adiciona uma posicao na pilha */
		pilha->elementos = (struct elemento *) realloc(pilha->elementos,
				(pilha->tam+1) * sizeof(struct elemento));
		/* insere o elemento nessa posicao e incrementa a contagem do tamanho */
		pilha->elementos[pilha->tam++] = elem;

	}
}

/* remove um elemento da pilha e retorna ele */
struct elemento *pop(void)
{
	struct elemento *retorno = NULL;
	/* nada a fazer se a pilha estiver vazia */
	if (pilha->elementos != NULL) {

		/* ultima posicao */
		retorno = &pilha->elementos[pilha->tam-1]; 

		/* pilha se torna vazia entao deleta */
		if (pilha->tam == 1) {

			deletaPilha();

		} else {

			/* deleta o ultimo elemento e reduz a contagem do tamanho */
			pilha->elementos = (struct elemento *) realloc(pilha->elementos,
					(pilha->tam-1) * sizeof(struct elemento));
			pilha->tam--;

		}

	}

	return retorno;
}

/* remove um elemento da pilha e NAO retorna ele */
void del(void)
{
	/* nada a fazer se a pilha estiver vazia */
	if (pilha->elementos != NULL) {

		/* pilha se torna vazia entao deleta */
		if (pilha->tam == 1) {

			deletaPilha();

		} else {

			/* deleta o ultimo elemento e reduz a contagem do tamanho */
			pilha->elementos = (struct elemento *) realloc(pilha->elementos,
					(pilha->tam-1) * sizeof(struct elemento));
			pilha->tam--;

		}

	}

}

void deletaPilha(void)
{
	free(pilha->elementos);
	free(pilha);
}

/* exibe a pilha na tela */
void printPilha(void)
{
	printf(SEPARADOR"\n");
	if (pilha != NULL && pilha->elementos != NULL) {
		for(int i=pilha->tam-1; i >= 0 ; --i) {
			printf("%d- %s\n",i,pilha->elementos[i].metodo);
		}
	}
	printf("\n");
}
