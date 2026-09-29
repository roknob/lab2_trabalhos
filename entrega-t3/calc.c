#include "calc.h"

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
  {'T', 'E', 'E', 'E', 'E', 'X'},
  {'O', 'O', 'E', 'E', 'E', 'O'},
  {'O', 'O', 'O', 'E', 'E', 'O'},
  {'O', 'O', 'O', 'E', 'E', 'O'},
  {'X', 'E', 'E', 'E', 'E', 'D'}
};

static int categoria_operador_pilha(Str operador)
{
  unichar c = s_ch(operador, 0);
  switch (c)
  {
    case 'V':
      return 0;
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
  unichar c = s_ch(operador, 0);
  switch (c)
  {
    case 'F':
      return 0;
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
    return s_cria("V");
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

static void acao_opera(Lista operadores, Lista operandos)
{
  Str operador = l_desempilha(operadores);
  Str s_operando = l_desempilha(operandos);
  Str p_operando = l_desempilha(operandos);
  double segundo_operando = s_número(s_operando);
  double primeiro_operando = s_número(p_operando);
  double resultado_operandos;
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
    resultado_operandos = primeiro_operando / segundo_operando;
    break;
  case '^':
    resultado_operandos = pow(primeiro_operando, segundo_operando);
    break;
  default:
    break;
  }
  Str resultado = s_cria_número(resultado_operandos);
  l_empilha(operandos, resultado);
  s_destroi(operador);
  s_destroi(s_operando);
  s_destroi(p_operando);
}

static void executa_acao(char acao, Lista operandos, Lista operadores, Str operador_e,
                          int *i, bool *terminou, Str *resultado_erro)
{
  if(acao == 'T'){
    *terminou = true;
  } else if(acao == 'X'){
    *resultado_erro = define_erro("erro de sintaxe");
    *terminou = true;
  } else if(acao == 'E'){
    acao_empilha(operadores, operador_e);
    (*i)++;
  } else if(acao == 'D'){
    acao_descarta(operadores);
    (*i)++;
  } else if(acao == 'O'){
    acao_opera(operadores, operandos);
  }
}

static bool le_proximo(Lista tokens, int *i, Lista operandos, 
                        bool *terminou, Str *resultado_erro, Str *operador_e)
{
  if(*i == l_tam(tokens)){
    *operador_e = s_cria("F");
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
    Str operador_e;
    bool compara_pxe = le_proximo(tokens, &i, operandos, &terminou, &resultado_erro, &operador_e);
    if(compara_pxe){
      int cat_p = categoria_operador_pilha(operador_p);
      int cat_e = categoria_operador_entrada(operador_e);
      char acao = tabela[cat_p][cat_e];
      executa_acao(acao, operandos, operadores, operador_e, &i, &terminou, &resultado_erro);
    }
  }
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
Lista tokeniza(Str txt);