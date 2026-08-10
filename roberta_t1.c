// programa para testar o terminal em modo "cru"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
  bool terminou_jogo;
  bool terminou_onda;
  bool terminou_partida;
  int pontos;
  int inimigos_inativos;
  int tiros;
  // ...
} estado_t;

void joga_onda(estado_t *est)
{
  //repete enquanto nao terminar a onda
  {
    processa_teclado(est);
    processa_tempo(est);
    apresenta(est);
  }
}

void joga_partida(estado_t *est)
{
  while (!est->terminou_partida) {
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
    char u = 0;
    int n = 0;
    do {
        char c = lechar();
        if (c != 0) {
            u = c;
            n = 0;
        }
        n++;
        printf("leituras: %d  ult ch: %d   \r", n, c);
    } while (u != 'q');

    estado_t estado;
    inicializa_tela();
    inicializa_estado(&estado);
    while (!estado.terminou_jogo) {
        joga_partida(&estado);
    }
    desinicializa_tela();

    normaliza_terminal();
}