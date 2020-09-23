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
		erroSintatico(ERRO_SINTATICO_EOF);

	} else if (esperado != tokenAtual.token){

		erroSintatico(ERRO_SINTATICO);

	}

	return retorno;
}

/* Trata um Erro Sintático
 * e aborta o programa
 */
void erroSintatico(int tipo)
{
	if (tipo == ERRO_SINTATICO) {
		erro = ERRO_SINTATICO;
		erroMsg = "token não esperado";
		
	} else {
		erro = ERRO_SINTATICO_EOF;
		erroMsg = "fim de arquivo não esperado.";

		/* Aborta a compilação */
		abortar();
	}

	/* Aborta a compilação */
	abortar();
}

void iniciarAnSin(void)
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
		lexan();
		variavel();
	} else if (tokenAtual.token == Const) {
		lexan();
		constante();
	} else {
		blocoComando();
	}
}


void blocoComando()
{
	if (casaToken(If)) {
		
	} else {
		erroSintatico(ERRO_SINTATICO);
	}
}

/* Const id = literal; */
void constante(void)
{
	if (casaToken(Identificador)) {
		lexan();
		if (casaToken(Igual)) {
			lexan();
			if (casaToken(Literal)) {
				lexan();
				if (casaToken(PtVirgula)) {
					declaracao();
				}
			}
		}
	}
}

/* var char|integer listaIds();
 */
void variavel(void)
{
	if (tokenAtual.token == Char || tokenAtual.token == Integer) {
		lexan();
		listaIds();
	} else {
		erroSintatico(ERRO_SINTATICO);
	}

}


/* id;
 * id,id,...;
 * id=literal,...|;
 * id[int],...|;
 */
void listaIds(void)
{
	if (tokenAtual.token == Identificador) {
		lexan();
		if (tokenAtual.token == Virgula){
			/* Lendo id,id */
			lexan();
			listaIds();
		} else if (tokenAtual.token == PtVirgula) {
			/* lendo id; */
			lexan();
			declaracao();
		} else if (tokenAtual.token == Igual) {
			/* lendo id=literal */
			lexan();
			if (casaToken(Literal)) {
				lexan();
				if (tokenAtual.token == Virgula) {
					/* outro id */
					lexan();
					listaIds();
				} else if (tokenAtual.token == PtVirgula) {
					/* terminou de ler o comando */
					lexan();
					declaracao();
				} else {
					erroSintatico(ERRO_SINTATICO);
				}
			} else {
				erroSintatico(ERRO_SINTATICO);
			}
		} else if (tokenAtual.token == A_Colchete) {
			/* lendo id[int] */
			lexan();
			if (casaToken(Literal)) {
				lexan();
				if (casaToken(F_Colchete)) {
					lexan();
					if (tokenAtual.token == Virgula) {
						/* outro id */
						lexan();
						listaIds();
					} else if (tokenAtual.token == PtVirgula) {
						/* terminou de ler o comando */
						lexan();
						declaracao();
					} else {
						erroSintatico(ERRO_SINTATICO);
					}
				} else {
					erroSintatico(ERRO_SINTATICO);
				}
			} else {
				erroSintatico(ERRO_SINTATICO);
			}
			
		} else {
			erroSintatico(ERRO_SINTATICO);
		}
	} else {
		erroSintatico(ERRO_SINTATICO);
	}
}
