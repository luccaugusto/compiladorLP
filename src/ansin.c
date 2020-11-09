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

/* TODO 
 * permitir declaracao no for
 */

#ifndef _ANSIN
#define _ANSIN

	#include "ansin.h"
	#include "semac.c"
	#include "codegen.c"
	
	/* atribui posicao de acesso ao vetor no registro lexico */
	void atrPos(int pos)
	{
		tokenAtual.pos = pos;
	}
	
	/* atribui tipo à constante na tabela de simbolos */
	void atrTipo()
	{
		tokenAtual.endereco->simbolo.tipo = tokenAtual.tipo;
	}
	
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
			if (lex) erroSintatico(ER_SIN);
			else erroSintatico(ER_SIN_EOF);
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
	
		erro = tipo;
		switch (tipo)
		{
			case ER_SIN:
				erroMsg = "token nao esperado";
				break;
			case ER_SIN_EOF:
				erroMsg = "fim de arquivo nao esperado";
				break;
			case ER_SIN_NDEC:
				erroMsg = "identificador nao declarado";
				break;
			case ER_SIN_JADEC:
				erroMsg = "identificador ja declarado";
				break;
			case ER_SIN_TAMVET:
				erroMsg = "tamanho do vetor excede o maximo permitido";
				break;
			case ER_SIN_C_INC:
				erroMsg = "classe de identificador incompativel";
				break;
			case ER_SIN_T_INC :
				erroMsg = "tipos incompativeis";
				break;
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
		initDeclaracao();
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
		push("Declaracao",pilha);
	
		/* var ou const */
		if (tokenAtual.token == Var) {

			/* inicia bloco de declaracoes */
			if (!iniciouDec)
				initDeclaracao();

			lido=0;
			lexan();
			variavel();
			declaracao();
		} else if (tokenAtual.token == Const) {

			/* inicia bloco de declaracoes */
			if (!iniciouDec)
				initDeclaracao();

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
	
			fimDeclaracao();
			blocoComandos();
			fimDeArquivo();
		}
		del(pilha);
	}
	
	
	/* Bloco de comandos
	 * For, If, ID= , ;, readln();, write();, writeln();
	 */
	void blocoComandos()
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: blocoComandos\n");
		push("blocoComandos",pilha);
	
		switch(tokenAtual.token)
		{
			case Identificador:
				/* acao semantica */
				verificaDeclaracao(tokenAtual.lexema);
	
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
		del(pilha);
	}
	
	/* EOF */
	void fimDeArquivo(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: fimDeArquivo\n");
		push("fimdearquivo",pilha);
	
		/* se lex nao for 0 ainda n leu o EOF */
		/* leu fim de arquivo mas nao em estado de aceitacao */
		if (lex)
			erroSintatico(ER_SIN);
	
		if (estado_sin != ACEITACAO_SIN)
			erroSintatico(ER_SIN_EOF);
	
		sucesso();
		del(pilha);
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
		push("constante",pilha);

		int negativo = 0;
	
		defClasse(CL_Const);
	
		estado_sin = N_ACEITACAO_SIN;
		casaToken(Identificador);
	
		/* Ação semantica */
		verificaClasse();
	
		lexan();
		casaToken(Igual);         lexan();

		/* literal negativo */
		if (tokenAtual.token == Menos) {
			negativo = 1;
			lexan(); 
		}

		/* atribui tipo a constante */
		atrTipo();
		casaToken(Literal);

		/* codegen */
		genDeclaracao(tokenAtual.tipo,
				tokenAtual.classe,
				tokenAtual.tamanho,
				tokenAtual.lexema,
				negativo
				);

       	lexan();
		casaToken(PtVirgula);     lexan(); lido = 1;
		del(pilha);
	}
	
	/* var char|integer listaIds();
	 */
	void variavel(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: variavel\n");
		push("variavel",pilha);
	
		/* Ação semantica */
		defClasse(CL_Var);
	
		estado_sin = N_ACEITACAO_SIN;
		if (tokenAtual.token == Char || tokenAtual.token == Integer) {
			if (tokenAtual.token == Char) tokenAtual.tipo = TP_Char;
			else tokenAtual.tipo = TP_Integer;
	
			lexan();
			listaIds();
		} else {
			erroSintatico(ER_SIN);
		}
		estado_sin = ACEITACAO_SIN;
		del(pilha);
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
		push("listaIds",pilha);

		int negativo = 0;
	
		/* acao semantica */
		verificaClasse();
	
		casaToken(Identificador); lexan();
		if (tokenAtual.token == Virgula){
			/* Lendo id,id */

			/* codegen */
			genDeclaracao(tokenAtual.tipo,
					tokenAtual.classe,
					tokenAtual.endereco->simbolo.tamanho,
					NULL,
					negativo
			);

			lexan();
			listaIds();
	
		} else if (tokenAtual.token == PtVirgula) {
			/* lendo fim de um comando */

			/* codegen */
			genDeclaracao(tokenAtual.tipo,
					tokenAtual.classe,
					tokenAtual.endereco->simbolo.tamanho,
					NULL,
					negativo
			);

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

	 		/* literal negativo */
			if (tokenAtual.token == Menos) {
				negativo = 1;
				lexan();
			}
			casaToken(Literal); 
	
			/* acao semantica */
			verificaTipo(tokenAtual.endereco->simbolo.tipo, tokenAtual.tipo);

			/* codegen */
			genDeclaracao(tokenAtual.tipo,
					tokenAtual.classe,
					tokenAtual.endereco->simbolo.tamanho,
					tokenAtual.lexema,
					negativo
			);
	
			lexan();
			if (tokenAtual.token == Virgula) {
				/* outro id */
				lexan();
				listaIds();
			} else {
				/* terminou de ler o comando */
				casaToken(PtVirgula); lexan();
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
		} else {
			/* lendo id[literal] */
			casaToken(A_Colchete); lexan();
			casaToken(Literal);
	
			/* acao semantica */
			verificaTam();

	 		lexan();
			casaToken(F_Colchete); lexan();

			/* codegen */
			genDeclaracao(tokenAtual.tipo,
					tokenAtual.classe,
					tokenAtual.endereco->simbolo.tamanho,
					NULL,
					negativo
			);
	
	
			if (tokenAtual.token == Virgula) {
				/* outro id */
				lexan();
				listaIds();
			} else {
				/* terminou de ler o comando */
				casaToken(PtVirgula); lexan();
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
		del(pilha);
	}
	
	/***********************************************
	 *
	 *  Procedimentos de Bloco de comandos
	 *
	 ***********************************************/
	
	
	/* Atribuicao
	 * ID=expressao();
	 */
	void atribuicao(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: atribuicao\n");
		push("atribuicao",pilha);
	
		/* lendo array: id[expressao()] */
		if (tokenAtual.token == A_Colchete) {
			lexan();
	
			/* acao semantica */
			verificaTipo(expressao(), TP_Integer);
	
			atrPos(1);
			casaToken(F_Colchete); lexan();
		}
		casaToken(Igual); lexan();
	
		/* acao semantica */
		verificaTipo(expressao(), tokenAtual.endereco->simbolo.tipo);
		verificaAtrVetor();
	
		del(pilha);
	} 
	
	/* Repeticao
	 * ID = literal to literal repeticao1();
	 */
	void repeticao(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: repeticao\n");
		push("repeticao",pilha);
	
		casaToken(Identificador);
	
		/* acao semantica */
		verificaTipo(buscaTipo(tokenAtual.lexema),TP_Integer);
	
		lexan();
	
		/* lendo array: id[i] */
		if (tokenAtual.token == A_Colchete) {
			lexan();
	
			/* acao semantica */
			verificaTipo(expressao(),TP_Integer);
	
			casaToken(F_Colchete); lexan();
		}
	
		/* ja leu ( id|id[i] ) e pode fechar o comando */
		casaToken(Igual);   lexan();
		casaToken(Literal);
	
		/* acao semantica */
		verificaTipo(tokenAtual.tipo,TP_Integer);
	
	 	lexan();
		casaToken(To);      lexan();
	
		if (tokenAtual.token == Literal) {
			/* acao semantica */
			verificaTipo(tokenAtual.tipo, TP_Integer);
	
			lexan();
	
		} else if (tokenAtual.token == Identificador) {
			/* acao semantica */
			verificaTipo(buscaTipo(tokenAtual.lexema), TP_Integer);
	
			lexan();
	
			if (tokenAtual.token == A_Colchete) {
				/* lendo array: id[i] */
				lexan();
	
				/* acao semantica */
				verificaTipo(expressao(),TP_Integer);
	
				casaToken(F_Colchete); lexan();
			}
			/* ja leu ( id|id[i] ) e pode fechar o comando */
		}
	
		repeticao1();
		
	
		del(pilha);
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
		push("repeticao1",pilha);
	
		if (tokenAtual.token == Step) {
			lexan();
			casaToken(Literal); 
	
			/* acao semantica */
			verificaTipo(tokenAtual.tipo, TP_Integer);
	
			lexan();
	
		}
	
		casaToken(Do); lexan();
		comandos2();
		del(pilha);
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
		push("comandos2",pilha);
	
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
				casaToken(F_Chaves); lexan();
				break;
	
			case F_Chaves:
				/* encontrou o fim do bloco de comandos atual,
				 * retorna e deixa o metodo que chamou tratar o }
				 */
				return;
	
			default:
				if (lex) erroSintatico(ER_SIN);
				else erroSintatico(ER_SIN_EOF);
		}
		del(pilha);
	}
	
	/* Teste
	 * expressao() then comandos2() teste1()
	 */
	void teste(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: teste\n");
		push("teste",pilha);
	
		/* acao semantica */
		verificaTipo(expressao(),TP_Logico);
	
		/* then foi lido antes de retornar de expressao() */
		casaToken(Then);
		lexan();
		comandos2();
	
		if (tokenAtual.token == F_Chaves)
			lexan(); 
	
		teste1();
		del(pilha);
	}
	
	/* else comandos2()
	 * ou fim do if
	 * blocoComandos()
	 */
	void teste1(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: teste1\n");
		push("teste1",pilha);
	
		if (tokenAtual.token == Else) {
			lexan();
			comandos2();
		}
		del(pilha);
	}
	
	/* Comando de leitura
	 * readln(id)
	 */
	void leitura(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: leitura\n");
		push("leitura",pilha);
	
		casaToken(A_Parenteses);  lexan();
		casaToken(Identificador); lexan();
	
		if (tokenAtual.token == A_Colchete) {
			/* lendo array: id[i] */
			lexan();
			expressao();
			casaToken(F_Colchete); lexan();
		}
	
		/* ja leu ( id|id[i] ) e pode fechar o comando */
		casaToken(F_Parenteses); lexan();
		casaToken(PtVirgula); lexan();
	
		del(pilha);
	}
	
	/* Comando nulo
	 * ;
	 */
	void nulo(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: nulo\n");
		push("nulo",pilha);
	
		casaToken(PtVirgula); lexan();
		del(pilha);
	}
	
	/* Comando de escrita
	 * write(id|const)
	 */
	void escrita(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: escrita\n");
		push("escrita",pilha);
	
		casaToken(A_Parenteses); lexan();
		expressao2();
		casaToken(F_Parenteses); lexan();
		casaToken(PtVirgula);    lexan();
		del(pilha);
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
		if (DEBUG_SIN) printf("SIN: escritaLn\n");
		push("escritaLn",pilha);
	
		escrita();
		del(pilha);
	}
	
	/* le uma expressao e retorna o tipo final */
	Tipo expressao(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: expressao\n");
		push("expressao",pilha);
	
		/* verifica o tipo do valor atual com o tipo da expressao a direita */
		Tipo esq;
		Tipo ret;
	
		if (tokenAtual.token == A_Parenteses) {
			/* (expressao()) */
	
			lexan();
			ret = expressao();
			casaToken(F_Parenteses); lexan();
	
		} else if (tokenAtual.token == Identificador) {
			/* id */
	
			/* pega o tipo do id em questao */
			esq = buscaTipo(tokenAtual.lexema);
			lexan();
			/* lendo array: id[i] */
			if (tokenAtual.token == A_Colchete) {
				lexan();
				expressao();
				casaToken(F_Colchete); lexan();
			}
	
			ret = expressao1(esq);
	
		} else if (tokenAtual.token == Literal) {
			/* literal */
	
			esq = tokenAtual.tipo;
			lexan();
	
			ret = expressao1(esq);
	
		} else if (tokenAtual.token == Menos) {
			lexan();
			/* TODO inverter o sinal da expressao */
	
			/*acao semantica */
			verificaTipo(expressao(), TP_Integer);
		}
		/* else lambda */
	
		del(pilha);
		return ret;
	}
	
	/* operadores expressao();
	 * parametro esq: tipo da espressao do lado esquerdo
	 * retorna o tipo da expressao do lado direito do operador
	 */
	Tipo expressao1(Tipo esq)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: expressao1\n");
		push("expressao1",pilha);
	
		Tipo ret = esq;
	
		/* op id|literal */
		/* operadores exclusivos de inteiros que avaliam para logicos */
		if (tokenAtual.token == MaiorIgual ||
			tokenAtual.token == MenorIgual ||
			tokenAtual.token == Maior      ||
			tokenAtual.token == Menor)
		{
			lexan();
			
			/* acao semantica */
			verificaTipo(esq,TP_Integer);
			verificaTipo(expressao(),TP_Integer);
	
			ret = TP_Logico;
	
		/* operadores exclusivos de inteiros que avaliam para inteiros */
		} else if (
			tokenAtual.token == Mais       ||
			tokenAtual.token == Menos      ||
			tokenAtual.token == Porcento   ||
			tokenAtual.token == Barra      ||
			tokenAtual.token == Vezes)
		{
			lexan();
	
			ret = expressao();
			
			/* acao semantica */
			verificaTipo(esq,TP_Integer);
			verificaTipo(ret,TP_Integer);
	
		/* operadores exclusivos de tipo logico que avaliam para logicos*/
		} else if (
			tokenAtual.token == Or         ||
			tokenAtual.token == And        ||
			tokenAtual.token == Not) 
		{
			lexan();
			ret = expressao();
			
			/* acao semantica */
			verificaTipo(toLogico(ret),TP_Logico);
			ret = TP_Logico;
	
		/* Operadores sobre todos os tipos, avaliam para logicos */
		} else if (
			tokenAtual.token == Diferente  ||
			tokenAtual.token == Igual)
		{
	
			lexan();
			
			/* acao semantica */
			verificaTipo(expressao(),esq);
			ret = TP_Logico;
		}
	
		/* leu lambda */
		/* id sozinho, retorna (lambda) */
	
		del(pilha);
		return ret;
	}
	
	/* lista de expressoes */
	void expressao2(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: expressao2\n");
		push("expressao2",pilha);
	
		expressao();
		expressao3();
		del(pilha);
	}
	
	/* mais uma expressao ou lambda */
	void expressao3(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: expressao3\n");
		push("expressao3",pilha);
	
		if (tokenAtual.token == Virgula) {
			lexan();
			expressao2();
		}
		/* else lambda */
		del(pilha);
	}
#endif
