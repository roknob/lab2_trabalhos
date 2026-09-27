#include "calc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef enum {
  t_operador,
  t_operando,
  r_erro
} tipo_t;

static tipo_t(Str token)
{
  unichar c = s_ch(token, 0);
  if(c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '(' || c == ')' || c == '='){
    return t_operador;
  }
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

// Calcula o valor de expressão e retorna uma nova Str contendo o resultado.
// Em cado de erro, os primeiros caracteres da Str de retorno são "#ERRO ".
Str calculadora(Str expressão)
{
  Lista tokens = tokeniza(expressão); 
  Lista operandos = l_cria();
  Lista operadores = l_cria();
  int i = 0;
  while(true){
    Str operador_p;
    if(l_vazia(operadores)){
      operador_p = s_cria("V");
    } else {
      operador_p = l_topo(operadores);
    }
    Str operador_e;
    if(i == l_tam(tokens)){
      operador_e = s_cria("F");
    } else {
      operador_e = l_dado_pos(tokens, i);
    }
    int cat_p = categoria_operador_pilha(operador_p);
    int cat_e = categoria_operador_entrada(operador_e);
    char acao = tabela[cat_p][cat_e];
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