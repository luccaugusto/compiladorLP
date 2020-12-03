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

/* HEADERS */
#include <stddef.h>
#include "ansin.h"
#include "types.h"
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
 * Quando só existe uma opção de Token
 * casa_token DEVE ser utilizada.
 */
int
casa_token(Tokens esperado)
{
	int retorno = 1;

	if (esperado != reg_lex.token){
		if (not_eof) erro_sintatico(ER_SIN);
		else erro_sintatico(ER_SIN_EOF);
	}

	lexan();
	return retorno;
}

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

	abortar();
}

void
iniciar_ansin(void)
{
	lexan();

	/* codegen: inicia bloco de declaracoes */
	init_declaracao();

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

			lexan();
			variavel();

		} else if (reg_lex.token == Const) {

			lexan();
			constante();

		} 

	} while (reg_lex.token == Var || reg_lex.token == Const);

	/* codegen: finaliza bloco de declaracoes e 
	 * inicializa bloco do programa */
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

	int eh_comando = 1;

	do {

		switch(reg_lex.token)
		{
			case Identificador:
				/* acao semantica */
				eh_id_ja_declarado(reg_lex.lexema);
				eh_const(reg_lex.lexema);

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
				eh_comando = 0;
		}

	} while (eh_comando);

	del(pilha);

}

/* EOF */
void
fim_de_arquivo(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN("fim_de_arquivo");


	/* se not_eof nao for 0 ainda nao leu o EOF */
	if (not_eof)
		erro_sintatico(ER_SIN);

	/* leu fim de arquivo mas nao em estado de aceitacao */
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
	def_classe(CL_Const);

	estado_sin = N_ACEITACAO_SIN;
	casa_token(Identificador);

	/* acao semantica */
	eh_id_nao_declarado(lexAux, t);

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

	casa_token(PtVirgula);
	del(pilha);
}

