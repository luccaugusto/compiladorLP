/* Definicao das funcoes utilitárias do projeto */

/* ************************** *
              HASH
 * ************************** */
unsigned int hash(unsigned char *str)
{
    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return (hash % TAM_TBL);
}



/* ************************** *
           REGEX LIKE 
 * ************************** */
int ehDigito(char l)
{
	/* valores de 0-9 em ascii */
	int retorno = 0;
	if (48 <= l && l <= 57)
		retorno = 1;
	return retorno;
}

int ehLetra(char l)
{
	/* valores de a-z em ascii */
	int retorno = 0;
	if (97 <= l && l <= 122)
		retorno = 1;
	return retorno;
}

int ehBranco(char l)
{
	/* valores de tab, line feed, <CR>, espaço em ascii */
	int retorno = 0;
	if (l == 9 || l == 10 || l == 13 || l == 32)
		retorno = 1;
	return retorno;
}

int identificaTipo(char *lex)
{
	int retorno = Identificador;

	if (strcmp(lex,"const") == 0)
		retorno = Const;
	else if (strcmp(lex,"var") == 0)
		retorno = Var;
	else if (strcmp(lex,"integer") == 0)
		retorno = Integer;
	else if (strcmp(lex,"char") == 0)
		retorno = Char;
	else if (strcmp(lex,"for") == 0)
		retorno = For;
	else if (strcmp(lex,"if") == 0)
		retorno = If;
	else if (strcmp(lex,"else") == 0)
		retorno = Else;
	else if (strcmp(lex,"and") == 0)
		retorno = And;
	else if (strcmp(lex,"or") == 0)
		retorno = Or;
	else if (strcmp(lex,"not") == 0)
		retorno = Not;
	else if (strcmp(lex,"to") == 0)
		retorno = To;
	else if (strcmp(lex,"then") == 0)
		retorno = Then;
	else if (strcmp(lex,"readln") == 0)
		retorno = Readln;
	else if (strcmp(lex,"step") == 0)
		retorno = Step;
	else if (strcmp(lex,"write") == 0)
		retorno = Write;
	else if (strcmp(lex,"writeln") == 0)
		retorno = Writeln;
	else if (strcmp(lex,"do") == 0)
		retorno = Do;

	return retorno;
}

/* ************************** *
   MANIPULACAO DE STRING 
 * ************************** */

char *concatenar(char *inicio, char *fim)
{
	char *retorno;
	int tamInicio = strlen(inicio);
	int tamFim = strlen(fim);

	/* acrescenta a posicao do \0 */
	retorno = (char *) malloc(tamInicio+tamFim+1);

	for (int i=0; i<tamInicio; ++i)
		retorno[i] = inicio[i];

	for (int i=0; i<tamFim; ++i)
		retorno[tamInicio+i] = fim[i];

	retorno[tamInicio+tamFim]='\0';

	return retorno;
}

/* Converte maiusculas para minusculas */
char minusculo(char l)
{
	if (l >= 65 && l <=90)
		l += 32; 
	return l;
}

/* ************************** *
   LEITURA DE ARQUIVO 
 * ************************** */
char getChar(void)
{
	int c = fgetc(progFonte);
	return minusculo((char)c);
}
