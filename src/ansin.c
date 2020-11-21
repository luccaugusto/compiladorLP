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

#ifndef _ANSIN
#define _ANSIN

	#include "ansin.h"
	#include "semac.c"
	#include "codegen.c"
	
	char* lexAux;
	
	/* atribui posicao de acesso ao vetor no registro lexico */
	void atrPos(int pos)
	{
		regLex.pos = pos;
	}
	
	/* atribui tipo à constante na tabela de simbolos */
	void atrTipo()
	{
		regLex.endereco->simbolo.tipo = regLex.tipo;
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
	
		if (esperado != regLex.token){
			if (lex) erroSintatico(ER_SIN);
			else erroSintatico(ER_SIN_EOF);
		}
	
		lexan();
		return retorno;
	}
	
	/* Trata um Erro Sintático
	 * e aborta o programa
	 */
	void erroSintatico(int tipo)
	{
	
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
				erroMsg = "tamanho do vetor excede o máximo permitido";
				break;
			case ER_SIN_C_INC:
				erroMsg = "classe de identificador incompatível";
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
	
		/* codegen: inicia bloco de declaracoes */
		initDeclaracao();
	
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
		DEBUGSIN("declaracao");
	
		/* var ou const */
		if (regLex.token == Var) {
	
			lido=0;
			lexan();
			variavel();
			declaracao();
	
		} else if (regLex.token == Const) {
	
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
	
			/* codegen: finaliza bloco de declaracoes e 
			 * inicializa bloco do programa 
			 */
			fimDecInitCom();
	
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
		DEBUGSIN("blocoComandos");
	
		switch(regLex.token)
		{
			case Identificador:
				/* acao semantica */
				verificaDeclaracao(regLex.lexema);
				verificaConst(regLex.lexema);

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
				escrita(0);
				blocoComandos();
				estado_sin = ACEITACAO_SIN;
				break;
	
			case Writeln:
				estado_sin = N_ACEITACAO_SIN;
				lexan();
				escrita(1);
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
		DEBUGSIN("fimDeArquivo");
	
	
		/* se lex nao for 0 ainda n leu o EOF */
		/* leu fim de arquivo mas nao em estado de aceitacao */
		if (lex)
			erroSintatico(ER_SIN);
	
		if (estado_sin != ACEITACAO_SIN)
			erroSintatico(ER_SIN_EOF);
	
		/* codegen: finaliza o programa */
		fimComandos();
	
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
		DEBUGSIN("constante");
	
		/* salva o lexema atual e o tipo para verificacao da classe */
		lexAux = regLex.lexema;
		Tipo t = regLex.tipo;
	
		int negativo = 0;
	
		defClasse(CL_Const);
	
		estado_sin = N_ACEITACAO_SIN;
		casaToken(Identificador);
	
		/* Ação semantica */
		verificaClasse(lexAux, t);
	
		casaToken(Igual);
	
		/* literal negativo */
		if (regLex.token == Menos) {
			negativo = 1;
			lexan(); 
		}
	
		/* atribui tipo a constante */
		atrTipo();
	
		lexAux = removeComentario(lexemaLido);
		casaToken(Literal);
	
		/* codegen */
		genDeclaracao(regLex.tipo,
				regLex.classe,
				regLex.tamanho,
				lexAux,
				negativo
				);
	
		casaToken(PtVirgula); lido = 1;
		del(pilha);
	}
	
	/* var char|integer listaIds();
	*/
	void variavel(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("variavel");
	
		/* Ação semantica */
		defClasse(CL_Var);
	
		estado_sin = N_ACEITACAO_SIN;
		if (regLex.token == Char || regLex.token == Integer) {

			if (regLex.token == Char) regLex.tipo = TP_Char;
			else regLex.tipo = TP_Integer;
	
			lexan();
			listaIds(regLex.tipo);
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
	 *
	 * recebe o ultimo tipo lido como parametro
	 * pois o tipo do registro lexico pode mudar
	 * em casos id = literal, nesse caso regLex.tipo
	 * vai ser o tipo do literal
	 */
	void listaIds(Tipo ultimoTipo)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("listaIds");
	
		Tipo t;
		int negativo = 0;
	
		/* acao semantica */
		verificaClasse(regLex.lexema, ultimoTipo);
	
		casaToken(Identificador);
		if (regLex.token == Virgula){
			/* Lendo id,id */
	
			/* codegen */
			genDeclaracao(
					regLex.endereco->simbolo.tipo,
					regLex.classe,
					regLex.endereco->simbolo.tamanho,
					NULL,
					negativo
					);
	
			lexan();
			listaIds(ultimoTipo);
	
		} else if (regLex.token == PtVirgula) {
			/* lendo fim de um comando */
	
			/* codegen */
			genDeclaracao(
					regLex.endereco->simbolo.tipo,
					regLex.classe,
					regLex.endereco->simbolo.tamanho,
					NULL,
					negativo
					);
	
			lexan();
			/* Lista de declaracoes tipo Var integer c; char d; */
			if (regLex.token == Integer || regLex.token == Char)
				variavel();
			else
				/* fim do comando e marca lido como 1
				 * pois leu um lexema que nao
				 * foi utilizado aqui, portanto
				 * o proximo metodo nao precisa ler
				 * este lexema
				 * */
				lido = 1;
	
		} else if (regLex.token == Igual) {
			/* lendo id=literal */
			lexan();
	
			/* literal negativo */
			if (regLex.token == Menos) {
				negativo = 1;
				lexan();
			}
	
			t = regLex.tipo;
			lexAux = regLex.lexema;
	
			casaToken(Literal);
	
			/* acao semantica */
			verificaTipo(regLex.endereco->simbolo.tipo, t);
	
			/* codegen */
			genDeclaracao(
					t,
					regLex.classe,
					regLex.endereco->simbolo.tamanho,
					lexAux,
					negativo
					);
	
			if (regLex.token == Virgula) {
				/* outro id */
				lexan();
				listaIds(ultimoTipo);
			} else {
				/* terminou de ler o comando */
				casaToken(PtVirgula);
	
				/* Lista de declaracoes tipo Var integer c; char d; */
				if (regLex.token == Integer || regLex.token == Char)
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
			casaToken(A_Colchete);
	
			lexAux = regLex.lexema;
			casaToken(Literal);
	
			/* acao semantica */
			verificaTam(str2int(lexAux));
	
			casaToken(F_Colchete);
	
			/* codegen */
			genDeclaracao(
					regLex.endereco->simbolo.tipo,
					regLex.classe,
					regLex.endereco->simbolo.tamanho,
					NULL,
					negativo
					);
	
	
			if (regLex.token == Virgula) {
				/* outro id */
				lexan();
				listaIds(ultimoTipo);
			} else {
				/* terminou de ler o comando */
				casaToken(PtVirgula);
	
				/* Lista de declaracoes tipo Var integer c; char d; */
				if (regLex.token == Integer || regLex.token == Char)
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
		DEBUGSIN("atribuicao");
	
		NOVO_FATOR(pai);
		NOVO_FATOR(expr);  /* fator da expressao do lado direito */
		int tipoId = regLex.endereco->simbolo.tipo;
		pai->tipo = tipoId;

		/* codegen
		 * salva no pai o endereco de id
		 * para caso leia outro id na expressao de atribuicao
		 *
		 * zera os temporarios
		 */
		pai->endereco = regLex.endereco->simbolo.memoria;
		zeraTemp();
	
		/* lendo array: id[expressao()] */
		if (regLex.token == A_Colchete) {
			lexan();
	
			NOVO_FATOR(aux);

			aux = expressao();
			/* acao semantica */
			verificaTipo(aux->tipo, TP_Integer);
	
			/* marca como array */
			atrPos(1);
			casaToken(F_Colchete);
			fatorGeraArray(pai, aux, regLex.endereco->simbolo.lexema);
		}

		casaToken(Igual);
	
	
		expr = expressao();

		/* acao semantica */
		verificaTipo(expr->tipo, tipoId);
		verificaAtrVetor();

		/* codegen */
		genAtribuicao(pai,expr);
	
		del(pilha);
		casaToken(PtVirgula);
	} 
	
	/* Repeticao
	 * ID = literal to literal repeticao1();
	 */
	void repeticao(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("repeticao");
	
		Tipo t = regLex.endereco->simbolo.tipo;
		NOVO_FATOR(pai);
		NOVO_FATOR(filho);
		NOVO_FATOR(filho2);
	
		lexAux = regLex.lexema;

		/* codegen
		 * salva o endereco do id no pai
		 * para caso leia outro id na expressao de atribuicao
		 * e zera temporarios
		 */
		pai->endereco = regLex.endereco->simbolo.memoria;
		pai->tipo = t;
		zeraTemp();

		casaToken(Identificador);

		/* acao semantica */
		verificaDeclaracao(lexAux);
		verificaConst(lexAux);
		verificaTipo(t,TP_Integer);
	
		/* lendo array: id[i] */
		if (regLex.token == A_Colchete) {
			lexan();

			NOVO_FATOR(aux);
			aux = expressao();
	
			/* acao semantica */
			verificaTipo(aux->tipo,TP_Integer);

			/* codegen */
			acessoArray(pai, aux);
	
			casaToken(F_Colchete);
		}
	
		/* ja leu ( id|id[i] ) e pode fechar o comando */
		casaToken(Igual);
	
		filho = expressao();

		/* codegen */
		genAtribuicao(pai, filho);
	
		/* acao semantica */
		verificaTipo(filho->tipo,TP_Integer);
	
		casaToken(To);

		filho2 = expressao();

		/* codegen */
		rot inicio = novoRot();
		rot fim = novoRot();
		genRepeticao(pai,filho2,inicio,fim);
	
		repeticao1(pai, inicio, fim);
	
		del(pilha);
	}
	
	/* 
	 * step literal do comandos2();
	 * ou
	 * do comandos2();
	 */
	void repeticao1(struct Fator *pai, rot inicio, rot fim)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("repeticao1");
	
		Tipo t;
		char *step = "1";
	
		if (regLex.token == Step) {
			lexan();

			/* guarda o step */
			t = regLex.tipo;
			step = regLex.lexema;

			casaToken(Literal);
	
			/* acao semantica */
			verificaTipo(t, TP_Integer);
	
		}
	
		casaToken(Do);
		comandos2();

		/* codegen */
		genFimRepeticao(pai, inicio, fim, step);

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
		DEBUGSIN("comandos2");
	
		switch(regLex.token)
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
				escrita(0);
				break;
	
			case Writeln:
				lexan();
				escrita(1);
				break;
	
			case A_Chaves:
				lexan();
				blocoComandos();
				/* o } ja foi lido por alguem na chamada antiga chamou */
				casaToken(F_Chaves);
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
		DEBUGSIN("teste");

		NOVO_FATOR(expr);
		rot falso = novoRot();
		rot fim = novoRot();

		/* codegen */
		zeraTemp();

		expr = expressao();
	
		/* acao semantica */
		verificaTipo(expr->tipo, TP_Logico);

		/* codegen */
		genTeste(expr, falso, fim);
	
		/* then foi lido antes de retornar de expressao() */
		casaToken(Then);
	
		comandos2();
	
		if (regLex.token == F_Chaves)
			lexan(); 
	
		teste1(falso, fim);
		del(pilha);
	}
	
	/* else comandos2()
	 * ou fim do if
	 * blocoComandos()
	 */
	void teste1(rot falso, rot fim)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("teste1");

		genElseTeste(falso, fim);
	
		if (regLex.token == Else) {
			lexan();
			comandos2();
		}

		genFimTeste(fim);
		del(pilha);
	}
	
	/* Comando de leitura
	 * readln(id)
	 */
	void leitura(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("leitura");
		NOVO_FATOR(pai);
		NOVO_FATOR(expr);
		char *lexId;
		struct Celula *registro;
	
		casaToken(A_Parenteses);

		lexId = regLex.lexema;
		registro = pesquisarRegistro(lexId);

		/* acao semantica */
		verificaConst(lexId);
		verificaDeclaracao(lexId);

		casaToken(Identificador);

		/* codegen */
		pai->endereco = registro->simbolo.memoria;
		pai->tipo = registro->simbolo.tipo;
		pai->tamanho = registro->simbolo.tamanho;
		zeraTemp();
	
		if (regLex.token == A_Colchete) {
			/* lendo array: id[i] */
			lexan();
			expr = expressao();
			casaToken(F_Colchete);

			/* acao semantica */
			verificaTipo(expr->tipo, TP_Integer);
			fatorGeraArray(pai, expr, lexId);
		}
	
		/* ja leu ( id|id[i] ) e pode fechar o comando */
		casaToken(F_Parenteses);
		casaToken(PtVirgula);

		/* codegen */
		genEntrada(pai);
	
		del(pilha);
	}
	
	/* Comando nulo
	 * ;
	 */
	void nulo(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("nulo");
	
		casaToken(PtVirgula);
		del(pilha);
	}
	
	/* Comando de escrita
	 * write(id|const)
	 */
	void escrita(int ln)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("escrita");
	
		zeraTemp();
		casaToken(A_Parenteses);
		expressao2(ln);
		casaToken(F_Parenteses);
		casaToken(PtVirgula);
		del(pilha);
	}
	
	/* le uma expressao e retorna o tipo final
	 * Expressão
	 * X -> Xs [ O Xs ]
	 * O  -> = | <> | < | > | >= | <=
	 */
	struct Fator *expressao(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("expressao");

		NOVO_FATOR(ret);
		NOVO_FATOR(filho);

		filho = expressaoS();
		
		/* codegen */
		atualizaPai(ret,filho);

		/* operacoes tipo x tipo -> logico */
		if (regLex.token == Igual || regLex.token == Diferente) {

			/* codegen */
			guardaOp(ret);

			lexan();

			NOVO_FATOR(filho2);
			filho2 = expressaoS();

			/* acao semantica */
			verificaTipo(ret->tipo, filho2->tipo);

			/* codegen */
			genOpTermos(ret,filho2);

			ret->tipo = TP_Logico;
		}

		/* operacoes tipo int x int -> logico */
		else if (regLex.token == Menor || regLex.token == Maior ||
				regLex.token == MaiorIgual || regLex.token == MenorIgual) 
		{

			/* codegen */
			guardaOp(ret);

			lexan();

			NOVO_FATOR(filho2);
			filho2 = expressaoS();

			/* acao semantica */
			verificaTipo(ret->tipo,TP_Integer);
			verificaTipo(filho2->tipo,TP_Integer);

			/* codegen */
			genOpTermos(ret,filho2);

			ret->tipo = TP_Logico;
		}


		del(pilha);
		return ret;
	}

	/* Expressao simples
	 * Xs -> [-] T {( + | - | ‘or’) T}
	 */
	struct Fator *expressaoS(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("expressaoS");

		int menos = 0;
		int not = 0;
		NOVO_FATOR(ret);
		NOVO_FATOR(filho);

		if (regLex.token == Menos) {
			menos = 1;
			lexan();
		} else if (regLex.token == Not) {
			not = 1;
			lexan();
		}

		filho = termo();

		/* codegen */
		atualizaPai(ret,filho);
		if (menos) {
			/* acao semantica */
			verificaTipo(filho->tipo, TP_Integer);
			fatorGeraMenos(ret,filho);
		} else if (not) {
			verificaTipo(toLogico(filho->tipo), TP_Logico);
			fatorGeraNot(ret,filho);
		}


		/* operacoes int x int -> int */
		if (regLex.token == Mais || regLex.token == Menos) {

			/* codegen */
			guardaOp(ret);

			lexan();

			NOVO_FATOR(filho2);
			filho2 = termo();

			/* acao semantica */
			verificaTipo(ret->tipo, TP_Integer);
			verificaTipo(filho2->tipo, TP_Integer);

			/* codegen */
			genOpTermos(ret,filho2);

			ret->tipo = TP_Integer;
		}
		
		/* operacoes logico x logico -> logico */
		else if (regLex.token == Or) {

			/* codegen */
			guardaOp(ret);
			lexan();

			NOVO_FATOR(filho2);
			filho2 = termo();
			
			/* acao semantica */
			verificaTipo(toLogico(ret->tipo), TP_Logico);
			verificaTipo(toLogico(filho2->tipo), TP_Logico);

			/* codegen */
			genOpTermos(ret,filho2);

			ret->tipo = TP_Logico;
		}

		del(pilha);
		return ret;
	}

	void acaoFilhoTermo2(struct Fator *atual, Tipo gerado)
	{
			/* codegen */
			guardaOp(atual);

			lexan();

			NOVO_FATOR(filho2);
			filho2 = fator();

			/* acao semantica */
			verificaTipo(atual->tipo, gerado);
			verificaTipo(filho2->tipo,  gerado);

			/* codegen */
			genOpTermos(atual,filho2);
	
	}

	/* Termo
	 * T  -> F {( * | / | % | ‘and’ ) F}
	 */
	struct Fator *termo(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("termo");

		NOVO_FATOR(atual);
		NOVO_FATOR(filho);

		filho = fator();

		/* codegen */
		atualizaPai(atual,filho);

		/* operacoes int x int -> int */
		if (regLex.token == Vezes ||
				 regLex.token == Barra || regLex.token == Porcento )
		{
			acaoFilhoTermo2(atual, TP_Integer);
		}
		/* operacoes logico x logico -> logico */
		else if (regLex.token == And) {
			acaoFilhoTermo2(atual, TP_Logico);
		}

		del(pilha);
		return atual;
	}

	/* Fator
	 * F  -> ‘(‘ X ‘)’ | literal | id ['[' X ']']
	 */
	struct Fator *fator(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("fator");

		NOVO_FATOR(ret);

		int array = 0;
		char *lexId;

		if (regLex.token == A_Parenteses) {

			lexan();
			NOVO_FATOR(expr);
			expr = expressao();

			/* codegen */
			fatorGeraExp(ret,expr);
			
			casaToken(F_Parenteses);

		/* fator -> literal */
		} else if (regLex.token == Literal) {

			lexAux = removeComentario(lexemaLido);
			ret->tipo = regLex.tipo;

			/* codegen */
			fatorGeraLiteral(ret,lexAux);
			
			lexan();

		/* fator -> id */
		} else if (regLex.token == Identificador) {
			lexId = regLex.lexema;

			/* acao semantica */
			verificaDeclaracao(lexId);

			lexan();

			NOVO_FATOR(aux);
			/* lendo array: id[expressao()] */
			if (regLex.token == A_Colchete) {
				lexan();

				aux = expressao();

				/* acao semantica */
				verificaTipo(aux->tipo, TP_Integer);

				/* marca como array */
				atrPos(1); array = 1;

				casaToken(F_Colchete);
			}

			/* codegen */
			if (array)
				fatorGeraArray(ret,aux,lexId);
			else
				fatorGeraId(ret,lexId);

			ret->tipo = pesquisarRegistro(lexId)->simbolo.tipo;
		}

		del(pilha);
		return ret;
	}

	
	/* lista de expressoes */
	void expressao2(int ln)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("expressao2");
		NOVO_FATOR(expr);
	
		expr = expressao();
		genSaida(expr, 0);

		if (regLex.token == Virgula) {
			lexan();
			expressao2(0);
		}

		if (ln)
			proxLinha();

		del(pilha);
	}
#endif
