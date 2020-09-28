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

/* TODO Refatorar identificacao de array ou identificador */


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
	/* mostra a pilha de chamadas */
	printPilha();

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
	if (DEBUG_SIN) printf("Declaracao\n");
	/*push("Declaracao");*/
	/* var ou const */
	if (tokenAtual.token == Var) {
		lexan();
		variavel();
		declaracao();
	} else if (tokenAtual.token == Const) {
		lexan();
		constante();
		declaracao();
	} else {
		lexan();
		blocoComandos();
		fimDeArquivo();
	}
}


/* Bloco de comandos
 * For, If, ID= , ;, readln();, write();, writeln();
 */
void blocoComandos()
{
	if (DEBUG_SIN) printf("blocoComandos\n");
	/*push("blocoComandos");*/
	switch(tokenAtual.token)
	{
		case Identificador:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			atribuicao();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case For:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			repeticao();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case If:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			teste();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case PtVirgula:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			nulo();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case Readln:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			leitura();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case Write:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			escrita();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case Writeln:
			estado_sin = N_ACEITACAO_SIN;
			lexan();
			escritaLn();
			blocoComandos();
			estado_sin = ACEITACAO_SIN;
			break;

		case F_Chaves:
			/* encontrou o fim do bloco de comandos atual,
			 * retorna e deixa o metodo que chamou tratar o }
			 */
			estado_sin = ACEITACAO_SIN;
			return;

		default:
			return;
	}
}

/* EOF */
void fimDeArquivo(void)
{
	if (DEBUG_SIN) printf("fimDeArquivo\n");
	/*push("fimdearquivo");*/

	/* se lex nao for 0 ainda n leu o EOF */
	if (lex)
		erroSintatico(ERRO_SINTATICO);

	/* leu fim de arquivo mas nao em estado de aceitacao */
	else if (estado_sin != ACEITACAO_SIN)
		erroSintatico(ERRO_SINTATICO_EOF);

	sucesso();
}

/***********************************************
 *
 *  Procedimentos de Declaração
 *
 ***********************************************/

/* Const id = literal; */
void constante(void)
{
	if (DEBUG_SIN) printf("constante\n");
	/*push("constante");*/
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
	if (DEBUG_SIN) printf("variavel\n");
	/*push("variavel");*/
	estado_sin = N_ACEITACAO_SIN;
	if (tokenAtual.token == Char || tokenAtual.token == Integer) {
		lexan();
		listaIds();
	} else {
		erroSintatico(ERRO_SINTATICO);
	}
	estado_sin = ACEITACAO_SIN;
}


/* id;
 * id,id,...;
 * id=literal,...|;
 * id[int],...|;
 */
void listaIds(void)
{
	if (DEBUG_SIN) printf("listaIds\n");
	/*push("listaIds");*/
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
			/* else fim do comando */

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
					/* else fim do comando */
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
	if (DEBUG_SIN) printf("atribuicao\n");
	/*push("atribuicao");*/
	/* lendo array: id[i] */
	if (tokenAtual.token == A_Colchete) {
		lexan();
		if (tokenAtual.token == Identificador || tokenAtual.token == Literal) {
			lexan();
			if (casaToken(F_Colchete)) {
				lexan();
			}
		} else {
			erroSintatico(ERRO_SINTATICO);
		}
	}
	if (casaToken(Igual)) {
		lexan();
		if (tokenAtual.token == Identificador) {
			lexan();
			/* lendo array: id[i] */
			if (tokenAtual.token == A_Colchete) {
				lexan();
				if (tokenAtual.token == Identificador || tokenAtual.token == Literal) {
					lexan();
					if (casaToken(F_Colchete)) {
						lexan();
					}

				} else {
					erroSintatico(ERRO_SINTATICO);
				}
			}
			if (casaToken(PtVirgula)) {
				lexan();
			}

		} else if (casaToken(Literal)) {
			lexan();
			if (casaToken(PtVirgula)) {
				lexan();
			}
		}
	}
} 

/* Repeticao
 * ID = literal to literal repeticao1();
 */
void repeticao(void)
{
	if (DEBUG_SIN) printf("repeticao\n");
	/*push("repeticao");*/
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
	if (DEBUG_SIN) printf("repeticao1\n");
	/*push("repeticao1");*/
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
 * { blocoComandos() }
 * ou
 * comando unico
 */
void comandos2(void)
{
	if (DEBUG_SIN) printf("comandos2\n");
	/*push("comandos2");*/
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

		case A_Chaves:
			lexan();
			blocoComandos();
			/* o } ja foi lido por alguem na chamada antiga chamou */
			if (casaToken(F_Chaves)) {
				lexan();
			}
			break;

		case F_Chaves:
			/* encontrou o fim do bloco de comandos atual,
			 * retorna e deixa o metodo que chamou tratar o }
			 */
			return;

		default:
			erroSintatico(ERRO_SINTATICO);
	}
}

