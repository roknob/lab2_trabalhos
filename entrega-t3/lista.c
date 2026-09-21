#include "lista.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct nó nó;

struct nó
{
  dado_t dado;
  nó *ant;
  nó *prox;
};

struct lista
{
  nó *sentinela;
  int tamanho;
};

static void l_ok(Lista l)
{
  assert(l != NULL);
  int t = 0;
  nó *n = l->sentinela->prox;
  while(n != l->sentinela){
    nó *prox = n->prox;
    assert(prox->ant == n);
    n = prox;
    t++;
  }
  assert(t == l->tamanho);
}

// cria e inicializa uma lista vazia
Lista l_cria()
{
  Lista l = malloc(sizeof(*l));
  assert(l != NULL);
  nó *sent = malloc(sizeof(*sent));
  assert(sent != NULL);
  l->sentinela = sent;
  l->sentinela->ant = l->sentinela;
  l->sentinela->prox = l->sentinela;
  l->tamanho = 0;
  return l;
}

// cria uma lista contendo substrings de s
// as substrings são separadas por quaisquer caractere de sep
// os caracteres de sep não aparecem nas substrings
// exemplos:
//   "a,ba,ca, te", ", " -> ["a" "ba" "ca" "te"]
//   "aba \ncate\n", "\n" -> ["aba " "cate"]
Lista l_cria_separando(Str s, Str sep)
{
  //
}

// libera a memória ocupada por uma lista
void l_destroi(Lista l)
{
  l_ok(l);
  nó *n = l->sentinela->prox;
  while(n != l->sentinela){
    nó *prox = n->prox;
    s_destroi(n->dado);
    free(n);
    n = prox;
  }
  free(l->sentinela);
  free (l);
}

// retorna o número de elementos na lista
int l_tam(Lista l)
{
  l_ok(l);
  return l->tamanho;
}

// retorna true se a lista tiver cheia
bool l_cheia(Lista l)
{
  l_ok(l);
  return false;
}

// retorna true se a lista tiver vazia
bool l_vazia(Lista l)
{
  l_ok(l);
  if(l->sentinela->ant == l->sentinela && l->sentinela->prox == l->sentinela) {
    return true;
  } else {
    return false;
  }
}

// imprime os dados que estão na lista
void l_imprime(Lista l)
{
  l_ok(l);
  printf("[ ");
  for (nó *n = l->sentinela->prox; n != l->sentinela; n = n->prox) {
    s_imprime(n->dado);
  }
  printf("]\n");
}

// insere o dado d no início da lista l
void l_insere_inicio(Lista l, dado_t d)
{
  l_ok(l);
  nó *novo = malloc(sizeof(*novo));
  assert(novo != NULL);
  novo->dado = d;
  nó *antigo_primeiro = l->sentinela->prox;
  novo->prox = antigo_primeiro;
  novo->ant = l->sentinela;
  antigo_primeiro->ant = novo;
  l->sentinela->prox = novo;
  l->tamanho++;
}

// insere o dado d no final da lista l
void l_insere_fim(Lista l, dado_t d)
{
  l_ok(l);
  nó *novo = malloc(sizeof(*novo));
  assert(novo != NULL);
  novo->dado = d;
  nó *antigo_ultimo = l->sentinela->ant;
  novo->prox = l->sentinela;
  novo->ant = antigo_ultimo;
  antigo_ultimo->prox = novo;
  l->sentinela->ant = novo;
  l->tamanho++;
}

static nó *no_anterior(Lista l, int p){
  nó *n;
  if(p == 0) {
    return l->sentinela;
  } else {
    n = l->sentinela;
    for(int i = 0; i < p; i++){
        n = n->prox;
    }
  }
  return n;
}

// insere o dado d na lista l, de forma que ele fique na posição p
// a primeira posição é 0
void l_insere_pos(Lista l, dado_t d, int p)
{
  l_ok(l);
  nó *no_ant = no_anterior(l, p);
  nó *novo = malloc(sizeof(*novo));
  assert(novo != NULL);
  novo->dado = d;
  nó *antigo_p = no_ant->prox;
  novo->prox = antigo_p;
  novo->ant = no_ant;
  antigo_p->ant = novo;
  no_ant->prox = novo;
  l->tamanho++;
}

// retorna o dado no início da lista
dado_t l_dado_inicio(Lista l)
{
  l_ok(l);
  assert(!l_vazia(l));
  return l->sentinela->prox->dado;
}

// retorna o dado no final da lista
dado_t l_dado_fim(Lista l)
{
  l_ok(l);
  assert(!l_vazia(l));
  return l->sentinela->ant->dado;
}

// retorna o dado na posição pos da lista
dado_t l_dado_pos(Lista l, int pos)
{
  l_ok(l);
  nó *n;
  if(pos == 0) {
    return l_dado_inicio(l);
  } else {
    n = l->sentinela->prox;
    for(int i = 0; i < pos; i++){
        n = n->prox;
    }
  }
  return n->dado;
}

// remove e retorna o dado no início da lista
dado_t l_remove_inicio(Lista l)
{
  l_ok(l);
  nó *remover = l->sentinela->prox;
  Str dado_removido = remover->dado;
  nó *seguinte = remover->prox;
  l->sentinela->prox = seguinte;
  seguinte->ant = l->sentinela;
  free(remover);
  l->tamanho--;
  return dado_removido;
}

// remove e retorna o dado no final da lista
dado_t l_remove_fim(Lista l)
{
  l_ok(l);
  nó *remover = l->sentinela->ant;
  Str dado_removido = remover->dado;
  nó *anterior = remover->ant;
  l->sentinela->ant = anterior;
  anterior->prox = l->sentinela;
  free(remover);
  l->tamanho--;
  return dado_removido;
}

// remove e retorna o dado na posição pos da lista
dado_t l_remove_pos(Lista l, int pos)
{
  l_ok(l);
  nó *anterior = no_anterior(l, pos);
  nó *remover = anterior->prox;
  Str dado_removido = remover->dado;
  nó *proximo = remover->prox;
  anterior->prox = proximo;
  proximo->ant = anterior;
  free(remover);
  l->tamanho--;
  return dado_removido;
}


// funções para usar a lista como uma fila

// l_cria, l_destroi, l_vazia

// retorna o dado que está no início da fila
dado_t l_primeiro(Lista l)
{
  return l_dado_inicio(l);
}

// insere um dado no fim da fila
void l_insere(Lista l, dado_t d)
{
  l_insere_fim(l, d);
}

// remove e retorna o dado que está no início da fila
dado_t l_remove(Lista l)
{
  return l_remove_inicio(l);
}


// funções para usar a lista como uma pilha

// l_cria, l_destroi, l_vazia

// retorna o dado que está no topo da pilha
dado_t l_topo(Lista l)
{
  return l_dado_inicio(l);
}

// empilha um dado no topo da pilha
void l_empilha(Lista l, dado_t d)
{
  l_insere_inicio(l, d);
}

// remove e retorna o dado que está no topo da pilha
dado_t l_desempilha(Lista l)
{
  return l_remove_inicio(l);
}