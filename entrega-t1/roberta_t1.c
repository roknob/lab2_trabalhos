#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define posicoes_ataque_dia 13
#define posicoes_ataque_noite 8

//struct de um ataque
typedef struct {
    bool posicao_vazia;
    char ataque_ativo;
} ataque_t;

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

//funcao pra decidir o turno entre dia e noite
void sorteia_turno(estado_t *est){
    int chance_dia;
    if(est->fase_onda>=5){
        chance_dia = 20;
    } else {
        chance_dia = 100 - 20 *(est->fase_onda-1);
    }
    int resultado_turno = rand() % 100;
    if(resultado_turno<chance_dia){
        est->turno_dia=true;
    } else {
        est->turno_dia=false;
    }
}

//funcao que inicia cada onda
void inicializa_onda(estado_t *est){
    est->terminou_onda=false;
    est->pontos_onda=0;
    est->arma_atual='0';
    sorteia_turno(est);
    if(est->turno_dia==true){
        inicializa_onda_dia(est);
    } else {
        inicializa_onda_noite(est);
    }
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

//finaliza o jogo
void termina_jogo(estado_t *est){
    est->terminou_jogo=true;
    est->terminou_partida=true;
    est->terminou_onda=true;
}

//ve qual a arma ta e aletera a arma pra proxima na sequencia
void altera_arma(estado_t *est){
    char armas_dia[]="0123456789N";
    char armas_noite[]="02468N";
    char *turno_armas;
    int qtd_turno_armas;
    if (est->turno_dia==true){
        turno_armas=armas_dia;
        qtd_turno_armas=strlen(armas_dia);
    } else {
        turno_armas=armas_noite;
        qtd_turno_armas=strlen(armas_noite);
    }
    int posicao_atual_arma;
    for(int i=0; i<qtd_turno_armas; i++){
        if (turno_armas[i]==est->arma_atual){
            posicao_atual_arma=i;
        }
    }
    int proxima_posicao_arma=posicao_atual_arma+1;
    if(proxima_posicao_arma==qtd_turno_armas){
        proxima_posicao_arma=0;
    }
    est->arma_atual=turno_armas[proxima_posicao_arma];
}

//funcao que sorteia os ataques
char sorteia_ataque (estado_t *est){
    char armas_dia[]="0123456789N";
    char armas_noite[]="02468N";
    char *turno_armas;
    int qtd_turno_armas;
    if (est->turno_dia==true){
        turno_armas=armas_dia;
        qtd_turno_armas=strlen(armas_dia);
    } else {
        turno_armas=armas_noite;
        qtd_turno_armas=strlen(armas_noite);
    }
    int indice_sorteio_arma = rand() % qtd_turno_armas;
    char resultado_sorteio_arma=turno_armas[indice_sorteio_arma];
    return resultado_sorteio_arma;
}

//funcao que gera os inimigos
void gera_inimigos(estado_t *est){
    int posicao_turno;
    if(est->turno_dia==true){
        posicao_turno=posicoes_ataque_dia-1;
    } else {
        posicao_turno=posicoes_ataque_noite-1;
    }
    est->lista_ataques[posicao_turno].ataque_ativo=sorteia_ataque(est);
    est->lista_ataques[posicao_turno].posicao_vazia=false;
    est->inimigos_ativos++;
    est->inimigos_inativos--;
}

//funcao de verificar o inimigo
int verifica_inimigo(estado_t *est){
    int indice_alvo = -1;
    if(est->tiros!=0){
        for(int i=0; i<posicoes_ataque_dia; i++){
            if((!est->lista_ataques[i].posicao_vazia && est->lista_ataques[i].ataque_ativo==est->arma_atual)
                || (!est->lista_ataques[i].posicao_vazia && est->arma_atual=='n'&& est->lista_ataques[i].ataque_ativo=='N')){
                indice_alvo=i;
                break;
            }
        }
        est->tiros--;
    }
    return indice_alvo;
}

void calcula_pontos_tiros(estado_t *est, int indice_alvo){
    if(est->turno_dia==true){
        if(est->lista_ataques[indice_alvo].ataque_ativo=='n'){
            est->pontos_onda+=(posicoes_ataque_dia-indice_alvo)*2;
        } else {
            est->pontos_onda+=posicoes_ataque_dia-indice_alvo;
        }
    } else {
        if(est->lista_ataques[indice_alvo].ataque_ativo=='n'){
            est->pontos_onda+=(posicoes_ataque_noite-indice_alvo)*4;
        } else {
            est->pontos_onda+=(posicoes_ataque_noite-indice_alvo)*2;
        }
    }
}

void destroi_inimigo(estado_t *est, int indice_alvo){
    if(indice_alvo!=-1){
        if(est->lista_ataques[indice_alvo].ataque_ativo==est->arma_atual){
            est->lista_ataques[indice_alvo].posicao_vazia=true;
            calcula_pontos_tiros(est, indice_alvo);
            est->lista_ataques[indice_alvo].ataque_ativo='\0';
        } else {
            if(est->arma_atual=='n'&& est->lista_ataques[indice_alvo].ataque_ativo=='N'){
            est->lista_ataques[indice_alvo].ataque_ativo='n';
            }
        }
    }
}

void atira(estado_t *est){
    int indice_alvo = verifica_inimigo(est);
    destroi_inimigo(est, indice_alvo);
}

//chama a funcao conforme a tecla que foi pressionada
void processa_teclado(estado_t *est){
    char tec = lechar();
    switch (tec)
    {
    case '\033':
        termina_jogo(est);
        break;
    case '\t':
        altera_arma(est);
        break;
    case '\n':
        atira(est);
        break;
    case ' ':
        atuacao_sonar(est);
        break;
    default:
        break;
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