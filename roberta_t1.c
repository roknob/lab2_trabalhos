#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

//struct de um ataque
typedef struct {
    bool posicao_vazia;
    char ataque_ativo;
} ataque_t;

#define posicoes_ataque_dia 13
#define posicoes_ataque_noite 8

//struct de estado com tudo que sera preciso para o jogo
typedef struct {
    bool terminou_jogo;
    bool terminou_onda;
    bool terminou_partida;
    bool turno_dia;
    int fase_onda;
    int pontos_onda;
    int pontos_totais;
    int tiros;
    int escudos;
    char arma_atual;
    int inimigos_ativos;
    int inimigos_inativos;
    int inimigos_derrotados;
    ataque_t lista_ataques[posicoes_ataque_dia];
} estado_t;

//funcao que inicia o jogo
void inicializa_estado(estado_t *est){
    est->terminou_jogo=false;
    est->terminou_onda=false;
    est->terminou_partida=false;
    est->turno_dia=true;
    est->fase_onda=1;
    est->pontos_onda=0;
    est->pontos_totais=0;
    est->tiros=30;
    est->escudos=3;
    est->arma_atual='0';
    est->inimigos_ativos=0;
    est->inimigos_inativos=20;
    est->inimigos_derrotados=0;
    for(int i=0; i<posicoes_ataque_dia; i++){
        est->lista_ataques[i].posicao_vazia=true;
        est->lista_ataques[i].ataque_ativo='\0';
    }
}

//funcao que inicia onda de dia
void inicializa_onda_dia(estado_t *est){
    est->tiros=30;
    est->escudos=3;
    est->arma_atual='0';
    est->inimigos_ativos=0;
    est->inimigos_inativos=20;
    est->inimigos_derrotados=0;
    for(int i=0; i<posicoes_ataque_dia; i++){
        est->lista_ataques[i].posicao_vazia=true;
        est->lista_ataques[i].ataque_ativo='\0';
    }
}

//funcao que inicia onda de noite
void inicializa_onda_noite(estado_t *est){
    est->tiros=30;
    est->escudos=3;
    est->arma_atual='0';
    est->inimigos_ativos=0;
    est->inimigos_inativos=15;
    est->inimigos_derrotados=0;
    for(int i=0; i<posicoes_ataque_dia; i++){
        est->lista_ataques[i].posicao_vazia=true;
        est->lista_ataques[i].ataque_ativo='\0';
    }
}

//funcao que inicia cada onda
void inicializa_onda(estado_t *est){
    est->terminou_onda=false;
    est->pontos_onda=0;
    est->arma_atual='0';
    if(est->turno_dia==true){
        inicializa_onda_dia(est);
    } else {
        inicializa_onda_noite(est);
    }
}

void joga_onda(estado_t *est)
{
  //repete enquanto nao terminar a onda
  {
    processa_teclado(est);
    processa_tempo(est);
    apresenta(est);
  }
  est->fase_onda++;
}

void joga_partida(estado_t *est)
{
  while (!est->terminou_partida) {
    inicializa_onda(est);
    joga_onda(est);
  }
}

// configura o terminal para o modo "cru", para permitir a leitura
//   de cada caractere digitado sem esperar pelo "enter".
void configura_terminal()
{
    if (system("stty raw opost -echo min 0 time 1") != 0) {
        perror("erro na execução de system(\"stty\")");
        fprintf(stderr, "você tem o programa stty instalado?\n");
        exit(1);
    };
    if (setvbuf(stdin, NULL, _IONBF, 0) != 0) {
        perror("erro na execução de setvbuf()");
        exit(1);
    }
}

// configura o terminal para o modo normal, com bufferização por linha.
void normaliza_terminal()
{
    system("stty sane");
}

// lê um caractere do teclado.
// retorna o código do caractere lido ou 0 casa nada tenha sido digitado.
// só funciona corretamente se o terminal estiver em modo "cru".
char lechar()
{
    fflush(stdout);
    char c;
    if (fread(&c, 1, 1, stdin) == 1) return c;
    return 0;
}

int main()
{
    configura_terminal();
    srand(time(NULL));
    estado_t estado;
    inicializa_tela();
    inicializa_estado(&estado);
    while (!estado.terminou_jogo) {
        joga_partida(&estado);
    }
    desinicializa_tela();

    normaliza_terminal();
}