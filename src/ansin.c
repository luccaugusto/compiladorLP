/* **************************************************************************
 * Compilador desenvolvido para a disciplina de compiladores 2020-2 PUC minas
 * Este programa segue o estilo de código definido em: 
 * https://suckless.org/coding_style/
 *
 * Alunos: Laura Nunes - 587484
 * 		   Lucca Augusto - 587488
 * 		   Richard Mariano - 598894
 *
 * Arquivo ansin:
 * Implementação do analisador sintático. Quase toda função representa um
 * simbolo da gramatica, com algumas funções auxiliares. Durante a análise
 * sintática, ações semânticas e de geração de código são executadas.
 * (ver semac.c e codegen.c)
 * Portanto é a partir deste arquivo que a lógica principal do programa
 * acontece.
 *
 * *************************************************************************/

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

	/* HEADERS */
	#include "ansin.h"
	#include "semac.c"
	
	/* VARIAVEIS GLOBAIS */
	char* lexAux;
	
	/* atribui posicao de acesso ao vetor no registro lexico */
	void 
	atr_pos(int pos)
	{
		reg_lex.pos = pos;
	}
	
	/* atribui tipo à constante na tabela de simbolos */
	void
	atr_tipo()
	{
		reg_lex.endereco->simbolo.tipo = reg_lex.tipo;
	}
	
	/* confere se o último token lido é esperado
	 * Caso não seja o token esperado, aborta a
	 * execução chamando erro_sintatico()
	 *
	 * essa função tem seu uso limitado pois
	 * alguns estados aceitam mais de um Token
	 * como por exemplo: Var char|integer.
	 * Nesses casos casa_token nao pode ser
	 * utilizada.
	 * Quando só existe uma opção de Token
	 * casa_token DEVE ser utilizada.
	 */
	int
	casa_token(Tokens esperado)
	{
		int retorno = 1;
	
		if (esperado != reg_lex.token){
			if (lex) erro_sintatico(ER_SIN);
			else erro_sintatico(ER_SIN_EOF);
		}
	
		lexan();
		return retorno;
	}
	
	/* Trata um Erro Sintático
	 * e aborta o programa
	 */
	void
	erro_sintatico(int tipo)
	{
	
		erro = tipo;
		switch (tipo)
		{
			case ER_SIN:
				erro_msg = "token nao esperado";
				break;
			case ER_SIN_EOF:
				erro_msg = "fim de arquivo nao esperado";
				break;
			case ER_SIN_NDEC:
				erro_msg = "identificador nao declarado";
				break;
			case ER_SIN_JADEC:
				erro_msg = "identificador ja declarado";
				break;
			case ER_SIN_TAMVET:
				erro_msg = "tamanho do vetor excede o maximo permitido";
				break;
			case ER_SIN_C_INC:
				erro_msg = "classe de identificador incompativel";
				break;
			case ER_SIN_T_INC :
				erro_msg = "tipos incompativeis";
				break;
		}
	
		/* Aborta a compilação */
		abortar();
	}
	
	/* Consime o primeiro token e chama 
	 * o simbolo inicial
	 */
	void
	iniciar_ansin(void)
	{
		/* consome o primeiro token */
		lexan();
	
		/* codegen: inicia bloco de declaracoes */
		init_declaracao();
	
		/* inicia pelo primeiro simbolo da gramatica */
		declaracao();
		bloco_comandos();
		fim_de_arquivo();
	}
	
	/* Declaracao de variáveis ou constantes 
	 * Var variavel();
	 * Const constante();
	 */
	void
	declaracao(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("declaracao");

		do {
			/* var ou const */
			if (reg_lex.token == Var) {

				lido=0;
				lexan();
				variavel();

			} else if (reg_lex.token == Const) {

				lido=0;
				lexan();
				constante();

			} 

		} while (reg_lex.token == Var || reg_lex.token == Const);

		/* else */
		/* existem casos especificos onde o
		 * token do bloco de comandos ja foi lido
		 * e portanto nao precisa ser lido aqui,
		 * conferir lista_ids para ver a lista desses
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
		fim_dec_init_com();

		del(pilha);
	}
	
	
	/* Bloco de comandos
	 * For, If, ID= , ;, readln();, write();, writeln();
	 */
	void
	bloco_comandos()
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("bloco_comandos");

		/* controla se o lexema lido inicia comando */
		int ehComando = 1;
	
		do {

			switch(reg_lex.token)
			{
				case Identificador:
					/* acao semantica */
					verifica_declaracao(reg_lex.lexema);
					verifica_const(reg_lex.lexema);

					estado_sin = N_ACEITACAO_SIN;
					lexan();
					atribuicao();
					estado_sin = ACEITACAO_SIN;
					break;

				case For:
					estado_sin = N_ACEITACAO_SIN;
					lexan();
					repeticao();
					estado_sin = ACEITACAO_SIN;
					break;

				case If:
					estado_sin = N_ACEITACAO_SIN;
					lexan();
					teste();
					estado_sin = ACEITACAO_SIN;
					break;

				case PtVirgula:
					estado_sin = N_ACEITACAO_SIN;
					nulo();
					estado_sin = ACEITACAO_SIN;
					break;

				case Readln:
					estado_sin = N_ACEITACAO_SIN;
					lexan();
					leitura();
					estado_sin = ACEITACAO_SIN;
					break;

				case Write:
					estado_sin = N_ACEITACAO_SIN;
					lexan();
					escrita(0);
					estado_sin = ACEITACAO_SIN;
					break;

				case Writeln:
					estado_sin = N_ACEITACAO_SIN;
					lexan();
					escrita(1);
					estado_sin = ACEITACAO_SIN;
					break;

				default:
					ehComando = 0;
			}

		} while (ehComando);

		del(pilha);

	}
	
	/* EOF */
	void
	fim_de_arquivo(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("fim_de_arquivo");
	
	
		/* se lex nao for 0 ainda n leu o EOF */
		/* leu fim de arquivo mas nao em estado de aceitacao */
		if (lex)
			erro_sintatico(ER_SIN);
	
		if (estado_sin != ACEITACAO_SIN)
			erro_sintatico(ER_SIN_EOF);
	
		/* codegen: finaliza o programa */
		fim_comandos();
	
		sucesso();
		del(pilha);
	}
	
	/***********************************************
	 *
	 *  Procedimentos de Declaração
	 *
	 ***********************************************/
	
	/* Const id = literal; */
	void
	constante(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("constante");
	
		/* suporte acao semantica */
		/* salva o lexema atual e o tipo para verificacao da classe */
		lexAux = reg_lex.lexema;
		Tipo t = reg_lex.tipo;
	
		int negativo = 0;
	
		/* suporte acao semantica */
		defClasse(CL_Const);
	
		estado_sin = N_ACEITACAO_SIN;
		casa_token(Identificador);
	
		/* acao semantica */
		verifica_classe(lexAux, t);
	
		casa_token(Igual);
	
		/* literal negativo */
		if (reg_lex.token == Menos) {
			negativo = 1;
			lexan(); 
		}
	
		/* suporte acao semantica */
		/* atribui tipo a constante */
		atr_tipo();
	
		lexAux = removeComentario(lexema_lido);
		casa_token(Literal);
	
		/* codegen */
		gen_declaracao(reg_lex.tipo,
				reg_lex.classe,
				reg_lex.tamanho,
				lexAux,
				negativo
				);
	
		casa_token(PtVirgula); lido = 1;
		del(pilha);
	}
	
	/* var char|integer lista_ids(); */
	void
	variavel(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("variavel");
		int ehVariavel = 1;
	
		/* suporte acao semantica */
		defClasse(CL_Var);

		do {
			estado_sin = N_ACEITACAO_SIN;
			if (reg_lex.token == Char || reg_lex.token == Integer) {

				/* suporte acao semantica */
				if (reg_lex.token == Char) reg_lex.tipo = TP_Char;
				else reg_lex.tipo = TP_Integer;

				lexan();
				ehVariavel = lista_ids(reg_lex.tipo);
			} else {
				erro_sintatico(ER_SIN);
			}

		} while (ehVariavel);

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
	 * em casos id = literal, nesse caso reg_lex.tipo
	 * vai ser o tipo do literal
	 *
	 * retorna 1 caso outra declaracao de tipos
	 * foi encontrada, 0 caso contrario
	 */
	int
	lista_ids(Tipo ultimoTipo)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("lista_ids");
	
		Tipo t;
		int negativo = 0;
		int id = 1; /* controle de lexema indica lista de ids */
		int ret = 1;
	
		do {
			/* acao semantica */
			verifica_classe(reg_lex.lexema, ultimoTipo);

			casa_token(Identificador);
			if (reg_lex.token == Virgula){
				/* Lendo id,id */

				/* codegen */
				gen_declaracao(
						reg_lex.endereco->simbolo.tipo,
						reg_lex.classe,
						reg_lex.endereco->simbolo.tamanho,
						NULL,
						negativo
						);

				lexan();

			} else if (reg_lex.token == PtVirgula) {
				/* lendo fim de um comando */

				/* codegen */
				gen_declaracao(
						reg_lex.endereco->simbolo.tipo,
						reg_lex.classe,
						reg_lex.endereco->simbolo.tamanho,
						NULL,
						negativo
						);

				lexan();

				/* Lista de declaracoes tipo Var integer c; char d; 
				 * ou fim do comando
				 */
				id = 0;
				if (reg_lex.token == Integer || reg_lex.token == Char) {
					ret = 1;
				} else {
					/* fim do comando e marca lido como 1
					 * pois leu um lexema que nao
					 * foi utilizado aqui, portanto
					 * o proximo metodo nao precisa ler
					 * este lexema
					 * */
					ret = 0;
					lido = 1;
				}

			} else if (reg_lex.token == Igual) {
				/* lendo id=literal */
				lexan();

				/* literal negativo */
				if (reg_lex.token == Menos) {
					negativo = 1;
					lexan();
				}

				t = reg_lex.tipo;
				lexAux = reg_lex.lexema;

				casa_token(Literal);

				/* acao semantica */
				verifica_tipo(reg_lex.endereco->simbolo.tipo, t);

				/* codegen */
				gen_declaracao(
						t,
						reg_lex.classe,
						reg_lex.endereco->simbolo.tamanho,
						lexAux,
						negativo
						);

				if (reg_lex.token == Virgula) {
					/* outro id */
					lexan();

				} else {
					/* terminou de ler o comando */
					casa_token(PtVirgula);

					id = 0;
					/* Lista de declaracoes tipo Var integer c; char d; */
					if (reg_lex.token == Integer || reg_lex.token == Char) {
						ret = 1;
					} else {
						/* fim do comando e marca lido como 1
						 * pois leu um lexema que nao
						 * foi utilizado aqui, portanto
						 * o proximo metodo nao precisa ler
						 * este lexema
						 * */
						lido = 1;
						ret = 0;
					}
				}
			} else {
				/* lendo id[literal] */
				casa_token(A_Colchete);

				lexAux = reg_lex.lexema;
				casa_token(Literal);

				/* acao semantica */
				verifica_tam(str2int(lexAux));

				casa_token(F_Colchete);

				/* codegen */
				gen_declaracao(
						reg_lex.endereco->simbolo.tipo,
						reg_lex.classe,
						reg_lex.endereco->simbolo.tamanho,
						NULL,
						negativo
						);


				if (reg_lex.token == Virgula) {
					/* outro id */
					lexan();

				} else {
					/* terminou de ler o comando */
					casa_token(PtVirgula);

					id = 0;
					/* Lista de declaracoes tipo Var integer c; char d; */
					if (reg_lex.token == Integer || reg_lex.token == Char) {
						ret = 1;
					} else {
						/* fim do comando e marca lido como 1
						 * pois leu um lexema que nao
						 * foi utilizado aqui, portanto
						 * o proximo metodo nao precisa ler
						 * este lexema
						 * */
						ret = 0;
						lido = 1;
					}
				}
			}
		} while (id);

		del(pilha);
		return ret;
	}
	
	/***********************************************
	 *
	 *  Procedimentos de Bloco de comandos
	 *
	 ***********************************************/
	
	
	/* Atribuicao
	 * ID=expressao();
	 */
	void
	atribuicao(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("atribuicao");
	
		NOVO_FATOR(pai);
		NOVO_FATOR(expr);  /* fator da expressao do lado direito */
		int tipoId = reg_lex.endereco->simbolo.tipo;
		pai->tipo = tipoId;

		/* codegen
		 * salva no pai o endereco de id
		 * para caso leia outro id na expressao de atribuicao
		 *
		 * zera os temporarios
		 */
		pai->endereco = reg_lex.endereco->simbolo.memoria;
		zera_temp();
	
		/* lendo array: id[expressao()] */
		if (reg_lex.token == A_Colchete) {
			lexan();
	
			NOVO_FATOR(aux);

			aux = expressao();
			/* acao semantica */
			verifica_tipo(aux->tipo, TP_Integer);
	
			/* marca como array */
			atr_pos(1);
			casa_token(F_Colchete);
			fator_gera_array(pai, aux, reg_lex.endereco->simbolo.lexema);
		}

		casa_token(Igual);
	
	
		expr = expressao();

		/* acao semantica */
		verifica_tipo(expr->tipo, tipoId);
		verifica_atr_vetor();

		/* codegen */
		gen_atribuicao(pai,expr);
	
		del(pilha);
		casa_token(PtVirgula);
	} 
	
	/* Repeticao
	 * ID = exp to exp [step exp] do comandos2() 
	 */
	void
	repeticao(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("repeticao");
	
		Tipo t = reg_lex.endereco->simbolo.tipo;
		Tipo t2;
		NOVO_FATOR(pai);
		NOVO_FATOR(filho);
		NOVO_FATOR(filho2);
		NOVO_FATOR(f_aux);
	
		lexAux = reg_lex.lexema;

		/* codegen
		 * salva o endereco do id no pai
		 * para caso leia outro id na expressao de atribuicao
		 * e zera temporarios
		 */
		pai->endereco = reg_lex.endereco->simbolo.memoria;
		pai->tipo = t;
		zera_temp();

		/* id = exp */
		casa_token(Identificador);

		/* acao semantica */
		verifica_declaracao(lexAux);
		verifica_const(lexAux);
		verifica_tipo(t,TP_Integer);
	
		/* lendo array: id[i] */
		if (reg_lex.token == A_Colchete) {
			lexan();

			f_aux = expressao();
	
			/* acao semantica */
			verifica_tipo(f_aux->tipo,TP_Integer);

			/* codegen */
			acesso_array(pai, f_aux);
	
			casa_token(F_Colchete);
			free(f_aux);
		}
	
		/* ja leu ( id|id[i] ) e pode fechar o comando */
		casa_token(Igual);
	
		filho = expressao();

		/* codegen */
		gen_atribuicao(pai, filho);
		/* acao semantica */
		verifica_tipo(filho->tipo,TP_Integer);

		/* id = exp */
		/* to exp */
		casa_token(To);

		filho2 = expressao();

		/* codegen */
		rot inicio = novo_rot();
		rot fim = novo_rot();
		gen_repeticao(pai,filho2,inicio,fim);
		/* to exp */

		/* [step exp] */
		if (reg_lex.token == Step) {
			lexan();

			/* guarda o step */
			f_aux = expressao();

			/* acao semantica */
			verifica_tipo(f_aux->tipo, TP_Integer);
		}
		/* [step exp] */
	
		casa_token(Do);
		comandos2();

		/* codegen */
		gen_fim_repeticao(pai, inicio, fim, f_aux);
	
		del(pilha);
	}
	
	
	/* R1 na gramatica
	 * { bloco_comandos() }
	 * ou
	 * comando unico
	 */
	void
	comandos2(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("comandos2");
	
		switch(reg_lex.token)
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
				bloco_comandos();
				casa_token(F_Chaves);
				break;

			default:
				if (lex) erro_sintatico(ER_SIN);
				else erro_sintatico(ER_SIN_EOF);
		}

		del(pilha);
	}
	
	/* Teste
	 * expressao() then comandos2() teste1()
	 */
	void
	teste(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("teste");

		NOVO_FATOR(expr);
		rot falso = novo_rot();
		rot fim = novo_rot();

		/* codegen */
		zera_temp();

		expr = expressao();
	
		/* acao semantica */
		verifica_tipo(expr->tipo, TP_Logico);

		/* codegen */
		gen_teste(expr, falso, fim);
	
		/* then foi lido antes de retornar de expressao() */
		casa_token(Then);
	
		comandos2();
	
		teste1(falso, fim);
		del(pilha);
	}
	
	/* else comandos2()
	 * ou fim do if
	 * bloco_comandos()
	 */
	void
	teste1(rot falso, rot fim)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("teste1");

		gen_else_teste(falso, fim);
	
		if (reg_lex.token == Else) {
			lexan();
			comandos2();
		}

		gen_fim_teste(fim);
		del(pilha);
	}
	
	/* Comando de leitura
	 * readln(id)
	 */
	void
	leitura(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("leitura");
		NOVO_FATOR(pai);
		NOVO_FATOR(expr);
		char *lexId;
		struct Celula *registro;
	
		casa_token(A_Parenteses);

		lexId = reg_lex.lexema;
		registro = pesquisar_registro(lexId);

		/* acao semantica */
		verifica_const(lexId);
		verifica_declaracao(lexId);

		casa_token(Identificador);

		/* codegen */
		pai->endereco = registro->simbolo.memoria;
		pai->tipo = registro->simbolo.tipo;
		pai->tamanho = registro->simbolo.tamanho;
		zera_temp();
	
		if (reg_lex.token == A_Colchete) {
			/* lendo array: id[i] */
			lexan();
			expr = expressao();
			casa_token(F_Colchete);

			/* acao semantica */
			verifica_tipo(expr->tipo, TP_Integer);
			fator_gera_array(pai, expr, lexId);
		}
	
		/* ja leu ( id|id[i] ) e pode fechar o comando */
		casa_token(F_Parenteses);
		casa_token(PtVirgula);

		/* codegen */
		gen_entrada(pai);
	
		del(pilha);
	}
	
	/* Comando nulo
	 * ;
	 */
	void
	nulo(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("nulo");
	
		casa_token(PtVirgula);
		del(pilha);
	}
	
	/* Comando de escrita
	 * write(id|const)
	 */
	void
	escrita(int ln)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("escrita");
	
		zera_temp();
		casa_token(A_Parenteses);
		expressao2(ln);
		casa_token(F_Parenteses);
		casa_token(PtVirgula);
		del(pilha);
	}
	
	/* le uma expressao e retorna o tipo final
	 * Expressão
	 * X -> Xs [ O Xs ]
	 * O  -> = | <> | < | > | >= | <=
	 */
	struct Fator *
	expressao(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("expressao");

		NOVO_FATOR(ret);
		NOVO_FATOR(filho);

		filho = expressao_s();
		
		/* codegen */
		atualiza_pai(ret,filho);

		/* operacoes tipo x tipo -> logico */
		if (reg_lex.token == Igual || reg_lex.token == Diferente) {

			/* codegen */
			guarda_op(ret);

			lexan();

			NOVO_FATOR(filho2);
			filho2 = expressao_s();

			/* acao semantica */
			verifica_tipo(ret->tipo, filho2->tipo);

			/* codegen */
			gen_op_termos(ret,filho2);

			ret->tipo = TP_Logico;
		}

		/* operacoes tipo int x int -> logico */
		else if (reg_lex.token == Menor || reg_lex.token == Maior ||
				reg_lex.token == MaiorIgual || reg_lex.token == MenorIgual) 
		{

			/* codegen */
			guarda_op(ret);

			lexan();

			NOVO_FATOR(filho2);
			filho2 = expressao_s();

			/* acao semantica */
			verifica_tipo(ret->tipo,TP_Integer);
			verifica_tipo(filho2->tipo,TP_Integer);

			/* codegen */
			gen_op_termos(ret,filho2);

			ret->tipo = TP_Logico;
		}


		del(pilha);
		return ret;
	}

	/* Expressao simples
	 * Xs -> [-] T {( + | - | ‘or’) T}
	 */
	struct Fator *
	expressao_s(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("expressao_s");

		int menos = 0;
		int not = 0;
		NOVO_FATOR(ret);
		NOVO_FATOR(filho);

		if (reg_lex.token == Menos) {
			menos = 1;
			lexan();
		} else if (reg_lex.token == Not) {
			not = 1;
			lexan();
		}

		filho = termo();

		/* codegen */
		atualiza_pai(ret,filho);
		if (menos) {
			/* acao semantica */
			verifica_tipo(filho->tipo, TP_Integer);
			fator_gera_menos(ret,filho);
		} else if (not) {
			verifica_tipo(to_logico(filho->tipo), TP_Logico);
			fator_gera_not(ret,filho);
		}


		/* operacoes int x int -> int */
		if (reg_lex.token == Mais || reg_lex.token == Menos) {

			/* codegen */
			guarda_op(ret);

			lexan();

			NOVO_FATOR(filho2);
			filho2 = termo();

			/* acao semantica */
			verifica_tipo(ret->tipo, TP_Integer);
			verifica_tipo(filho2->tipo, TP_Integer);

			/* codegen */
			gen_op_termos(ret,filho2);

			ret->tipo = TP_Integer;
		}
		
		/* operacoes logico x logico -> logico */
		else if (reg_lex.token == Or) {

			/* codegen */
			guarda_op(ret);
			lexan();

			NOVO_FATOR(filho2);
			filho2 = termo();
			
			/* acao semantica */
			verifica_tipo(to_logico(ret->tipo), TP_Logico);
			verifica_tipo(to_logico(filho2->tipo), TP_Logico);

			/* codegen */
			gen_op_termos(ret,filho2);

			ret->tipo = TP_Logico;
		}

		del(pilha);
		return ret;
	}

	void
	acaoFilhoTermo2(struct Fator *atual, Tipo gerado)
	{
			/* codegen */
			guarda_op(atual);

			lexan();

			NOVO_FATOR(filho2);
			filho2 = fator();

			/* acao semantica */
			verifica_tipo(atual->tipo, gerado);
			verifica_tipo(filho2->tipo,  gerado);

			/* codegen */
			gen_op_termos(atual,filho2);
	
	}

	/* Termo
	 * T  -> F {( * | / | % | ‘and’ ) F}
	 */
	struct Fator *
	termo(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("termo");

		NOVO_FATOR(atual);
		NOVO_FATOR(filho);

		filho = fator();

		/* codegen */
		atualiza_pai(atual,filho);

		/* operacoes int x int -> int */
		if (reg_lex.token == Vezes ||
				 reg_lex.token == Barra || reg_lex.token == Porcento )
		{
			acaoFilhoTermo2(atual, TP_Integer);
		}
		/* operacoes logico x logico -> logico */
		else if (reg_lex.token == And) {
			acaoFilhoTermo2(atual, TP_Logico);
		}

		del(pilha);
		return atual;
	}

	/* Fator
	 * F  -> ‘(‘ X ‘)’ | literal | id ['[' X ']']
	 */
	struct Fator *
	fator(void)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("fator");

		NOVO_FATOR(ret);

		int array = 0;
		char *lexId;

		if (reg_lex.token == A_Parenteses) {

			lexan();
			NOVO_FATOR(expr);
			expr = expressao();

			/* codegen */
			fator_gera_exp(ret,expr);
			
			casa_token(F_Parenteses);

		/* fator -> literal */
		} else if (reg_lex.token == Literal) {

			lexAux = removeComentario(lexema_lido);
			ret->tipo = reg_lex.tipo;

			/* codegen */
			fator_gera_literal(ret,lexAux);
			
			lexan();

		/* fator -> id */
		} else if (reg_lex.token == Identificador) {
			lexId = reg_lex.lexema;

			/* acao semantica */
			verifica_declaracao(lexId);

			lexan();

			NOVO_FATOR(aux);
			/* lendo array: id[expressao()] */
			if (reg_lex.token == A_Colchete) {
				lexan();

				aux = expressao();

				/* acao semantica */
				verifica_tipo(aux->tipo, TP_Integer);

				/* marca como array */
				atr_pos(1); array = 1;

				casa_token(F_Colchete);
			}

			/* codegen */
			if (array)
				fator_gera_array(ret,aux,lexId);
			else
				fator_gera_id(ret,lexId);

			ret->tipo = pesquisar_registro(lexId)->simbolo.tipo;
		}

		del(pilha);
		return ret;
	}

	
	/* lista de expressoes */
	void
	expressao2(int ln)
	{
		/* DEBUGGER E PILHA */
		DEBUGSIN("expressao2");
		NOVO_FATOR(expr);
	
		expr = expressao();
		gen_saida(expr, 0);

		while (reg_lex.token == Virgula) {

			lexan();

			expr = expressao();
			gen_saida(expr, 0);

		}

		/* codegen */
		if (ln) prox_linha();

		del(pilha);
	}
#endif
