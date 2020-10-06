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
extern int lido;


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

	if (esperado != tokenAtual.token){
		if (lex) erroSintatico(ERRO_SINTATICO);
		else erroSintatico(ERRO_SINTATICO_EOF);
	}

	return retorno;
}

/* Trata um Erro Sintático
 * e aborta o programa
 */
void erroSintatico(int tipo)
{
	/* mostra a pilha de chamadas */
	/*printPilha(); */

	if (tipo == ERRO_SINTATICO) {
		erro = ERRO_SINTATICO;
		erroMsg = "token nao esperado";
	} else {
		erro = ERRO_SINTATICO_EOF;
		erroMsg = "fim de arquivo nao esperado";
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
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: Declaracao\n");
	push("Declaracao");


	/* var ou const */
	if (tokenAtual.token == Var) {
		lido=0;
		lexan();
		variavel();
		declaracao();
	} else if (tokenAtual.token == Const) {
		lido=0;
		lexan();
		constante();
		declaracao();
	} else {
		/* existem casos especificos onde o
	 	* token do bloco de comandos ja foi lido
	 	* e portanto nao precisa ser lido aqui,
	 	* conferir listaIds para ver a lista desses
	 	* casos 
	 	*
	 	* se ainda nao leu, le
	 	* se ja leu, utiliza o lexema lido
	 	* e marca que nao leu
	 	* */
		if (!lido) lexan();
		else lido = 0;

		blocoComandos();
		fimDeArquivo();
	}
	del();
}


/* Bloco de comandos
 * For, If, ID= , ;, readln();, write();, writeln();
 */
void blocoComandos()
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: blocoComandos\n");
	push("blocoComandos");

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
	del();
}

/* EOF */
void fimDeArquivo(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: fimDeArquivo\n");
	push("fimdearquivo");


	/* se lex nao for 0 ainda n leu o EOF */
	if (lex)
		erroSintatico(ERRO_SINTATICO_EOF);

	/* leu fim de arquivo mas nao em estado de aceitacao */
	if (estado_sin != ACEITACAO_SIN)
		erroSintatico(ERRO_SINTATICO_EOF);

	sucesso();
	del();
}

/***********************************************
 *
 *  Procedimentos de Declaração
 *
 ***********************************************/

/* Const id = literal; */
void constante(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: constante\n");
	push("constante");

	estado_sin = N_ACEITACAO_SIN;
	casaToken(Identificador); lexan();
	casaToken(Igual);         lexan();
	casaToken(Literal);       lexan();
	casaToken(PtVirgula);
	del();
}

/* var char|integer listaIds();
 */
void variavel(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: variavel\n");
	push("variavel");

	estado_sin = N_ACEITACAO_SIN;
	if (tokenAtual.token == Char || tokenAtual.token == Integer) {
		lexan();
		listaIds();
	} else {
		erroSintatico(ERRO_SINTATICO);
	}
	estado_sin = ACEITACAO_SIN;
	del();
}


/* id;
 * id,id,...;
 * id=literal,...|;
 * id[int],...|;
 */
void listaIds(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: listaIds\n");
	push("listaIds");

	casaToken(Identificador); lexan();

	if (tokenAtual.token == Virgula){
		/* Lendo id,id */
		lexan();
		listaIds();

	} else if (tokenAtual.token == PtVirgula) {
		/* lendo fim de um comando */
		lexan();
		/* Lista de declaracoes tipo Var integer c; char d; */
		if (tokenAtual.token == Integer || tokenAtual.token == Char)
			variavel();
		else
			/* fim do comando e marca lido como 1
			 * pois leu um lexema que nao
			 * foi utilizado aqui, portanto
			 * o proximo metodo nao precisa ler
			 * este lexema
			 * */
			lido = 1;

	} else if (tokenAtual.token == Igual) {
		/* lendo id=literal */
		lexan();
		casaToken(Literal); lexan();
		if (tokenAtual.token == Virgula) {
			/* outro id */
			lexan();
			listaIds();
		} else if (casaToken(PtVirgula)) {
			/* terminou de ler o comando */
			lexan();
			/* Lista de declaracoes tipo Var integer c; char d; */
			if (tokenAtual.token == Integer || tokenAtual.token == Char)
				variavel();
			else
				/* fim do comando e marca lido como 1
			 	* pois leu um lexema que nao
			 	* foi utilizado aqui, portanto
			 	* o proximo metodo nao precisa ler
			 	* este lexema
			 	* */
				lido = 1;
		}
	} else if (casaToken(A_Colchete)) {
		/* lendo id[int] */
		lexan();
		casaToken(Literal);    lexan();
		casaToken(F_Colchete); lexan();

		if (tokenAtual.token == Virgula) {
			/* outro id */
			lexan();
			listaIds();
		} else if (casaToken(PtVirgula)) {
			/* terminou de ler o comando */
			lexan();
			/* Lista de declaracoes tipo Var integer c; char d; */
			if (tokenAtual.token == Integer || tokenAtual.token == Char)
				variavel();
			else
				/* fim do comando e marca lido como 1
			 	* pois leu um lexema que nao
			 	* foi utilizado aqui, portanto
			 	* o proximo metodo nao precisa ler
			 	* este lexema
			 	* */
				lido = 1;
		}
	}
	del();
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
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: atribuicao\n");
	push("atribuicao");

	/* lendo array: id[i] */
	if (tokenAtual.token == A_Colchete) {
		lexan();
		if (tokenAtual.token == Identificador || tokenAtual.token == Literal) {
			lexan();
			casaToken(F_Colchete); lexan();
		} else {
			erroSintatico(ERRO_SINTATICO);
		}
	}
	casaToken(Igual); lexan();
	if (tokenAtual.token == Identificador) {
		lexan();
		/* lendo array: id[i] */
		if (tokenAtual.token == A_Colchete) {
			lexan();
			if (tokenAtual.token == Identificador || tokenAtual.token == Literal) {
				lexan();
				casaToken(F_Colchete); lexan();
			} else {
				erroSintatico(ERRO_SINTATICO);
			}
		}
		casaToken(PtVirgula); lexan();

	} else if (casaToken(Literal)) {
		lexan();
		casaToken(PtVirgula); lexan();
	}
	del();
} 

