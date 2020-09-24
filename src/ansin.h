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
	estado_sin = N_ACEITACAO_SIN;
	/* var ou const */
	if (tokenAtual.token == Var) {
		lexan();
		variavel();
	} else if (tokenAtual.token == Const) {
		lexan();
		constante();
	} else {
		lexan();
		blocoComando();
	}
	estado_sin = ACEITACAO_SIN;
}


/* Bloco de comandos
 * For, If, ID= , ;, readln();, write();, writeln();
 */
void blocoComando()
{
	switch(tokenAtual.token)
	{
		case Identificador:
			lexan();
			atribuicao();
			break;

		case For:
			lexan();
			repeticao();
			break;

		case If:
			lexan();
			teste();
			break;

		case PtVirgula:
			lexan();
			nulo();
			break;

		case Readln:
			lexan();
			leitura();
			break;

		case Write:
			lexan();
			escrita();
			break;

		case Writeln:
			lexan();
			escritaLn();
			break;

		default:
			erroSintatico(ERRO_SINTATICO);
	}
}

/* EOF */
void fimDeArquivo(void)
{
	if (estado_sin != ACEITACAO_SIN) {
		erroSintatico(ERRO_SINTATICO_EOF);
	}
	return;
}

/***********************************************
 *
 *  Procedimentos de Declaração
 *
 ***********************************************/

/* Const id = literal; */
void constante(void)
{
	estado_sin = N_ACEITACAO_SIN;
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
	estado_sin = N_ACEITACAO_SIN;
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
			if (tokenAtual.token == Integer || tokenAtual.token == Char)
				/* Lista de declaracoes tipo Var integer c; char d; */
				variavel();
			else
				/* fim do comando */
				estado_sin = ACEITACAO_SIN;
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
					if (tokenAtual.token == Integer || tokenAtual.token == Char)
						/* Lista de declaracoes tipo Var integer c; char d; */
						variavel();
					else
						/* fim do comando */
						estado_sin = ACEITACAO_SIN;
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
						if (tokenAtual.token == Var || tokenAtual.token == Const) 
							/* var|const integer id[2]; var|const char ... */
							declaracao();
						else /* integer id[2]; integer listaIds()... */
							estado_sin = ACEITACAO_SIN;
							variavel();

					}
				}
			}
		}
	}
	estado_sin = ACEITACAO_SIN;
}


/***********************************************
 *
 *  Procedimentos de Bloco de comandos
 *
 ***********************************************/

/* Atribuicao
 * ID=literal;
 */
void atribuicao(void)
{
	if (casaToken(Identificador)) {
		lexan();
		if (casaToken(Igual)) {
			lexan();
			if (casaToken(Literal)) {
				lexan();
				if (casaToken(PtVirgula)) {
					estado_sin = ACEITACAO_SIN;
					lexan();
					blocoComando();
				}
			}
		}
	}
} 

/* Repeticao
 * For ID = literal to literal repeticao1();
 */
void repeticao(void)
{
	if (casaToken(Identificador)) {
		lexan();
		if (casaToken(Igual)) {
			lexan();
			if (casaToken(Literal)) {
				lexan();
				if (casaToken(To)) {
					lexan();
					if (casaToken(Literal)) {
						lexan();
						repeticao1();
					}
				}
			}
		}
	}
}

/* 
 * R2 na gramatica
 *
 * step literal do comandos2();
 * ou
 * do comandos2();
 */
void repeticao1(void)
{
	if (tokenAtual.token == Step) {
		lexan();
		if (casaToken(Literal)) {
			lexan();
			if (casaToken(Do)) {
				lexan();
				comandos2();
			}
		}
	} else if (casaToken(Do)) {
		lexan();
		comandos2();
	}
}

/* R1 na gramatica
 * { blocoComando() }
 * ou
 * comando unico
 */
void comandos2(void)
{
	switch(tokenAtual.token)
	{
		case Identificador:
			lexan();
			atribuicao();
			estado_sin = ACEITACAO_SIN;
			break;

		case For:
			lexan();
			repeticao();
			estado_sin = ACEITACAO_SIN;
			break;

		case If:
			lexan();
			teste();
			estado_sin = ACEITACAO_SIN;
			break;

		case PtVirgula:
			lexan();
			nulo();
			estado_sin = ACEITACAO_SIN;
			break;

		case Readln:
			lexan();
			leitura();
			estado_sin = ACEITACAO_SIN;
			break;

		case Write:
			lexan();
			escrita();
			estado_sin = ACEITACAO_SIN;
			break;

		case Writeln:
			lexan();
			escritaLn();
			estado_sin = ACEITACAO_SIN;
			break;

		case A_Chaves:
			lexan();
			blocoComando();
			lexan();
			if (casaToken(F_Chaves)) {
				lexan();
				blocoComando();
				estado_sin = ACEITACAO_SIN;
			}
			break;

		default:
			erroSintatico(ERRO_SINTATICO);
	}
}

/* Teste
 * if expressao() then comandos2() teste1()
 */
void teste(void)
{
	lexan();
	expressao();
	lexan();
	if (casaToken(Then)) {
		comandos2();
		lexan();
		teste1();
	}
}

/* else comandos2()
 * ou fim do if
 * blocoComando()
 */
void teste1(void)
{
	lexan();
	if (tokenAtual.token == Else) {
		comandos2();
	} else {
		blocoComando();
	}
}

void leitura(void)
{
	return;
}

void nulo(void)
{
	return;
}

void escrita(void)
{
	return;
}

void escritaLn(void)
{
	return;
}

void expressao(void)
{
	return;
}
