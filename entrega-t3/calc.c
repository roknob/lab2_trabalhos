#include "calc.h"
#include "dicionario.h"

#include <stdlib.h>
#include <assert.h>
#include <math.h>

typedef struct calc *Calc;

struct calc
{
  Dicionário variáveis;
};

static bool igual(chave_t a, chave_t b)
{
  Str sa = a;
  Str sb = b;
  return s_igual(sa, sb);
}

static bool menor(chave_t a, chave_t b)
{
  (void) a;
  (void) b;
  return false;
}

static Calc calc_cria(void)
{
  Calc c = malloc(sizeof(*c));
  assert(c != NULL);
  c->variáveis = dic_cria(menor, igual);
  return c;
}

static void calc_destroi(Calc c)
{
  chave_t chave;
  valor_t valor;
  dic_inicia_percurso(c->variáveis);
  while(dic_próximo(c->variáveis, &chave, &valor)){
    s_destroi(chave);
    s_destroi(valor);
  }
  dic_destrói(c->variáveis);
  free(c);
}

static bool verifica_espaço(unichar c)
{
  return c == ' ' || c == '\t' || c == '\n';
}

static int pula_espacos(Str txt, int pos)
{
  while (verifica_espaço(s_ch(txt, pos))) {
    pos++;
  }
  return pos;
}

static bool verifica_dígito_ou_ponto(unichar c)
{
  return c == '.' || (c >= '0' && c <= '9');
}

