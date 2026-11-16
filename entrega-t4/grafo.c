#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "grafo.h"

// funções que implementam as operações básicas de um grafo
//
// um grafo é constituido por nós e arestas que os interligam.
// existe um valor associado a cada nó e a cada aresta. Esses valores são
//   armazenados pelo grafo, mas o grafo desconhece seu tipo, só conhece
//   seu tamanho.
// os nós são identificados por um inteiro que corresponde à ordem em que
//   são inseridos no grafo, com 0 correspondendo ao primeiro.
// a identificação de um nó é sempre um número inferior ao número de nós
//   no grafo. A remoção de um nó altera a identificação dos nós com números
//   superiores.
// as arestas são direcionadas, e são identificadas pelo seu nó de origem
//   e de destino.
// os valores das arestas e dos nós são copiados para o grafo. O acesso a
//   esses valores é por meio de ponteiros. O usuário do grafo pode alterar
//   o valor armazenado no grafo após obter o ponteiro (com g_valor_do_nó
//   por exemplo). O usuário pode também alterar o valor usando uma função
//   de alteração (como g_altera_valor_do_nó).

typedef struct Aresta aresta;
typedef struct No no;

struct Aresta {
    int origem;
    int destino;
    void *valor;
    aresta *prox_saida;
    aresta *prox_entrada;
};

struct No {
    void *valor;
    aresta *saidas;
    aresta *entradas;
};

struct grafo {
    int tam_no;
    int tam_aresta;
    int qtd_no;
    int capacidade_no;
    no *nos;
    bool consulta_saida;
    aresta *corrente;
};


// cria um grafo vazio que suporta dados do tamanho fornecido (em bytes)
//   nos nós e nas arestas
Grafo g_cria(int tam_nó, int tam_aresta){
    Grafo g = malloc(sizeof(*g));
    assert(g != NULL);
    g->tam_no = tam_nó;
    g->tam_aresta = tam_aresta;
    g->qtd_no = 0;
    g->capacidade_no = 4;
    g->nos = malloc(sizeof(*g->nos) * g->capacidade_no);
    assert(g->nos != NULL);
    g->consulta_saida = false;
    g->corrente = NULL;
    return g;
}

// libera a memória ocupada por um grafo
void g_destrói(Grafo self){
    for(int i = 0; i < self->qtd_no; i++){
        free(self->nos[i].valor);
        aresta *aux = self->nos[i].saidas;
        while(aux != NULL){
            aresta *temp = aux->prox_saida;
            free(aux->valor);
            free(aux);
            aux = temp;
        }
    }
    free(self->nos);
    free(self);
}

// Nós

// insere um nó no grafo, com o dado apontado por p_dado
// retorna o número do novo nó (que é o número de nós que
//   existia antes da inserção)
int g_insere_nó(Grafo self, void *p_dado){
    if(self->qtd_no == self->capacidade_no){
        int nova_capacidade = self->capacidade_no * 2;
        no *novo = realloc(self->nos, sizeof(*self->nos) * nova_capacidade);
        assert(novo != NULL);
        self->nos = novo;
        self->capacidade_no = nova_capacidade;
    }
    int pos = self->qtd_no;
    self->nos[pos].valor = malloc(self->tam_no);
    assert(self->nos[pos].valor != NULL);
    memcpy(self->nos[pos].valor, p_dado, self->tam_no);
    self->nos[pos].saidas = NULL;
    self->nos[pos].entradas = NULL;
    self->qtd_no++;
    return pos;
}

static void remove_aresta(Grafo self, int origem, int destino){
    aresta *ant = NULL;
    aresta *a = self->nos[origem].saidas;
    while(a != NULL && a->destino != destino){
        ant = a;
        a = a->prox_saida;
    }
    if(a == NULL){
        return;
    }
    if(ant == NULL){
        self->nos[origem].saidas = a->prox_saida;
    } else {
        ant->prox_saida = a->prox_saida;
    }
    ant = NULL;
    aresta *e = self->nos[destino].entradas;
    while(e != NULL && e != a){
        ant = e;
        e = e->prox_entrada;
    }
    assert(e != NULL);
    if(ant == NULL){
        self->nos[destino].entradas = a->prox_entrada;
    } else {
        ant->prox_entrada = a->prox_entrada;
    }
    free(a->valor);
    free(a);
}