/* var char|integer lista_ids(); */
void
variavel(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN("variavel");
	int eh_variavel = 1;

	/* suporte acao semantica */
	def_classe(CL_Var);

	do {
		estado_sin = N_ACEITACAO_SIN;
		if (reg_lex.token == Char || reg_lex.token == Integer) {

			/* suporte acao semantica */
			if (reg_lex.token == Char) reg_lex.tipo = TP_Char;
			else reg_lex.tipo = TP_Integer;

			lexan();
			eh_variavel = lista_ids(reg_lex.tipo);
		} else {
			erro_sintatico(ER_SIN);
		}

	} while (eh_variavel);

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
	int eh_id = 1;
	int ret = 1;

	do {
		/* acao semantica */
		eh_id_nao_declarado(reg_lex.lexema, ultimoTipo);

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
			 * ou fim do comando */
			eh_id = 0;
			ret = (reg_lex.token == Integer ||
					reg_lex.token == Char) ?  1 : 0;

		} else if (reg_lex.token == Igual) {
			/* lendo id=literal */
			lexan();

			if (reg_lex.token == Menos) {
				negativo = 1;
				lexan();
			}

			t = reg_lex.tipo;
			lexAux = reg_lex.lexema;

			casa_token(Literal);

			/* acao semantica */
			tipos_compativeis(reg_lex.endereco->simbolo.tipo, t);

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
				casa_token(PtVirgula);

				eh_id = 0;
				/* Lista de declaracoes tipo Var integer c; char d; */
				ret = (reg_lex.token == Integer ||
						reg_lex.token == Char) ?  1 : 0;
			}
		} else {
			/* lendo id[literal] */
			casa_token(A_Colchete);

			lexAux = reg_lex.lexema;
			casa_token(Literal);

			/* acao semantica */
			tamanho_valido(str2int(lexAux));

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

				eh_id = 0;
				/* Lista de declaracoes tipo Var integer c; char d; */
				ret = (reg_lex.token == Integer ||
						reg_lex.token == Char) ?  1 : 0;
				
			}
		}
	} while (eh_id);

	del(pilha);
	return ret;
}


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
	int eh_array;

	/* codegen */
	pai->endereco = reg_lex.endereco->simbolo.memoria;
	zera_temp();

	/* lendo array: id[expressao()] */
	if (reg_lex.token == A_Colchete) {
		lexan();

		NOVO_FATOR(aux);

		aux = expressao();
		/* acao semantica */
		tipos_compativeis(aux->tipo, TP_Integer);

		atr_pos(1); eh_array = 1;
		casa_token(F_Colchete);
		fator_gera_array(pai, aux, reg_lex.endereco->simbolo.lexema);
	}

	casa_token(Igual);
	expr = expressao();

	/* acao semantica */
	tipos_compativeis(expr->tipo, tipoId);
	if (eh_array)
		atr_vetor_valida();

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
	NOVO_FATOR(pai);
	struct Expr *filho = NULL;
	struct Expr *filho2 = NULL;
	struct Expr *f_aux = NULL;

	lexAux = reg_lex.lexema;

	/* codegen */
	pai->endereco = reg_lex.endereco->simbolo.memoria;
	pai->tipo = t;
	zera_temp();

	casa_token(Identificador);

	/* acao semantica */
	eh_id_ja_declarado(lexAux);
	eh_const(lexAux);
	tipos_compativeis(t,TP_Integer);

	/* lendo array: id[i] */
	if (reg_lex.token == A_Colchete) {
		lexan();

		f_aux = expressao();

		/* acao semantica */
		tipos_compativeis(f_aux->tipo,TP_Integer);

		/* codegen */
		acesso_array(pai, f_aux);

		casa_token(F_Colchete);
		free(f_aux);
		f_aux = NULL;
	}

	casa_token(Igual);

	filho = expressao();

	/* codegen */
	gen_atribuicao(pai, filho);
	/* acao semantica */
	tipos_compativeis(filho->tipo,TP_Integer);

	casa_token(To);

	filho2 = expressao();

	/* codegen */
	rot inicio = novo_rot();
	rot fim = novo_rot();
	gen_repeticao(pai,filho2,inicio,fim);

	if (reg_lex.token == Step) {
		lexan();

		/* guarda o step */
		f_aux = expressao();

		/* acao semantica */
		tipos_compativeis(f_aux->tipo, TP_Integer);
	}

	casa_token(Do);
	comandos2();

	/* codegen */
	gen_fim_repeticao(pai, inicio, fim, f_aux);

	del(pilha);
}


/* 
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
			if (not_eof) erro_sintatico(ER_SIN);
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
	tipos_compativeis(expr->tipo, TP_Logico);

	/* codegen */
	gen_teste(expr, falso, fim);

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
	char *l_aux;
	struct Celula *registro;

	casa_token(A_Parenteses);

	lexId = reg_lex.lexema;
	l_aux = lexema_lido;
	registro = pesquisar_registro(lexId);

	casa_token(Identificador);

	/* salva lexema_lido para que em caso de erro a mensagem
	 * mostre o lexema do identificador, e nao o que vier 
	 * depois dele.
	 */
	lexema_lido = l_aux;

	/* acao semantica */
	eh_const(lexId);
	eh_id_ja_declarado(lexId);

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
		tipos_compativeis(expr->tipo, TP_Integer);
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
	lista_expressoes(ln);
	casa_token(F_Parenteses);
	casa_token(PtVirgula);
	del(pilha);
}

void
fator_op_fator(struct Expr *atual, Tipo esperado)
{
		/* codegen */
		guarda_op(atual);

		lexan();

		NOVO_FATOR(filho2);
		filho2 = fator();

		/* acao semantica */
		tipos_compativeis(atual->tipo, esperado);
		tipos_compativeis(filho2->tipo,  esperado);

		/* codegen */
		gen_op_termos(atual,filho2);

}

/* le uma expressao e retorna o tipo final
 * Expressão
 * X -> Xs [ O Xs ]
 * O  -> = | <> | < | > | >= | <=
 */
