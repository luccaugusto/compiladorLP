/* Analisador sintático
 * retorna 1 caso sucesso,
 *         0 caso erro,
 *
 * TODAS as leituras de lexemas são feitas fora
 * da chamada ao próximo procedimento de símbolo
 * 
 * O procedimento de símbolo não faz leitura do
 * simbolo que espera. Esse símbolo já deve ter
 * sido lido antes.
 */


extern int erro;
extern int estado_sin;
extern int lex;


/* confere se o último token lido é esperado
 * Caso não seja o token esperado, aborta a
 * execução chamando erroSintatico()
 *
 * essa função tem seu uso limitado pois
 * alguns estados aceitam mais de um Token
 * como por exemplo: Var char|integer.
 * Nesses casos casaToken nao pode ser
 * utilizada.
 * Quando só existe uma opção de Token
 * casaToken DEVE ser utilizada.
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
	}

	/* Aborta a compilação */
	abortar();
}

/* Consime o primeiro token e chama 
 * o simbolo inicial
 */
void iniciarAnSin(void)
{
	/* consome o primeiro token */
	lexan();
	/* inicia pelo primeiro simbolo da gramatica */
	declaracao();
}

/* Declaracao de variáveis ou constantes 
 * Var variavel();
 * Const constante();
 */
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


/* TODO */
void blocoComando()
{
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
	if (casaToken(Identificador)) {
		lexan();
		if (tokenAtual.token == Virgula){
			/* Lendo id,id */
			lexan();
			listaIds();
		} else if (tokenAtual.token == PtVirgula) {
			/* lendo id; */
			lexan();
			if (tokenAtual.token == Integer tokenAtual.token == Char)
				/* Lista de declaracoes tipo Var integer c; char d; */
				variavel();
			else
				/* fim do comando */
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
				} else if (casaToken(PtVirgula)) {
					/* terminou de ler o comando */
					lexan();
					if (tokenAtual.token == Integer tokenAtual.token == Char)
						/* Lista de declaracoes tipo Var integer c; char d; */
						variavel();
					else
						/* fim do comando */
						declaracao();
				}
			}
		} else if (casaToken(A_Colchete)) {
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
					} else if (casaToken(PtVirgula)) {
						/* terminou de ler o comando */
						lexan();
						variavel();
					}
				}
			}
		}
	}
}
