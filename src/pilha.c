#ifndef _PILHA
#define _PILHA
	/*
	 * Implementacao de uma pilha das chamadas dos métodos.
	 * Atualmente utilizada para Debugar o analisador sintatico
	 */
	
	struct elemento {
		char *str; /* nome do metodo na pilha */
	};
	
	struct pilha_d {
		struct elemento *elementos;
		int tam;
	};
	
	void del(struct pilha_d *);
	struct pilha_d *initPilha(void);
	void printPilha(struct pilha_d *);
	void deletaPilha(struct pilha_d *);
	void push(char *, struct pilha_d *);
	struct elemento *pop(struct pilha_d *);
	
	
	/* inicia a pilha com um elemento apenas */
	struct pilha_d *initPilha(void)
	{
		struct pilha_d *pilha = (struct pilha_d *) malloc(sizeof(struct pilha_d));
		pilha->elementos = (struct elemento *) malloc(sizeof(struct elemento));
		pilha->tam = 1;
		return pilha;
	}
	
	/* insere na pilha */
	void push(char *str, struct pilha_d *pilha)
	{
		struct elemento elem;
		elem.str = str;
	
		if (pilha != NULL) {
			/* adiciona uma posicao na pilha */
			pilha->elementos = (struct elemento *) realloc(pilha->elementos,
					(pilha->tam+1) * sizeof(struct elemento));
			/* insere o elemento nessa posicao e incrementa a contagem do tamanho */
			pilha->elementos[pilha->tam++] = elem;
	
		}
	}
	
	/* remove um elemento da pilha e retorna ele */
	struct elemento *pop(struct pilha_d *pilha)
	{
		struct elemento *retorno = NULL;
		/* nada a fazer se a pilha estiver vazia */
		if (pilha != NULL) {
	
			/* ultima posicao */
			retorno = &pilha->elementos[pilha->tam-1]; 
	
			/* pilha se torna vazia entao deleta */
			if (pilha->tam == 1) {
	
				deletaPilha(pilha);
	
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
	void del(struct pilha_d *pilha)
	{
		/* nada a fazer se a pilha estiver vazia */
		if (pilha != NULL && pilha->tam > 0) {

			/* deleta o ultimo elemento e reduz a contagem do tamanho */
			pilha->elementos = (struct elemento *) realloc(pilha->elementos,
					(pilha->tam-1) * sizeof(struct elemento));
			pilha->tam--;

		}
	
	}
	
	void deletaPilha(struct pilha_d *pilha)
	{
		free(pilha->elementos);
		free(pilha);
	}
	
	/* exibe a pilha na tela */
	void printPilha(struct pilha_d *pilha)
	{
		printf("=-------=-------=\n");
		if (pilha != NULL && pilha->elementos != NULL) {
			for(int i=pilha->tam-1; i >= 0 ; --i) {
				printf("%d- %s\n",i,pilha->elementos[i].str);
			}
		}
		printf("\n");
	}
#endif
