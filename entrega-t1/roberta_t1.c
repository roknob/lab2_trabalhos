#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define posicoes_ataque_dia 13
#define posicoes_ataque_noite 8

// implementação de um cronômetro
typedef struct timespec crono;

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
    crono cronometro;
    double tempo_movimento;
    int recordes[3];
} estado_t;

//funcao que inicia a tela do jogo com o titulo
void inicializa_tela() {
    printf("invasores do espaço\n");
}

//funcao que iniciacializa o struct do estado atribuindo valores
void inicializa_estado(estado_t *est) {
    est->terminou_jogo = false;
    est->terminou_onda = false;
    est->terminou_partida = false;
    est->turno_dia = true;
    est->fase_onda = 1;
    est->pontos_onda = 0;
    est->pontos_totais = 0;
    est->tiros = 30;
    est->escudos = 3;
    est->arma_atual = '0';
    est->inimigos_ativos = 0;
    est->inimigos_inativos = 20;
    est->inimigos_derrotados = 0;
    for (int i = 0; i < posicoes_ataque_dia; i++) {
        est->lista_ataques[i].posicao_vazia = true;
        est->lista_ataques[i].ataque_ativo = '\0';
    }
    est->tempo_movimento = 2.0;
}

// inicializa um cronômetro com a hora atual
void crono_inicia(crono *c) {
    clock_gettime(CLOCK_MONOTONIC, c);
}

// retorna o tempo passado desde que o cronômetro *c foi iniciado, em segundos
double crono_parcial(crono *c) {
    crono agora;
    clock_gettime(CLOCK_MONOTONIC, &agora);

    double segundos = agora.tv_sec - c->tv_sec;
    double nanosegundos = agora.tv_nsec - c->tv_nsec;
    return segundos + 1e-9 * nanosegundos;
}

//funcao que inicia onda de dia
void inicializa_onda_dia(estado_t *est) {
    est->tiros = 30;
    est->escudos = 3;
    est->arma_atual = '0';
    est->inimigos_ativos = 0;
    est->inimigos_inativos = 20;
    est->inimigos_derrotados = 0;
    for (int i = 0; i < posicoes_ataque_dia; i++) {
        est->lista_ataques[i].posicao_vazia = true;
        est->lista_ataques[i].ataque_ativo = '\0';
    }
}

//funcao que inicia onda de noite
void inicializa_onda_noite(estado_t *est) {
    est->tiros = 30;
    est->escudos = 3;
    est->arma_atual = '0';
    est->inimigos_ativos = 0;
    est->inimigos_inativos = 15;
    est->inimigos_derrotados = 0;
    for (int i = 0; i < posicoes_ataque_dia; i++) {
        est->lista_ataques[i].posicao_vazia = true;
        est->lista_ataques[i].ataque_ativo = '\0';
    }
}

//funcao para calcular o tempo de movimentacao para cada onda;
void tempo_movimentacao(estado_t *est) {
    est->tempo_movimento = 2.0;
    for (int i = 1; i < est->fase_onda; i++) {
        est->tempo_movimento = est->tempo_movimento * 0.9;
    }
    if (est->turno_dia == false) {
        est->tempo_movimento = est->tempo_movimento * 3;
    }
}

//funcao pra decidir o turno entre dia e noite
void sorteia_turno(estado_t *est) {
    int chance_dia;
    if (est->fase_onda >= 5) {
        chance_dia = 20;
    } else {
        chance_dia = 100 - 20 * (est->fase_onda - 1);
    }
    int resultado_turno = rand() % 100;
    if (resultado_turno < chance_dia) {
        est->turno_dia = true;
    } else {
        est->turno_dia = false;
    }
}

