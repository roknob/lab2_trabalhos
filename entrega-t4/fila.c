#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "fila.h"

typedef struct No no;

struct No{
    no *prox;
    no *ant;
    void *dado;
};

struct fila{
    int tam_dado;
    no *inicio;
    no *fim;
    no *proximo;
    int n_elementos;
    int direcao;
};


// funções que implementam as operações básicas de uma fila

// cria uma fila vazia que suporta dados do tamanho fornecido (em bytes)
Fila f_cria(int tam_do_dado){
    Fila f = malloc(sizeof(*f));
    assert(f != NULL);
    f->tam_dado = tam_do_dado;
    f->inicio = NULL;
    f->fim = NULL;
    f->proximo = NULL;
    f->n_elementos = 0;
    f->direcao = 1;
    return f;
}

// libera a memória ocupada pela fila
void f_destrói(Fila self){
    no *aux = self->inicio;
    while(aux != NULL){
        no *temp = aux->prox;
        free(aux->dado);
        free(aux);
        aux = temp;
    }
    free(self);
}

// diz se a fila está vazia
bool f_tá_vazia(Fila self){
    if(self->n_elementos ==  0){
        return true;
    } else {
        return false;
    }
}

// remove o dado no início da fila e, se pdado não for NULL, copia o dado
//   removido para *pdado
void f_remove(Fila self, void *pdado){
    assert(!f_tá_vazia(self));
    no *temp = self->inicio;
    if(pdado != NULL){
        memcpy(pdado, temp->dado, self->tam_dado);
    }
    self->inicio = temp->prox;
    if(self->inicio != NULL){
        self->inicio->ant = NULL;
    } else {
        self->fim = NULL;
    }
    free(temp->dado);
    free(temp);
    self->n_elementos--;
}

// insere o dado apontado por pdado no final da fila
void f_insere(Fila self, void *pdado){
    no *novo = malloc(sizeof(*novo));
    assert(novo != NULL);
    novo->dado = malloc(self->tam_dado);
    assert(novo->dado != NULL);
    memcpy(novo->dado, pdado, self->tam_dado);
    novo->prox = NULL;
    novo->ant = self->fim;
    if(self->fim == NULL){
        self->inicio = novo;
    } else {
        self->fim->prox = novo;
    }
    self->fim = novo;
    self->n_elementos++;
}

// funções que implementam operações complementares, que permitem acesso
//   a todos os elementos da fila. Durante um percurso, a fila não pode ser
//   alterada.

// inicia um percurso aos elementos da fila, a partir de uma posição inicial
// se a posição for positiva, o percurso vai dessa posição até o fim da fila
// se a posição for negativa, o percurso vai dessa posição até o início
//   0 é a posição do primeiro dado (aquele que está na fila há mais tempo)
//   1 é a posição do segundo dado, etc
//   além disso,
//   -1 é a posição do último dado (o que está na fila há menos tempo)
//   -2 é a posição do penúltimo dado, etc
// cada dado do percurso será acessado por chamadas a f_próximo()
void f_inicia_percurso(Fila self, int pos_inicial){
    no *aux;
    if(pos_inicial >= 0){
        self->direcao = 1;
        aux = self->inicio;
        for(int i = 0; i < pos_inicial && aux != NULL; i++){
            aux = aux->prox;
        }
    } else {
        self->direcao = -1;
        aux = self->fim;
        for(int i = 0; i < -pos_inicial - 1 && aux != NULL; i++){
            aux = aux->ant;
        }        
    }
    self->proximo = aux;

}

// caso o percurso tenha terminado, retorna false
// senão, coloca o próximo dado do percurso em *pdado (se pdado não for NULL),
//   e retorna true
bool f_próximo(Fila self, void *pdado){
    if(self->proximo == NULL){
        return false;
    }
    if(pdado != NULL){
        memcpy(pdado, self->proximo->dado, self->tam_dado);
    }
    if(self->direcao == 1){
        self->proximo = self->proximo->prox;
    }
    if(self->direcao == -1){
        self->proximo = self->proximo->ant;
    }
    return true;
}