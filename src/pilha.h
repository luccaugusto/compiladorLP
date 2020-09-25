/*
 * Implementacao de uma pilha das chamadas dos métodos.
 * Atualmente utilizada para Debugar o analisador sintatico
 */

struct elemento {
	char *metodo; /* nome do metodo na pilha */
};

extern int tamPilha; /* numero de elementos na pilha */
extern struct elemento *pilha;

/* inicia a pilha com um elemento apenas */
void initPilha(void)
{
	pilha = malloc(sizeof(struct elemento));
}

/* insere na pilha */
void push(char *metodo)
{
	struct elemento elem;
	elem.metodo = metodo;
	/* pilha vazia, cria uma posicao e insere o elemento nela */
	if (pilha == NULL) {

		initPilha();
		++tamPilha;
		pilha[0] = elem;

	} else {

		/* adiciona uma posicao na pilha */
		pilha = (struct elemento *) realloc(pilha, sizeof(tamPilha * sizeof(struct elemento)));
		/* insere o elemento nessa posicao e incrementa a contagem do tamanho */
		pilha[tamPilha++] = elem;
		printPilha();

	}
}

/* remove um elemento da pilha e retorna ele */
struct elemento *pop(void)
{
	struct elemento *retorno = NULL;
	/* nada a fazer se a pilha estiver vazia */
	if (pilha != NULL) {

		/* ultima posicao */
		retorno = &pilha[tamPilha-1]; 

		/* pilha se torna vazia */
		if (tamPilha == 1) {

			--tamPilha;
			free(&pilha[0]);
			free(pilha);

		} else {

			/* deleta o ultimo elemento e reduz a contagem do tamanho */
			free(&pilha[tamPilha--]);
			if ( !realloc(pilha,sizeof(pilha - sizeof(struct elemento))) )
				printf("ERRO NA PILHA\n");

		}

	}

	return retorno;
}

/* exibe a pilha na tela */
void printPilha(void)
{
	printf(SEPARADOR"\n");
	if (pilha != NULL) {
		for(int i=tamPilha-1; i >= 0 ; --i) {
			printf("%d- %s\n",i,pilha[i].metodo);
		}
	}
	printf("\n");
}
