/* Definicao das funcoes utilitárias do projeto */

unsigned int hash(unsigned char *str)
{
    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return (hash % TAM_TBL);
}

int ehDigito(char l)
{
	/* valores de 0-9 em ascii */
	int retorno = 0;
	if (30 <= l && l <= 39)
		retorno = 1;
	return retorno;
}

int ehLetra(char l)
{
	/* valores de 0-9 em ascii */
	int retorno = 0;
	if (97 <= l && l <= 122)
		retorno = 1;
	return retorno;
}

int ehBranco(char l)
{
	/* valores de 10, 13, 32em ascii */
	int retorno = 0;
	if (l == 10 || l == 13 || l == 32)
		retorno = 1;
	return retorno;
}

char *concatenar(char *inicio, char *fim)
{
	char *retorno;
	int tamInicio = strlen(inicio);
	int tamFim = strlen(fim);

	retorno = malloc(tamInicio+tamFim);

	for (int i=0; i<tamInicio; ++i)
		retorno[i] = inicio[i];
	for (int i=0; i<tamFim; ++i)
		retorno[tamInicio+i] = fim[i];

	/*printf("concatenado %s\n",retorno);*/
	return retorno;
}

/* Converte maiusculas para minusculas */
char minusculo(char l)
{
	if (l >= 65 && l <=90)
		l += 32; 
	return l;
}