// remove um nó do grafo e as arestas incidentes nesse nó
// a identificação dos nós remanescentes é alterada, como se esse nó nunca
//   tivesse existido
void g_remove_nó(Grafo self, int nó){
    assert(nó >=0 && nó <self->qtd_no);
    while(self->nos[nó].saidas != NULL){
        remove_aresta(self, nó, self->nos[nó].saidas->destino);
    }
    while(self->nos[nó].entradas != NULL){
        remove_aresta(self, self->nos[nó].entradas->origem, nó);
    }
    free(self->nos[nó].valor);
    memmove(&self->nos[nó], &self->nos[nó + 1], sizeof(*self->nos) * (self->qtd_no - nó - 1));
    self->qtd_no--;
    for(int i = 0; i < self->qtd_no; i++){
        for(aresta *a = self->nos[i].saidas; a != NULL; a = a->prox_saida){
            a->origem = i;
            if(a->destino > nó){
                a->destino--;
            }
        }
    }
    self->corrente = NULL;
}

// altera o valor associado a um nó (copia o valor apontado por p_dado para
//   uma memória gerenciada pelo grafo)
void g_altera_valor_do_nó(Grafo self, int nó, void *p_dado){
    assert(nó >= 0 && nó < self->qtd_no);
    memcpy(self->nos[nó].valor, p_dado, self->tam_no);
}

// retorna um ponteiro para o valor associado a um nó que está na memória
//   gerenciada pelo grafo.
void *g_valor_do_nó(Grafo self, int nó){
    assert(nó >= 0 && nó < self->qtd_no);
    return self->nos[nó].valor;
}

// retorna o número de nós do grafo
int g_num_nós(Grafo self){
    return self->qtd_no;
}

// Arestas

// altera o valor da aresta que interliga o nó origem ao nó destino
//   (copia de *p_dado para uma memória gerenciada pelo grafo)
// caso a aresta não exista, deve ser criada
// caso p_dado seja NULL, a aresta deve ser removida
void g_altera_valor_da_aresta(Grafo self, int nó_origem, int nó_destino,
                              void *p_dado);

// retorna um ponteiro para o valor associado à aresta entre nó_origem e
//   nó_destino, localizado em memória gerenciada pelo grafo.
// retorna NULL se tal aresta não existir
void *g_valor_da_aresta(Grafo self, int nó_origem, int nó_destino);

// inicia uma consulta a arestas que partem do nó origem
// as próximas chamadas a 'g_próxima_aresta' devem retornar os valores
//   correspondentes a cada aresta que parte desse nó
void g_arestas_que_partem(Grafo self, int nó_origem);

// inicia uma consulta a arestas que chegam ao nó destino
// as próximas chamadas a 'g_próxima_aresta' devem retornar os valores
//   correspondentes a cada aresta que chega nesse nó
void g_arestas_que_chegam(Grafo self, int nó_destino);

// retorna um ponteiro para o dado associado à próxima aresta, de acordo
//   com a última consulta iniciada por 'g_arestas_que_partem' ou
//   'g_arestas_que_chegam'
// o valor do nó vizinho ao nó da consulta deve ser colocado em '*p_nó_vizinho'
//   (se não for NULL)
// caso não exista mais aresta que satisfaça a consulta, retorna NULL.
void *g_próxima_aresta(Grafo self, int *p_nó_vizinho);

// Algoritmos

// retorna true se grafo é cíclico, false caso contrário
bool g_tem_ciclo(Grafo self);

// retorna uma fila de inteiros contendo os números dos nós do grafo em
//   uma ordem em que, se o nó 'a' antecede 'b', não existe uma aresta
//   de 'b' para 'a' no grafo
// deve retornar uma fila vazia caso tal ordem não exista
// quem chama esta função é responsável por destruir a fila.
Fila g_ordem_topológica(Grafo self);
