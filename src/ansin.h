/* Analisador sintático
 * retorna 1 caso sucesso,
 *         0 caso erro,
 */

extern int erro;
extern int estado_sin;

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

void ansin(void)
{
	return;
}
