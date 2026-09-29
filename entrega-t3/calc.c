#include "calc.h"
#include "dicionario.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

typedef enum {
  t_operador,
  t_operando,
  t_erro
} tipo_t;

static tipo_t classifica_token(Str token)
{
  unichar c = s_ch(token, 0);
  if(c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '(' || c == ')' || c == '='){
    return t_operador;
  }
  if((c >='0' && c<='9') || c == '.'){
    return t_operando;
  }
  return t_erro;
}

static char tabela[5][6] = 
{
  {'T', 'E', 'E', 'E', 'E', 'L'},   // L: falta '('
  {'O', 'O', 'E', 'E', 'E', 'O'},
  {'O', 'O', 'O', 'E', 'E', 'O'},
  {'O', 'O', 'O', 'E', 'E', 'O'},
  {'R', 'E', 'E', 'E', 'E', 'D'}    // R: falta ')'
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

static Str acao_opera(Lista operadores, Lista operandos)
{
  if(l_tam(operandos) < 2){
    return define_erro("operandos insuficientes");
  }
  Str operador = l_desempilha(operadores);
  Str s_operando = l_desempilha(operandos);
  Str p_operando = l_desempilha(operandos);
  double segundo_operando = s_número(s_operando);
  double primeiro_operando = s_número(p_operando);
  double resultado_operandos = 0;
  Str erro = NULL;
  unichar c = s_ch(operador, 0);
  switch (c)
  {
  case '+':
    resultado_operandos = primeiro_operando + segundo_operando;
    break;
  case '-':
    resultado_operandos = primeiro_operando - segundo_operando;
    break;
  case '*':
    resultado_operandos = primeiro_operando * segundo_operando;
    break;
  case '/':
    if(segundo_operando == 0){
      erro = define_erro("divisao por zero");
    } else {
      resultado_operandos = primeiro_operando / segundo_operando;
    }
    break;
  case '^':
    resultado_operandos = pow(primeiro_operando, segundo_operando);
    break;
  default:
    break;
  }
  s_destroi(operador);
  s_destroi(s_operando);
  s_destroi(p_operando);
  if(erro != NULL){
    return erro;
  }
  Str resultado = s_cria_número(resultado_operandos);
  l_empilha(operandos, resultado);
  return NULL;
}

static void executa_acao(char acao, Lista operandos, Lista operadores, Str operador_e,
                          int *i, bool *terminou, Str *resultado_erro)
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
    Str erro = acao_opera(operadores, operandos);
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

static Str monta_resultado(Lista tokens, Lista operandos, Lista operadores, Str resultado_erro)
{
  Str resultado;
  if(resultado_erro != NULL){
    resultado = resultado_erro;
  } else if(l_tam(operandos) != 1) {
    resultado = define_erro("expressao invalida");
  } else {
    resultado = l_desempilha(operandos);
  }
  l_destroi(tokens);
  l_destroi(operandos);
  l_destroi(operadores);
  return resultado;
}

// Calcula o valor de expressão e retorna uma nova Str contendo o resultado.
// Em cado de erro, os primeiros caracteres da Str de retorno são "#ERRO ".
Str calculadora(Str expressão)
{
  Lista tokens = tokeniza(expressão); 
  Lista operandos = l_cria();
  Lista operadores = l_cria();
  bool terminou = false;
  Str resultado_erro = NULL;  // fica NULL se não houver erro
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
        executa_acao(acao, operandos, operadores, operador_e, &i, &terminou, &resultado_erro);
      }
    }
  }
  Str resultado = monta_resultado(tokens, operandos, operadores, resultado_erro);
  return resultado;
}

// Retorna uma nova Lista contendo substrings de txt.
// Uma substring inicia em um caractere diferente de espaço, tabulação,
//   fim de linha.
// Se a substring inicia por um dígito ou um ponto, contém os demais dígitos
//   ou pontos que seguem.
// Se a substring inicia por uma letra ou sublinhado ou `$`, contém os
//   demais letras, sublinhados, `$` ou dígitos que seguem.
// Se a substring inicia por outro caractere, contém somente esse caractere.
// Exemplos:
// " 9. 5" -> ["9." "5"]
// "92+a ba 3b3 ** *  " -> ["92" "+" "a" "ba" "3" "b3" "*" "*" "*"]

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