static bool verifica_início_de_identificador(unichar c)
{
  return c == '$' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool verifica_continuação_de_identificador(unichar c)
{
  return verifica_início_de_identificador(c) || c == '_' || (c >= '0' && c <= '9');
}

static bool verificar_operador(unichar c)
{
  return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '(' || c == ')' || c == '=';
}

typedef enum
{
  t_operador,
  t_operando,
  t_erro
} tipo_t;

static tipo_t classifica_token(Str token)
{
  unichar c = s_ch(token, 0);
  if(verificar_operador(c)){
    return t_operador;
  }
  if(verifica_dígito_ou_ponto(c)){
    return t_operando;
  }
  if(verifica_início_de_identificador(c)){
    return t_operando;
  }
  return t_erro;
}

static char tabela[6][7] = 
{
  {'T', 'E', 'E', 'E', 'E', 'L', 'E'},
  {'O', 'O', 'E', 'E', 'E', 'O', 'E'},
  {'O', 'O', 'O', 'E', 'E', 'O', 'E'},
  {'O', 'O', 'O', 'E', 'E', 'O', 'E'},
  {'R', 'E', 'E', 'E', 'E', 'D', 'E'},
  {'O', 'E', 'E', 'E', 'E', 'O', 'E'}
};

static int categoria_operador_pilha(Str operador)
{
  if(operador == NULL){
    return 0;
  }
  unichar c = s_ch(operador, 0);
  switch (c)
  {
    case '+':
    case '-':
      return 1;
    case '*':
    case '/':
      return 2;
    case '^':
      return 3;
    case '(':
      return 4;
    case '=':
      return 5;
    default:
      return -1;
  }
}

static int categoria_operador_entrada(Str operador)
{
  if(operador == NULL){
    return 0;
  }
  unichar c = s_ch(operador, 0);
  switch (c)
  {
    case '+':
    case '-':
      return 1;
    case '*':
    case '/':
      return 2;
    case '^':
      return 3;
    case '(':
      return 4;
    case ')':
      return 5;
    case '=':
      return 6;
    default:
      return -1;
  }
}

static Str obtem_p(Lista operadores)
{
  if(l_vazia(operadores)) {
    return NULL;
  }
  return l_topo(operadores);
}

static void acao_empilha(Lista operadores, Str token)
{
  Str operador_atual = s_cria_cópia(token);
  l_empilha(operadores, operador_atual);
}

static void acao_descarta(Lista operadores)
{
  Str descartado = l_desempilha(operadores);
  s_destroi(descartado);
}

static Str define_erro(char *mensagem)
{
  char *inicio_mensagem_erro = "#ERRO ";
  Str mensagem_erro = s_cria(inicio_mensagem_erro);
  Str fim_mensagem_erro = s_cria(mensagem);
  s_anexa(mensagem_erro, fim_mensagem_erro);
  s_destroi(fim_mensagem_erro);
  return mensagem_erro;
}

static bool valor_operando(Dicionário variáveis, Str operando, double *valor)
{
  unichar c = s_ch(operando, 0);
  if(verifica_dígito_ou_ponto(c)){
    *valor = s_número(operando);
    return true;
  }
  valor_t encontrado = dic_busca(variáveis, operando);
  if(encontrado == VALOR_NÃO_EXISTE){
    return false;
  }
  Str s_valor = encontrado;
  *valor = s_número(s_valor);
  return true;
}

static bool calcula(unichar op, double a, double b, double *resultado)
{
  *resultado = 0;
  switch (op)
  {
  case '+':
    *resultado = a + b;
    break;
  case '-':
    *resultado = a - b;
    break;
  case '*':
    *resultado = a * b;
    break;
  case '/':
    if(b == 0){
      return false;
    }
    *resultado = a / b;
    break;
  case '^':
    *resultado = pow(a, b);
    break;
  default:
    break;
  }
  return true;
}

static Str acao_atribui(Dicionário variáveis, Lista operadores, Lista operandos)
{
  if(l_tam(operandos) < 2){
    return define_erro("operandos insuficientes");
  }
  Str operador = l_desempilha(operadores);
  Str direita = l_desempilha(operandos);
  Str esquerda = l_desempilha(operandos);
  s_destroi(operador);
  if(!verifica_início_de_identificador(s_ch(esquerda, 0))){
    s_destroi(esquerda);
    s_destroi(direita);
    return define_erro("atribuicao a um nao nome");
  }
  Str valor;
  if(verifica_início_de_identificador(s_ch(direita, 0))){
    valor_t encontrado = dic_busca(variáveis, direita);
    s_destroi(direita);
    if(encontrado == VALOR_NÃO_EXISTE){
      s_destroi(esquerda);
      return define_erro("variavel nao definida");
    }
    valor = s_cria_cópia(encontrado);
  } else {
    valor = direita;
  }
  Str anterior = dic_insere(variáveis, esquerda, valor);
  if(anterior != VALOR_NÃO_EXISTE){
    s_destroi(anterior);
    s_destroi(esquerda);
  }
  l_empilha(operandos, s_cria_cópia(valor));
  return NULL;
}

static Str acao_opera(Dicionário variáveis, Lista operadores, Lista operandos)
{
  if(l_tam(operandos) < 2){
    return define_erro("operandos insuficientes");
  }
  if(s_ch(l_topo(operadores), 0) == '='){
    return acao_atribui(variáveis, operadores, operandos);
  }
  Str operador = l_desempilha(operadores);
  Str s_operando = l_desempilha(operandos);
  Str p_operando = l_desempilha(operandos);
  double segundo = 0;
  double primeiro = 0;
  bool ok_segundo = valor_operando(variáveis, s_operando, &segundo);
  bool ok_primeiro = valor_operando(variáveis, p_operando, &primeiro);
  unichar op = s_ch(operador, 0);
  s_destroi(operador);
  s_destroi(s_operando);
  s_destroi(p_operando);
  if(!ok_segundo || !ok_primeiro){
    return define_erro("variavel nao definida");
  }
  double resultado;
  if(!calcula(op, primeiro, segundo, &resultado)){
    return define_erro("divisao por zero");
  }
  l_empilha(operandos, s_cria_número(resultado));
  return NULL;
}

static void executa_acao(Dicionário variáveis, char acao, Lista operandos, Lista operadores,
                          Str operador_e, int *i, bool *terminou, Str *resultado_erro)
{
  if(acao == 'T'){
    *terminou = true;
  } else if(acao == 'L'){
    *resultado_erro = define_erro("falta (");
    *terminou = true;
  } else if(acao == 'R'){
    *resultado_erro = define_erro("falta )");
    *terminou = true;
  } else if(acao == 'E'){
    acao_empilha(operadores, operador_e);
    (*i)++;
  } else if(acao == 'D'){
    acao_descarta(operadores);
    (*i)++;
  } else if(acao == 'O'){
    Str erro = acao_opera(variáveis, operadores, operandos);
    if(erro != NULL){
      *resultado_erro = erro;
      *terminou = true;
    }
  }
}

static bool le_proximo(Lista tokens, int *i, Lista operandos, 
                        bool *terminou, Str *resultado_erro, Str *operador_e)
{
  if(*i == l_tam(tokens)){
    *operador_e = NULL;
    return true;
  }
  Str token_atual = l_dado_pos(tokens, *i);
  tipo_t tipo = classifica_token(token_atual);
  if(tipo == t_operando){
    acao_empilha(operandos, token_atual);
    (*i)++;
    return false;
  } else if(tipo == t_erro){
    *resultado_erro = define_erro("caractere invalido");
    *terminou = true;
    return false;
  } else {
    *operador_e = token_atual;
    return true;
  }
}

static Str monta_resultado(Dicionário variáveis, Lista tokens, Lista operandos,
                           Lista operadores, Str resultado_erro)
{
  Str resultado;
  if(resultado_erro != NULL){
    resultado = resultado_erro;
  } else if(l_tam(operandos) != 1) {
    resultado = define_erro("expressao invalida");
  } else {
    Str operando = l_desempilha(operandos);
    if(verifica_início_de_identificador(s_ch(operando, 0))){
      valor_t encontrado = dic_busca(variáveis, operando);
      if(encontrado == VALOR_NÃO_EXISTE){
        resultado = define_erro("variavel nao definida");
      } else {
        resultado = s_cria_cópia(encontrado);
      }
      s_destroi(operando);
    } else {
      resultado = operando;
    }
  }
  l_destroi(tokens);
  l_destroi(operandos);
  l_destroi(operadores);
  return resultado;
}

// Calcula o valor de expressão e retorna uma nova Str contendo o resultado.
// Em cado de erro, os primeiros caracteres da Str de retorno são "#ERRO ".
static Str calcula_expressao(Calc c, Str expressão)
{
  Dicionário variáveis = c->variáveis;
  Lista tokens = tokeniza(expressão);
  Lista operandos = l_cria();
  Lista operadores = l_cria();
  bool terminou = false;
  Str resultado_erro = NULL;
  int i = 0;
  while(!terminou){
    Str operador_p = obtem_p(operadores);
    Str operador_e = NULL;
    bool compara_pxe = le_proximo(tokens, &i, operandos, &terminou, &resultado_erro, &operador_e);
    if(compara_pxe){
      int cat_p = categoria_operador_pilha(operador_p);
      int cat_e = categoria_operador_entrada(operador_e);
      if(cat_p < 0 || cat_e < 0){
        resultado_erro = define_erro("operador invalido");
        terminou = true;
      } else {
        char acao = tabela[cat_p][cat_e];
        executa_acao(variáveis, acao, operandos, operadores, operador_e, &i, &terminou, &resultado_erro);
      }
    }
  }
  Str resultado = monta_resultado(variáveis, tokens, operandos, operadores, resultado_erro);
  return resultado;
}

static int acha_fim_token(Str txt, int inicio)
{
  unichar c = s_ch(txt, inicio);
  if(verificar_operador(c)){
    return inicio + 1;
  }
  if(verifica_dígito_ou_ponto(c)){
    int pos = inicio + 1;
    while(verifica_dígito_ou_ponto(s_ch(txt, pos))){
      pos++;
    }
    return pos;
  }
  if(verifica_início_de_identificador(c)){
    int pos = inicio + 1;
    while(verifica_continuação_de_identificador(s_ch(txt, pos))){
      pos++;
    }
    return pos;
  }
  return -1;
}

static Calc calc_global = NULL;

static void destroi_calc_global(void)
{
  if(calc_global != NULL){
    calc_destroi(calc_global);
    calc_global = NULL;
  }
}

Str calculadora(Str expressão)
{
  if(calc_global == NULL){
    calc_global = calc_cria();
    atexit(destroi_calc_global);
  }
  return calcula_expressao(calc_global, expressão);
}

Lista tokeniza(Str txt)
{
  Lista tokens = l_cria();
  int pos = 0;
  int tam = s_tam(txt);
  while(true){
    pos = pula_espacos(txt, pos);
    if(pos == tam){
      break;
    }
    int fim = acha_fim_token(txt, pos);
    if(fim == -1){
      fim = pos + 1;
    }
    Str token = s_cria_substring(txt, pos, fim - pos);
    l_insere_fim(tokens, token);
    pos = fim;
  }
  return tokens;
}