/* Teste
 * expressao() then comandos2() teste1()
 */
void teste(void)
{
	if (DEBUG_SIN) printf("teste\n");
	/*push("teste");*/
	expressao();
	/* then foi lido antes de retornar de expressao() */
	if (casaToken(Then)) {
		lexan();
		comandos2();
		if (tokenAtual.token == F_Chaves) {
			lexan(); 
		}
		teste1();
	}
}

/* else comandos2()
 * ou fim do if
 * blocoComandos()
 */
void teste1(void)
{
	if (DEBUG_SIN) printf("teste1\n");
	/*push("teste1");*/
	lexan();
	if (tokenAtual.token == Else) {
		lexan();
		comandos2();
	}
}

/* Comando de leitura
 * readln(id)
 */
void leitura(void)
{
	if (DEBUG_SIN) printf("leitura\n");
	/*push("leitura");*/
	if (casaToken(A_Parenteses)) {
		lexan();
		if (casaToken(Identificador)) {
			lexan();
			if (tokenAtual.token == A_Colchete) {
				/* lendo array: id[i] */
				lexan();
				if (tokenAtual.token == Identificador || tokenAtual.token == Literal) {
					lexan();
					if (casaToken(F_Colchete)) {
						lexan();
					}
				} else {
					erroSintatico(ERRO_SINTATICO);
				}
			}
			/* ja leu ( id|id[i] ) e pode fechar o comando */
			if (casaToken(F_Parenteses)) {
				lexan();
				if (casaToken(PtVirgula)) {
					lexan();
				}
			}
		}
	}
}

/* Comando nulo
 * ;
 */
void nulo(void)
{
	if (DEBUG_SIN) printf("nulo\n");
	/*push("nulo");*/
	if (casaToken(PtVirgula)) {
		lexan();
	}
}

/* Comando de escrita
 * write(id|const)
 */
void escrita(void)
{
	if (DEBUG_SIN) printf("escrita\n");
	/*push("escrita");*/
	if (casaToken(A_Parenteses)) {
		lexan();
		expressao2();
		if (casaToken(F_Parenteses)) {
			lexan();
			if (casaToken(PtVirgula)) {
				lexan();
			}
		}
	}
}

/* Funciona como um wrapper para o escrita.
 * a sintaxe dos dois é a mesma porém o escritaLn
 * deve colocar a quebra de linha no final.
 * Isso será tratado posteriormente, ao implementar
 * a geração de código
 * TODO colocar quebra de linha
 */
void escritaLn(void)
{
	if (DEBUG_SIN) printf("escritaLn\n");
	/*push("escritaLn");*/
	escrita();
}

void expressao(void)
{
	if (DEBUG_SIN) printf("expressao\n");
	/*push("expressao");*/
	if (tokenAtual.token == A_Parenteses) {
		lexan();
		expressao();
		lexan();
		if (casaToken(F_Parenteses)) {
			lexan();
		}
	} else if (tokenAtual.token == Identificador) {
		 /* id */
		lexan();
		/* lendo array: id[i] */
		if (tokenAtual.token == A_Colchete) {
			lexan();
			if (tokenAtual.token == Identificador || tokenAtual.token == Literal) {
				lexan();
				if (casaToken(F_Colchete)) {
					lexan();
					expressao1();
				}
			} else {
				erroSintatico(ERRO_SINTATICO);
			}
		} else {
			expressao1();
		}
	} else if (casaToken(Literal)) {
		lexan();
		expressao1();
	}
}

void expressao1(void)
{
	if (DEBUG_SIN) printf("expressao1\n");
	/* op id */
	if (tokenAtual.token == MaiorIgual ||
		tokenAtual.token == MenorIgual ||
		tokenAtual.token == Maior      ||
		tokenAtual.token == Menor      ||
		tokenAtual.token == Diferente  ||
		tokenAtual.token == Or         ||
		tokenAtual.token == Mais       ||
		tokenAtual.token == Menos      ||
		tokenAtual.token == Porcento   ||
		tokenAtual.token == Barra      ||
		tokenAtual.token == And        ||
		tokenAtual.token == Vezes      ||
		tokenAtual.token == Not        ||
		tokenAtual.token == Igual)
	{
		lexan();
		expressao(); 
	}
	/* leu lambda */
	/* id sozinho, retorna (lambda) */
}

/* lista de expressoes */
void expressao2(void)
{
	expressao();
	expressao3();
}

void expressao3(void)
{
	if (tokenAtual.token == Virgula) {
		lexan();
		expressao2();
	}
	/* else lambda */
}
