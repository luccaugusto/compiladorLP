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
		if (DEBUG_SIN) printf("SIN: Declaracao\n");
		push("Declaracao",pilha);
	
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
		if (DEBUG_SIN) printf("SIN: blocoComandos\n");
		push("blocoComandos",pilha);
	
		switch(regLex.token)
		{
			case Identificador:
				/* acao semantica */
				verificaDeclaracao(regLex.lexema);
	
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
		if (DEBUG_SIN) printf("SIN: constante\n");
		push("constante",pilha);
	
		/* salva o lexema atual para verificacao da classe */
		lexAux = regLex.lexema;
	
		int negativo = 0;
	
		defClasse(CL_Const);
	
		estado_sin = N_ACEITACAO_SIN;
		casaToken(Identificador);
	
		/* Ação semantica */
		verificaClasse(lexAux);
	
		casaToken(Igual);
	
		/* literal negativo */
		if (regLex.token == Menos) {
			negativo = 1;
			lexan(); 
		}
	
		/* atribui tipo a constante */
		atrTipo();
	
		lexAux = regLex.lexema;
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
		if (DEBUG_SIN) printf("SIN: variavel\n");
		push("variavel",pilha);
	
		/* Ação semantica */
		defClasse(CL_Var);
	
		estado_sin = N_ACEITACAO_SIN;
		if (regLex.token == Char || regLex.token == Integer) {
			if (regLex.token == Char) regLex.tipo = TP_Char;
			else regLex.tipo = TP_Integer;
	
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
	
		Tipo t;
		int negativo = 0;
	
		/* acao semantica */
		verificaClasse(regLex.lexema);
	
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
			listaIds();
	
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
				listaIds();
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
				listaIds();
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
		if (DEBUG_SIN) printf("SIN: atribuicao\n");
		push("atribuicao",pilha);
	
		/* fator da expressao do lado direito */
		NOVO_FATOR(expr);
	
		/* termo da atribuicao */
		NOVO_TERMO(atual);
		atual->fator = expr;
		expr->termo = atual;
	
		atual->n_termos = 0;
	
		/* lendo array: id[expressao()] */
		if (regLex.token == A_Colchete) {
			lexan();
	
			NOVO_FATOR(aux);
			aux->termo = atual;
			aux = expressao(expr);
			/* acao semantica */
			verificaTipo(aux->tipo, TP_Integer);
	
			atrPos(1);
			casaToken(F_Colchete);
		}
		casaToken(Igual);
	
		lexAux = regLex.lexema;
	
		/* codegen */
		zeraTemp();
	
		/* acao semantica */
		expr = expressao(atual->fator);
		verificaTipo(expr->tipo, regLex.endereco->simbolo.tipo);
		verificaAtrVetor();
	
		del(pilha);
		casaToken(PtVirgula);
	} 
	
	/* Repeticao
	 * ID = literal to literal repeticao1();
	 */
	void repeticao(void)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: repeticao\n");
		push("repeticao",pilha);
	
		Tipo t = buscaTipo(regLex.lexema);
	
		casaToken(Identificador);
	
		/* acao semantica */
		verificaTipo(t,TP_Integer);
	
	
		/* lendo array: id[i] */
		if (regLex.token == A_Colchete) {
			lexan();
	
			/* acao semantica */
			verificaTipo(expressao(NULL)->tipo,TP_Integer);
	
			casaToken(F_Colchete);
		}
	
		/* ja leu ( id|id[i] ) e pode fechar o comando */
		casaToken(Igual);
	
		t = regLex.tipo;
		casaToken(Literal);
	
		/* acao semantica */
		verificaTipo(t,TP_Integer);
	
		casaToken(To);
	
		if (regLex.token == Literal) {
			/* acao semantica */
			verificaTipo(regLex.tipo, TP_Integer);
	
			lexan();
	
		} else if (regLex.token == Identificador) {
			/* acao semantica */
			verificaTipo(buscaTipo(regLex.lexema), TP_Integer);
	
			lexan();
	
			if (regLex.token == A_Colchete) {
				/* lendo array: id[i] */
				lexan();
	
				/* acao semantica */
				verificaTipo(expressao(NULL)->tipo,TP_Integer);
	
				casaToken(F_Colchete);
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
	
		Tipo t;
	
		if (regLex.token == Step) {
			lexan();
			t = regLex.tipo;
	
			casaToken(Literal);
	
			/* acao semantica */
			verificaTipo(t, TP_Integer);
	
	
		}
	
		casaToken(Do);
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
		if (DEBUG_SIN) printf("SIN: teste\n");
		push("teste",pilha);
	
		/* acao semantica */
		verificaTipo(expressao(NULL)->tipo,TP_Logico);
	
		/* then foi lido antes de retornar de expressao() */
		casaToken(Then);
	
		comandos2();
	
		if (regLex.token == F_Chaves)
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
	
		if (regLex.token == Else) {
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
	
		casaToken(A_Parenteses);
		casaToken(Identificador);
	
		if (regLex.token == A_Colchete) {
			/* lendo array: id[i] */
			lexan();
			expressao(NULL);
			casaToken(F_Colchete);
		}
	
		/* ja leu ( id|id[i] ) e pode fechar o comando */
		casaToken(F_Parenteses);
		casaToken(PtVirgula);
	
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
	
		casaToken(PtVirgula);
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
	
		/* fator da expressao do lado direito */
		NOVO_FATOR(expr);
	
		/* termo da atribuicao */
		NOVO_TERMO(atual);
		atual->fator = expr;
		expr->termo = atual;
	
		casaToken(A_Parenteses);
		expressao2(expr);
		casaToken(F_Parenteses);
		casaToken(PtVirgula);
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
	struct Fator *expressao(struct Fator *pai)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: expressao\n");
		push("expressao",pilha);
	
		/* verifica o tipo do valor atual com o tipo da expressao a direita */
		Tipo dir;
	
		/* fator da expressao atual  */
		NOVO_FATOR(atual);
		atual->termo = pai->termo;
	
		if (regLex.token == A_Parenteses) {
			/* (expressao()) */
	
			NOVO_FATOR(aux);
			aux->termo = pai->termo;
	
			lexan();
			aux = expressao(atual);
			casaToken(F_Parenteses);
	
			/* codegen */
			fatorGeraExp(atual,aux);
			acaoTermoFator1(pai);
	
		} else if (regLex.token == Identificador) {
			/* id */
			/* pega o tipo do id em questao */
			dir = buscaTipo(regLex.lexema);
			pai->termo->n_termos++;
	
			NOVO_FATOR(expr);
	
			lexan();
			/* lendo array: id[i] */
			if (regLex.token == A_Colchete) {
				lexan();
				expr = expressao(atual);
				casaToken(F_Colchete);
				/* codegen */
				fatorGeraArray(atual, expr);
			} else { 
				/* codegen */
				fatorGeraId(atual);
			}
	
			acaoTermoFator1(pai);
	
			atual->tipo = expressao1(dir,atual);
			pai->tipo = atual->tipo;
	
		} else if (regLex.token == Literal) {
			/* literal */
	
			pai->termo->n_termos++;
	
			dir = regLex.tipo;
			lexAux = regLex.lexema;
			lexan();
	
			atual->tipo = expressao1(dir,atual);

			pai->tipo = atual->tipo;
	
			/* codegen */
			fatorGeraConst(atual,lexAux);
			acaoTermoFator1(pai);
	
		} else if (regLex.token == Menos) {
			lexan();
			NOVO_FATOR(filho);

			filho = expressao(atual);
			/*acao semantica */
			verificaTipo(filho->tipo, TP_Integer);
	
			/* codegen */
			fatorGeraMenos(atual,filho);
			acaoTermoFator1(pai);
	
		} else if (regLex.token == Not) {
			lexan();
			NOVO_FATOR(filho);

			filho = expressao(atual);
			/*acao semantica */
			verificaTipo(toLogico(filho->tipo), TP_Logico);
	
			/* codegen */
			fatorGeraNot(atual,filho);
			acaoTermoFator1(pai);
		}
		/* else lambda */
	
		del(pilha);
		return atual;
	}
	
	/* operadores expressao();
	 * parametro esq: tipo da espressao do lado esquerdo
	 * retorna o tipo da expressao do lado direito do operador
	 */
	Tipo expressao1(Tipo esq, struct Fator *pai)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: expressao1\n");
		push("expressao1",pilha);

		NOVO_FATOR(expr);
	
		Tipo ret = esq;
		int lambda = 1;
	
		/* op id|literal */
		/* operadores exclusivos de inteiros que avaliam para logicos */
		if (regLex.token == MaiorIgual ||
				regLex.token == MenorIgual ||
				regLex.token == Maior      ||
				regLex.token == Menor)
		{
			acaoTermoFator2(pai);
			lambda = 0;
			lexan();
	
			expr = expressao(pai);

			/* acao semantica */
			verificaTipo(esq,TP_Integer);
			verificaTipo(expr->tipo,TP_Integer);
	
			ret = TP_Logico;
	
			/* operadores exclusivos de inteiros que avaliam para inteiros */
		} else if (
				regLex.token == Mais       ||
				regLex.token == Menos      ||
				regLex.token == Porcento   ||
				regLex.token == Barra      ||
				regLex.token == Vezes)
		{
			acaoTermoFator2(pai);
			lambda = 0;
			lexan();
	
			expr = expressao(pai);
	
			/* acao semantica */
			verificaTipo(esq,TP_Integer);
			verificaTipo(expr->tipo,TP_Integer);
	
			/* operadores exclusivos de tipo logico que avaliam para logicos*/
		} else if (
				regLex.token == Or         ||
				regLex.token == And        ||
				regLex.token == Not) 
		{
			acaoTermoFator2(pai);
			lambda = 0;
			lexan();

			expr = expressao(pai);
	
			/* acao semantica */
			verificaTipo(esq,TP_Logico);
			verificaTipo(toLogico(expr->tipo),TP_Logico);
			ret = TP_Logico;
	
			/* Operadores sobre todos os tipos, avaliam para logicos */
		} else if (
				regLex.token == Diferente  ||
				regLex.token == Igual)
		{
			acaoTermoFator2(pai);
			lambda = 0;
	
			lexan();

			expr = expressao(pai);
	
			/* acao semantica */
			verificaTipo(expr->tipo,esq);
			ret = TP_Logico;
		} 
	
		if (!lambda)
			acaoTermoFator3(pai);
	
	
		del(pilha);
		return ret;
	}
	
	/* lista de expressoes */
	void expressao2(struct Fator *pai)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: expressao2\n");
		push("expressao2",pilha);
	
		expressao(pai);
		expressao3(pai);
		del(pilha);
	}
	
	/* mais uma expressao ou lambda */
	void expressao3(struct Fator *pai)
	{
		/* DEBUGGER E PILHA */
		if (DEBUG_SIN) printf("SIN: expressao3\n");
		push("expressao3",pilha);
	
		if (regLex.token == Virgula) {
			lexan();
			expressao2(pai);
		}
		/* else lambda */
		del(pilha);
	}
#endif