/* Repeticao
 * ID = literal to literal repeticao1();
 */
void repeticao(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: repeticao\n");
	push("repeticao");

	casaToken(Identificador); lexan();
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
	casaToken(Igual);   lexan();
	casaToken(Literal); lexan();
	casaToken(To);      lexan();

	if (tokenAtual.token == Literal) {
		lexan();
		repeticao1();
	} else if (tokenAtual.token == Identificador) {
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
		repeticao1();

	}
	del();
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
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: repeticao1\n");
	push("repeticao1");

	if (tokenAtual.token == Step) {
		lexan();
		casaToken(Literal); lexan();
		casaToken(Do);      lexan();
		comandos2();
	} else if (casaToken(Do)) {
		lexan();
		comandos2();
	}
	del();
}

/* R1 na gramatica
 * { blocoComandos() }
 * ou
 * comando unico
 */
void comandos2(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: comandos2\n");
	push("comandos2");

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
			if (lex) erroSintatico(ERRO_SINTATICO);
			else erroSintatico(ERRO_SINTATICO_EOF);
	}
	del();
}

/* Teste
 * expressao() then comandos2() teste1()
 */
void teste(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: teste\n");
	push("teste");

	expressao();
	/* then foi lido antes de retornar de expressao() */
	casaToken(Then);
	lexan();
	comandos2();

	if (tokenAtual.token == F_Chaves)
		lexan(); 

	teste1();
	del();
}

/* else comandos2()
 * ou fim do if
 * blocoComandos()
 */
void teste1(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: teste1\n");
	push("teste1");

	lexan();
	if (tokenAtual.token == Else) {
		lexan();
		comandos2();
	}
	del();
}

/* Comando de leitura
 * readln(id)
 */
void leitura(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: leitura\n");
	push("leitura");

	casaToken(A_Parenteses);  lexan();
	casaToken(Identificador); lexan();

	if (tokenAtual.token == A_Colchete) {
		/* lendo array: id[i] */
		lexan();
		if (tokenAtual.token == Identificador || tokenAtual.token == Literal) {
			lexan();
			casaToken(F_Colchete); lexan();
		} else {
			erroSintatico(ERRO_SINTATICO);
		}
	}
	/* ja leu ( id|id[i] ) e pode fechar o comando */
	if (casaToken(F_Parenteses)) {
		lexan();
		casaToken(PtVirgula); lexan();
	}
	del();
}

/* Comando nulo
 * ;
 */
void nulo(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: nulo\n");
	push("nulo");

	casaToken(PtVirgula); lexan();
	del();
}

/* Comando de escrita
 * write(id|const)
 */
void escrita(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("escrita\n");
	push("escrita");

	casaToken(A_Parenteses); lexan();
	expressao2();
	casaToken(F_Parenteses); lexan();
	casaToken(PtVirgula);    lexan();
	del();
}

/* Funciona como um wrapper para o escrita.
 * a sintaxe dos dois é a mesma porém o escritaLn
 * deve colocar a quebra de linha no final.
 * Isso será tratado posteriormente, ao implementar
 * a geração de código
 * TODO colocar quebra de linha na geracao de codigo
 */
void escritaLn(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("escritaLn\n");
	push("escritaLn");

	escrita();
	del();
}

void expressao(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("expressao\n");
	push("expressao");

	if (tokenAtual.token == A_Parenteses) {
		lexan();
		expressao();
		lexan();
		casaToken(F_Parenteses); lexan();
	} else if (tokenAtual.token == Identificador) {
		 /* id */
		lexan();
		/* lendo array: id[i] */
		if (tokenAtual.token == A_Colchete) {
			lexan();
			if (tokenAtual.token == Identificador || tokenAtual.token == Literal) {
				lexan();
				casaToken(F_Colchete); lexan();
				expressao1();
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
	del();
}

void expressao1(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: expressao1\n");
	push("expressao1");

	/* op id|literal */
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
	del();
}

/* lista de expressoes */
void expressao2(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: expressao2\n");
	push("expressao2");

	expressao();
	expressao3();
	del();
}

/* mais uma expressao ou lambda */
void expressao3(void)
{
	/* DEBUGGER E PILHA */
	if (DEBUG_SIN) printf("SIN: expressao3\n");
	push("expressao3");

	if (tokenAtual.token == Virgula) {
		lexan();
		expressao2();
	}
	/* else lambda */
	del();
}
