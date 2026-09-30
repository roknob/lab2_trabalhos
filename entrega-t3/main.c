#include <stdio.h>
#include "calc.h"

int main(int argc, char *argv[])
{
  if(argc != 3){
    printf("uso: %s entrada.txt saida.txt\n", argv[0]);
    return 1;
  }
  Str conteudo = s_cria_de_arquivo(argv[1]);
  Str sep = s_cria("\n");
  Lista linhas = l_cria_separando(conteudo, sep);
  Lista saida = l_cria();
  for(int i = 0; i < l_tam(linhas); i++){
    Str linha = l_dado_pos(linhas, i);
    Str resultado = calculadora(linha);
    l_insere_fim(saida, resultado);
  }
  Str texto = s_cria_unindo(saida, sep);
  s_anexa(texto, sep);
  s_grava_arquivo(texto, argv[2]);
  s_destroi(texto);
  l_destroi(saida);
  l_destroi(linhas);
  s_destroi(sep);
  s_destroi(conteudo);
  return 0;
}