//funcao que inicia cada onda
void inicializa_onda(estado_t *est){
    est->terminou_onda = false;
    est->pontos_onda = 0;
    est->arma_atual = '0';
    crono_inicia(&est->cronometro);
    sorteia_turno(est);
    tempo_movimentacao(est);
    if (est->turno_dia == true) {
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
void termina_jogo(estado_t *est) {
    est->terminou_jogo = true;
    est->terminou_partida = true;
    est->terminou_onda = true;
}

//toca uma sequência de sons pra marcar o fim de uma onda
void toca_som_fim_onda() {
    system("aplay -q Sons/12.3.wav Sons/11.3.wav &");
}

//toca uma sequência de sons pra marcar o fim da partida
void toca_som_fim_partida() {
    system("aplay -q Sons/x.3.wav Sons/11.3.wav Sons/x.3.wav &");
}

//monta o nome do arquivo de som para o ataque na posição i, e coloca em pedaco
void monta_som_ataque(estado_t *est, int i, char *pedaco) {
    if (est->lista_ataques[i].posicao_vazia) {
        sprintf(pedaco, "Sons/x.3.wav ");
    } else if (est->lista_ataques[i].ataque_ativo == 'n'
                || est->lista_ataques[i].ataque_ativo == 'N') {
        sprintf(pedaco, "Sons/11.3.wav ");
    } else {
        sprintf(pedaco, "Sons/%c.3.wav ", est->lista_ataques[i].ataque_ativo);
    }
}

//toca som conforme o tipo
void toca_som(char tipo) {
    char comando[50];
    switch (tipo) {
        case 'n':
        case 'N':
            sprintf(comando, "aplay -q Sons/11.3.wav &");
            break;
        case ' ':
            sprintf(comando, "aplay -q Sons/x.3.wav &");
            break;
        default:
            sprintf(comando, "aplay -q Sons/%c.3.wav &", tipo);
            break;
    }
    system(comando);
}

//funcao que concatena os nomes dos sons para tocar
void atuacao_sonar(estado_t *est) {
    char comando[300];
    strcpy(comando, "aplay -q ");
    char pedaco[20];
    for (int i = 0; i < 3; i++) {
        if (i < est->escudos) {
            sprintf(pedaco, "Sons/12.3.wav ");
        } else {
            monta_som_ataque(est, i, pedaco);
        }
        strcat(comando, pedaco);
    }
    for (int j = 3; j < posicoes_ataque_dia; j++) {
        monta_som_ataque(est, j, pedaco);
        strcat(comando, pedaco);
    }
    strcat(comando, "&");
    system(comando);
}

//ve qual a arma ta e altera a arma pra proxima na sequencia
void altera_arma(estado_t *est) {
    char armas_dia[] = "0123456789n";
    char armas_noite[] = "02468n";
    char *turno_armas;
    int qtd_turno_armas;
    if (est->turno_dia == true) {
        turno_armas = armas_dia;
        qtd_turno_armas = strlen(armas_dia);
    } else {
        turno_armas = armas_noite;
        qtd_turno_armas = strlen(armas_noite);
    }
    int posicao_atual_arma;
    for (int i = 0; i < qtd_turno_armas; i++) {
        if (turno_armas[i] == est->arma_atual) {
            posicao_atual_arma = i;
        }
    }
    int proxima_posicao_arma = posicao_atual_arma + 1;
    if (proxima_posicao_arma == qtd_turno_armas) {
        proxima_posicao_arma = 0;
    }
    est->arma_atual = turno_armas[proxima_posicao_arma];
    toca_som(est->arma_atual);
}

//funcao que sorteia os ataques inimigos
char sorteia_ataque(estado_t *est) {
    char armas_dia[] = "0123456789N";
    char armas_noite[] = "02468N";
    char *turno_armas;
    int qtd_turno_armas;
    if (est->turno_dia == true) {
        turno_armas = armas_dia;
        qtd_turno_armas = strlen(armas_dia);
    } else {
        turno_armas = armas_noite;
        qtd_turno_armas = strlen(armas_noite);
    }
    int indice_sorteio_arma = rand() % qtd_turno_armas;
    char resultado_sorteio_arma = turno_armas[indice_sorteio_arma];
    return resultado_sorteio_arma;
}

//funcao que gera os inimigos
void gera_inimigos(estado_t *est){
    int posicao_turno;
    if (est->turno_dia == true) {
        posicao_turno = posicoes_ataque_dia - 1;
    } else {
        posicao_turno = posicoes_ataque_noite - 1;
    }
    char tipo_sorteado = sorteia_ataque(est);
    est->lista_ataques[posicao_turno].ataque_ativo = tipo_sorteado;
    est->lista_ataques[posicao_turno].posicao_vazia = false;
    est->inimigos_ativos++;
    est->inimigos_inativos--;
    toca_som(tipo_sorteado);
}

//funcao de verificar o inimigo e sua posicao
int verifica_inimigo(estado_t *est) {
    int indice_alvo = -1;
    if (est->tiros != 0) {
        for (int i = 0; i < posicoes_ataque_dia; i++) {
            if ((!est->lista_ataques[i].posicao_vazia
                && est->lista_ataques[i].ataque_ativo == est->arma_atual)
                || (!est->lista_ataques[i].posicao_vazia && est->arma_atual == 'n'
                && est->lista_ataques[i].ataque_ativo == 'N')) {
                indice_alvo = i;
                break;
            }
        }
        est->tiros--;
    }
    return indice_alvo;
}

//calcula os pontos conforme os tiros que atingem os inimigos
void calcula_pontos_tiros(estado_t *est, int indice_alvo) {
    if (est->turno_dia == true) {
        if (est->lista_ataques[indice_alvo].ataque_ativo == 'n') {
            est->pontos_onda += (posicoes_ataque_dia - indice_alvo) * 2;
        } else {
            est->pontos_onda += posicoes_ataque_dia - indice_alvo;
        }
    } else {
        if (est->lista_ataques[indice_alvo].ataque_ativo == 'n') {
            est->pontos_onda += (posicoes_ataque_noite - indice_alvo) * 4;
        } else {
            est->pontos_onda += (posicoes_ataque_noite - indice_alvo) * 2;
        }
    }
}

//destroi os inimigps quando a arma acerta o tiro
bool destroi_inimigo(estado_t *est, int indice_alvo) {
    bool acertou = false;
    if (indice_alvo != -1) {
        if (est->lista_ataques[indice_alvo].ataque_ativo == est->arma_atual) {
            est->lista_ataques[indice_alvo].posicao_vazia = true;
            calcula_pontos_tiros(est, indice_alvo);
            est->lista_ataques[indice_alvo].ataque_ativo = '\0';
            est->inimigos_ativos--;
            acertou = true;
        } else {
            if (est->arma_atual == 'n'
                && est->lista_ataques[indice_alvo].ataque_ativo == 'N') {
                est->lista_ataques[indice_alvo].ataque_ativo = 'n';
                acertou = true;
            }
        }
    }
    return acertou;
}

//atira nos inimigos
void atira(estado_t *est) {
    if (est->tiros == 0) {
        return;
    }
    int indice_alvo = verifica_inimigo(est);
    bool acertou = destroi_inimigo(est, indice_alvo);
    if (acertou) {
        toca_som(est->arma_atual);
    } else {
        toca_som(' ');
    }
}

//chama a funcao conforme a tecla que foi pressionada
void processa_teclado(estado_t *est) {
    char tec = lechar();
    switch (tec) {
        case '\033':
            termina_jogo(est);
            break;
        case '\t':
            altera_arma(est);
            break;
        case '\r':
            atira(est);
            break;
        case ' ':
            atuacao_sonar(est);
            break;
        default:
            break;
    }
}

//funcao que move o inimigo
//se houver algum ataque inimigo ativo, ele é movido para a esquerda, e
//caso um ataque colida com um escudo, tanto o escudo quando o ataque são destruídos
//caso um ataque colida com a base (a arma do jogador), o jogo termina
void move_inimigo(estado_t *est) {
    if (est->inimigos_ativos >= 1) {
        for (int i = 0; i < posicoes_ataque_dia; i++) {
            if (!est->lista_ataques[i].posicao_vazia) {
                if (i == 0) {
                    termina_jogo(est);
                    break;
                } else if (i - 1 < est->escudos) {
                    est->inimigos_ativos--;
                    est->escudos--;
                    est->lista_ataques[i].posicao_vazia = true;
                } else {
                    est->lista_ataques[i - 1].posicao_vazia = est->lista_ataques[i].posicao_vazia;
                    est->lista_ataques[i].posicao_vazia = true;
                    est->lista_ataques[i - 1].ataque_ativo = est->lista_ataques[i].ataque_ativo;
                }
            }
        }
    }        
}

//se ainda houver algum ataque não ativo na onda atual,
//é colocado na última posição dos ataques (mais à direita)
//se não houver mais ataque, ativo ou não, na onda atual, a onda termina
void verifica_ataque_ativo(estado_t *est) {
    if (est->inimigos_inativos > 0) {
        gera_inimigos(est);
    } else if (est->inimigos_ativos == 0) {
        est->terminou_onda = true;
    }
}

//funcao que processa o tempo decorrido
void processa_tempo(estado_t *est) {
    if (crono_parcial(&est->cronometro) >= est->tempo_movimento) {
        crono_inicia(&est->cronometro);
        move_inimigo(est);
        if (!est->terminou_jogo && !est->terminou_onda) {
            verifica_ataque_ativo(est);
        }        
    }
}

//funcao que apresenta o jogo na tela quando e noite
void apresenta_noite(estado_t *est) {
    printf("\033[2K\r");
    printf("\r %d ", est->pontos_onda);
}

//funcao que apresenta o jogo na tela quando e dia
void apresenta_dia(estado_t *est) {
    printf("\033[2K\r");
    printf(" %d ", est->pontos_onda);
    printf("%2d ", est->tiros);
    printf("%c", est->arma_atual);
    for (int i = 0; i < 3; i++) {
        if (i < est->escudos) {
            printf(")");
        } else if (est->lista_ataques[i].posicao_vazia == false) {
            printf("%c", est->lista_ataques[i].ataque_ativo);
        } else {
            printf(" ");
        }
    }
    for (int j = 3; j < posicoes_ataque_dia; j++) {
        if (est->lista_ataques[j].posicao_vazia == true) {
            printf(" ");
        } else {
            printf("%c", est->lista_ataques[j].ataque_ativo);
        }
    }
    printf("\r");
}

//funcao que apresenta na tela o jogo conforme o turno
void apresenta(estado_t *est) {
    if (est->turno_dia == false) {
        apresenta_noite(est);
    } else {
        apresenta_dia(est);
    }
}

//finaliza a onda atual
void finaliza_onda(estado_t *est){
    if (est->terminou_onda == true) {
        if (est->turno_dia == true) {
            est->pontos_onda += (est->tiros * 2) + (est->escudos * 10);
        } else {
            est->pontos_onda += ((est->tiros * 2) + (est->escudos * 10)) * 2;
        }
    }
    est->pontos_totais = est->pontos_totais + est->pontos_onda;
    if (est->terminou_jogo == false) {
        toca_som_fim_onda();
        printf("\ronda finalizada. sua pontuação é de %d.\n", est->pontos_totais);
        printf("clique r para começar nova onda");
        while (lechar() != 'r') {
            ;
        }
    }
}

//joga uma nova onda de inimigos
void joga_onda(estado_t *est)
{
    while (est->terminou_onda == false) {
        processa_teclado(est);
        processa_tempo(est);
        apresenta(est);
    }
    est->fase_onda++;
}

//continua jogo e chama uma nova onda de inimigos
void joga_partida(estado_t *est)
{
    while (!est->terminou_partida) {
        inicializa_onda(est);
        joga_onda(est);
        finaliza_onda(est);
    }
}

//atualiza os recordes do jogo no arquivo caso tenha um novo
void atualiza_recordes(estado_t *est) {
    int pontuacao_atual = est->pontos_totais;
    int posicao = -1;
    for (int i = 0; i < 3; i++) {
        if (pontuacao_atual > est->recordes[i]) {
            posicao = i;
            break;
        }
    }
    if (posicao != -1) {
        for (int j = 2; j > posicao; j--) {
            est->recordes[j] = est->recordes[j - 1];
        }
        est->recordes[posicao] = pontuacao_atual;
    }
}

//salva os recordes do jogo no arquivo
void salva_recordes(estado_t *est) {
    FILE *arq = fopen("recordes.txt", "w");
    if (arq == NULL) {
        return;
    }
    for (int i = 0; i < 3; i++) {
        fprintf(arq, "%d\n", est->recordes[i]);
    }
    fclose(arq);
}

//carrega os recordes atuais nos arquivos
void carrega_recordes(estado_t *est) {
    FILE *arq = fopen("recordes.txt", "r");
    if (arq == NULL) {
        for (int i = 0; i < 3; i++) {
            est->recordes[i] = 0;
        }
    } else {
        for (int i = 0; i < 3; i++) {
            if (fscanf(arq, "%d", &est->recordes[i]) != 1) {
                break;
            }
        }
        fclose(arq);
    }
}

//finaliza o jogo caso a pessoa nao deseja jogar novamente
void finaliza_partida(estado_t *est) {
    atualiza_recordes(est);
    salva_recordes(est);
    toca_som_fim_partida();
    printf("a sua pontuação foi: %d\nos recordes são: %d %d %d\n", 
            est->pontos_totais, est->recordes[0], est->recordes[1], est->recordes[2]);
    printf("deseja jogar novamente?\n[s para jogar | n para nao] \n");
    char resposta;
    do {
        while ((resposta = lechar()) == 0) {
            ;
        }
    } while (resposta != 's' && resposta != 'n');
    if (resposta == 's') {
        inicializa_estado(est);
    } else {
        est->terminou_jogo = true;
    }
}

// configura o terminal para o modo "cru", para permitir a leitura
// de cada caractere digitado sem esperar pelo "enter".
void configura_terminal() {
    if (system("stty raw opost -echo min 0 time 1") != 0) {
        perror("erro na execução de system(\"stty\")");
        fprintf(stderr, "você tem o programa stty instalado?\n");
        exit(1);
    }
    if (setvbuf(stdin, NULL, _IONBF, 0) != 0) {
        perror("erro na execução de setvbuf()");
        exit(1);
    }
}

//tela que aparece pos o jogo finalizar
void desinicializa_tela(estado_t *est) {
    printf("a sua pontuação foi: %d\nos recordes são: %d %d %d\n", 
            est->pontos_totais, est->recordes[0], est->recordes[1], est->recordes[2]);
}

// configura o terminal para o modo normal, com bufferização por linha.
void normaliza_terminal() {
    system("stty sane");
}

int main() {
    configura_terminal();
    srand(time(NULL));
    estado_t estado;
    inicializa_tela();
    inicializa_estado(&estado);
    carrega_recordes(&estado);
    while (!estado.terminou_jogo) {
        joga_partida(&estado);
        finaliza_partida(&estado);
    }
    desinicializa_tela(&estado);

    normaliza_terminal();
}