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

Lista l_cria_separando(Str s, Str sep)
{
  Lista nova_lista = l_cria();
  int pos = 0;
  int tam = 0;
  while(pos >= 0){
    int inicio = s_busca_nc(s, pos, sep);
    if(inicio == -1){
      break;
    }
    int fim = s_busca_c(s, inicio, sep);
    if(fim == -1){
      tam = -1;
    } else {
      tam = fim - inicio;
    }
    Str nova_substring = s_cria_substring(s, inicio, tam);
    l_insere_fim(nova_lista, nova_substring);
    pos = fim;
  }
  return nova_lista;
}

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

int l_tam(Lista l)
{
  l_ok(l);
  return l->tamanho;
}

bool l_cheia(Lista l)
{
  l_ok(l);
  return false;
}

bool l_vazia(Lista l)
{
  l_ok(l);
  if(l->sentinela->ant == l->sentinela && l->sentinela->prox == l->sentinela) {
    return true;
  } else {
    return false;
  }
}

void l_imprime(Lista l)
{
  l_ok(l);
  printf("[ ");
  for (nó *n = l->sentinela->prox; n != l->sentinela; n = n->prox) {
    s_imprime(n->dado);
  }
  printf("]\n");
}

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

dado_t l_dado_inicio(Lista l)
{
  l_ok(l);
  assert(!l_vazia(l));
  return l->sentinela->prox->dado;
}

dado_t l_dado_fim(Lista l)
{
  l_ok(l);
  assert(!l_vazia(l));
  return l->sentinela->ant->dado;
}

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

dado_t l_remove_inicio(Lista l)
{
  l_ok(l);
  assert(!l_vazia(l));
  nó *remover = l->sentinela->prox;
  Str dado_removido = remover->dado;
  nó *seguinte = remover->prox;
  l->sentinela->prox = seguinte;
  seguinte->ant = l->sentinela;
  free(remover);
  l->tamanho--;
  return dado_removido;
}

dado_t l_remove_fim(Lista l)
{
  l_ok(l);
  assert(!l_vazia(l));
  nó *remover = l->sentinela->ant;
  Str dado_removido = remover->dado;
  nó *anterior = remover->ant;
  l->sentinela->ant = anterior;
  anterior->prox = l->sentinela;
  free(remover);
  l->tamanho--;
  return dado_removido;
}

dado_t l_remove_pos(Lista l, int pos)
{
  l_ok(l);
  assert(!l_vazia(l));
  assert(pos >= 0 && pos < l->tamanho);
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

dado_t l_primeiro(Lista l)
{
  return l_dado_inicio(l);
}

void l_insere(Lista l, dado_t d)
{
  l_insere_fim(l, d);
}

dado_t l_remove(Lista l)
{
  return l_remove_inicio(l);
}

dado_t l_topo(Lista l)
{
  return l_dado_inicio(l);
}

void l_empilha(Lista l, dado_t d)
{
  l_insere_inicio(l, d);
}

dado_t l_desempilha(Lista l)
{
  return l_remove_inicio(l);
}