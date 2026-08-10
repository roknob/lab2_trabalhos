// programa para testar o terminal em modo "cru"

#include <stdio.h>
#include <stdlib.h>

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
    normaliza_terminal();
}