struct Expr *
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
		tipos_compativeis(ret->tipo, filho2->tipo);

		/* codegen */
		gen_op_termos(ret,filho2);

		ret->tipo = TP_Logico;

	/* operacoes tipo int x int -> logico */
	} else if (reg_lex.token == Menor || reg_lex.token == Maior ||
			reg_lex.token == MaiorIgual || reg_lex.token == MenorIgual) 
	{

		/* codegen */
		guarda_op(ret);

		lexan();

		NOVO_FATOR(filho2);
		filho2 = expressao_s();

		/* acao semantica */
		tipos_compativeis(ret->tipo,TP_Integer);
		tipos_compativeis(filho2->tipo,TP_Integer);

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
struct Expr *
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
		tipos_compativeis(filho->tipo, TP_Integer);
		fator_gera_menos(ret,filho);
	} else if (not) {
		tipos_compativeis(to_logico(filho->tipo), TP_Logico);
		fator_gera_not(ret,filho);
	}

	do {
		/* operacoes int x int -> int */
		if (reg_lex.token == Mais || reg_lex.token == Menos) {

			/* codegen */
			guarda_op(ret);

			lexan();

			NOVO_FATOR(filho2);
			filho2 = termo();

			/* acao semantica */
			tipos_compativeis(ret->tipo, TP_Integer);
			tipos_compativeis(filho2->tipo, TP_Integer);

			/* codegen */
			gen_op_termos(ret,filho2);

			ret->tipo = TP_Integer;

			/* operacoes logico x logico -> logico */
		} else if (reg_lex.token == Or) {

			/* codegen */
			guarda_op(ret);
			lexan();

			NOVO_FATOR(filho2);
			filho2 = termo();

			/* acao semantica */
			tipos_compativeis(to_logico(ret->tipo), TP_Logico);
			tipos_compativeis(to_logico(filho2->tipo), TP_Logico);

			/* codegen */
			gen_op_termos(ret,filho2);

			ret->tipo = TP_Logico;
		}

	} while(reg_lex.token == Mais || 
			reg_lex.token == Menos || reg_lex.token == Or);

	del(pilha);
	return ret;
}


/* Termo
 * T  -> F {( * | / | % | ‘and’ ) F}
 */
struct Expr *
termo(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN("termo");

	NOVO_FATOR(atual);
	NOVO_FATOR(filho);

	filho = fator();

	/* codegen */
	atualiza_pai(atual,filho);

	do {
		/* operacoes int x int -> int */
		if (reg_lex.token == Vezes ||
				reg_lex.token == Barra || reg_lex.token == Porcento )
		{
			fator_op_fator(atual, TP_Integer);
		}
		/* operacoes logico x logico -> logico */
		else if (reg_lex.token == And) {
			fator_op_fator(atual, TP_Logico);
		}
	} while (reg_lex.token == Vezes || reg_lex.token == Barra ||
			reg_lex.token == Porcento || reg_lex.token == And);

	del(pilha);
	return atual;
}

/* Fator
 * F  -> ‘(‘ X ‘)’ | literal | id ['[' X ']']
 */
struct Expr *
fator(void)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN("fator");

	NOVO_FATOR(ret);

	int eh_array = 0;
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

		if (reg_lex.tipo == TP_Char)
			ret->tamanho = strlen(remove_aspas(lexAux));
		else
			ret->tamanho = 1;

		/* codegen */
		fator_gera_literal(ret,lexAux);
		
		lexan();

	/* fator -> id */
	} else {
		lexId = reg_lex.lexema;

		casa_token(Identificador);

		/* acao semantica */
		eh_id_ja_declarado(lexId);


		NOVO_FATOR(aux);
		/* lendo array: id[expressao()] */
		if (reg_lex.token == A_Colchete) {
			lexan();

			aux = expressao();

			/* acao semantica */
			tipos_compativeis(aux->tipo, TP_Integer);

			atr_pos(1); eh_array = 1;

			casa_token(F_Colchete);
		}

		/* codegen */
		if (eh_array)
			fator_gera_array(ret,aux,lexId);
		else
			fator_gera_id(ret,lexId);

		ret->tipo = busca_tipo(lexId);
	}

	del(pilha);
	return ret;
}


/* lista de expressoes */
void
lista_expressoes(int ln)
{
	/* DEBUGGER E PILHA */
	DEBUGSIN("lista_expressoes");
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
