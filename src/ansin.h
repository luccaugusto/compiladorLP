/* Analisador sintático
 * retorna 1 caso sucesso,
 *         0 caso erro,
 */

extern int erro;
extern int estado_sin;
extern int lex;


/* confere se o último token lido é esperado
 * Caso não seja o token esperado, aborta a
 * execução chamando erro()
 */
int casaToken(Tokens esperado)
{
	int retorno = 1;
	if (esperado == 0 && estado_sin != ACEITACAO_SIN) {
		/* chega EOF (0) mas não em estado de aceitação */
		/* Erro Sintático */
		erro = ERRO_SINTATICO_EOF;
		erroMsg = "fim de arquivo não esperado.";

		/* Aborta a compilação */
		abortar();
	} else if (esperado != tokenAtual.token){
		/* Erro Sintático */
		erro = ERRO_SINTATICO;
		erroMsg = "token não esperado";
		
		printf("ERRO SIN\n");
		/* Aborta a compilação */
		abortar();
	}

	return retorno;
}

void iniciarAnSin()
{
	/* consome o primeiro token */
	lexan();
	/* inicia pelo primeiro simbolo da gramatica */
	declaracao();
}

void declaracao(void)
{
	/* var ou const */
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

/* Const id = literal; */
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

/* var char|integer id;
 * var char|integer id,...;
 * var char|integer id=literal,id,...;
 */
void variavel(void)
{
	/* pega o proximo token */
	lexan();
	/* funciona para char ou integer */
	if (tokenAtual.token == Char || tokenAtual.token == Integer) {
		printf("%s\n",tokenAtual.lexema);
		listaIds();
	} else {
		erro = ERRO_SINTATICO;
		erroMsg = "token não esperado";
		abortar();
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
