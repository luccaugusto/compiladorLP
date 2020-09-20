/* Analisador sintático
 * retorna 1 caso sucesso,
 *         0 caso erro,
 */

extern int erro;
extern int estado_sin;
extern int lex;


int casaToken(Tokens esperado)
{
	int retorno = 1;
	if (esperado == 0 && estado_sin != ACEITACAO_SIN) {
		/* chega EOF (0) mas não em estado de aceitação */
		/* Erro Sintático */
		erro = ERRO_SINTATICO_EOF;
		erroMsg = "fim de arquivo não esperado.";
	} else if (esperado != tokenAtual.token){
		/* Erro Sintático */
		erro = ERRO_SINTATICO;
		erroMsg = "token não esperado";
	}

	if (erro)
		retorno = 0;

	return retorno;
}

void iniciarAnSin()
{
	printf("INICIOU \n");
	lexan();
	declaracao();
}

void declaracao(void)
{
	if (tokenAtual.token == Var) {
		printf("%s\n",tokenAtual.lexema);
		variavel();
	} else if (tokenAtual.token == Const) {
		printf("%s\n",tokenAtual.lexema);
		constante();
	} else {
		blocoComando();
	}
}


void blocoComando()
{
	
}

void constante(void)
{
	lexan();
	if (casaToken(Identificador)) {
		printf("%s\n",tokenAtual.lexema);
		lexan();
		if (casaToken(Igual)) {
		printf("%s\n",tokenAtual.lexema);
			lexan();
			if (casaToken(Literal)) {
		printf("%s\n",tokenAtual.lexema);
				lexan();
				if (casaToken(PtVirgula)) {
		printf("%s\n",tokenAtual.lexema);
					declaracao();
				}
			}
		}
	}
}

void variavel(void)
{
	/* pega o proximo token */
	lexan();
	if (tokenAtual.token == Char || tokenAtual.token == Integer) {
		printf("%s\n",tokenAtual.lexema);
		listaIds();
	} else {
		erro = ERRO_SINTATICO;
		erroMsg = "token não esperado";
	}

}

void listaIds(void)
{
	/* TODO 
	* arrumar gramatica reconhecer atribuicoes e lista de ids
	*/
	lexan();
	if (tokenAtual.token == Identificador) {
		printf("%s\n",tokenAtual.lexema);
		lexan();
		if (casaToken(PtVirgula)) {
			printf("%s\n",tokenAtual.lexema);
			lexan();
			declaracao();
		} 
	}
}
