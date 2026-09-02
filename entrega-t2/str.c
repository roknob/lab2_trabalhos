// includes, constantes e declarações {{{1
#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN_ALLOC 8    // alocação mínima

struct str
{
  byte *bytes;
  int qtd_bytes_usados;
  int capacidade_bytes;
};

// A memória para conter os bytes de uma string deve ser alocada e/ou
//   realocada conforme a necessidade, cuidando para que a quantidade
//   de memória alocada seja sempre:
//   - nula (não alocada) se a string for vazia, ou
//   - não inferior ao necessário para armazenar os bytes da codificação utf8;
//   - não inferior à alocação mínima;
//   - não superior ao triplo do número de bytes necessários
//     (exceto quando for o mínimo);
//   - uma potência de 2.

// funções auxiliares {{{1

// verifica se a string cad está de acordo com a especificação
// aborta o programa se não tiver
static void s_ok(Str_c s)
{
  assert(s != NULL);
  assert(s->bytes != NULL || (s->capacidade_bytes == 0 && s->qtd_bytes_usados == 0));
  assert(s->qtd_bytes_usados <= s->capacidade_bytes);
  assert(s->capacidade_bytes == 0 || s->capacidade_bytes >= MIN_ALLOC);
  assert(s->capacidade_bytes == 0 || s->capacidade_bytes <= s->qtd_bytes_usados * 3
        || s->capacidade_bytes == MIN_ALLOC);
  assert(s->capacidade_bytes == 0 || (s->capacidade_bytes > 0
        && (s->capacidade_bytes & (s->capacidade_bytes - 1)) == 0));
}

// operações de criação e destruição {{{1

static void s_vazia(Str s)
{
  s->bytes = 0;
  s->capacidade_bytes = 0;
  s->qtd_bytes_usados = 0;
}

static int calcula_alocacao(int bytes_necessarios)
{
  int resultado_alocacao = MIN_ALLOC;
  while(resultado_alocacao < bytes_necessarios) {
    resultado_alocacao *= 2;
  }
  return resultado_alocacao;
} 

Str s_cria(char const *strC)
{
  Str s = malloc(sizeof(*s));
  assert(s != NULL);
  s_vazia(s);
  int numbytes;
  if(strC == NULL) {
    return s;
  } else {
    numbytes = strlen(strC);
  }
  if(u8_conta_unichar_nos_bytes(numbytes, (byte*) strC) != -1 && numbytes > 0) {
    s->qtd_bytes_usados = numbytes;
    s->capacidade_bytes = calcula_alocacao(numbytes);
    s->bytes = malloc(s->capacidade_bytes);
    assert(s->bytes != NULL);
    memcpy(s->bytes, strC, numbytes);
  }
  return s;
}

void s_destroi(Str s)
{
  s_ok(s);
  free(s->bytes);
  free(s);
}

Str s_cria_substring(Str_c s, int pos, int tam)
{
   Str nova = s_cria("");
   s_substring(nova, s, pos, tam);
   return nova;
}

Str s_cria_cópia(Str_c s)
{
   return s_cria_substring(s, 0, -1);
}

// Retorna uma nova string com o conteúdo do arquivo chamado nome.
// Retorna uma string vazia em caso de erro.
Str s_cria_de_arquivo(char *nome)
{
  Str s = s_cria("");
  FILE *arq = fopen(nome, "r");
  int tam_arq;
  if(arq == NULL) {
    return s;
  } else {
    fseek(arq, 0, SEEK_END);
    tam_arq = ftell(arq);
    fseek(arq, 0, SEEK_SET);
    byte *temp = malloc(tam_arq);
    fread(temp, 1, tam_arq, arq);
    if(u8_conta_unichar_nos_bytes(tam_arq, (byte*) temp) != -1 && tam_arq > 0) {
      s->qtd_bytes_usados = tam_arq;
      s->capacidade_bytes = calcula_alocacao(tam_arq);
      s->bytes = malloc(s->capacidade_bytes);
      assert(s->bytes != NULL);
      memcpy(s->bytes, temp, tam_arq);
    }
    free(temp);
    fclose(arq);
  }
  return s;
}

// operações de acesso {{{1

int s_tam(Str_c s)
{
  s_ok(s);
  int tam = u8_conta_unichar_nos_bytes(s->qtd_bytes_usados, s->bytes);
  assert(tam != -1);
  return tam;
}

char *s_strc(Str_c s)
{
  s_ok(s);
  char *copia = malloc(s->qtd_bytes_usados+1);
  assert(copia != NULL);
  if(s->qtd_bytes_usados > 0) {
    memcpy(copia, s->bytes, s->qtd_bytes_usados);
  }
  copia[s->qtd_bytes_usados] = '\0';
  return copia;
}

unichar s_ch(Str_c s, int pos)
{
  s_ok(s);
  byte *pos_s;
  if(pos >= 0 && pos < s_tam(s)) {
    pos_s = u8_avanca_unichar(s->bytes, pos);
    unichar cod_unicode;
    u8_unichar_nos_bytes(s->qtd_bytes_usados, pos_s, &cod_unicode);
    return cod_unicode;
  } else {
    return UNI_INV;
  }
}


// operações de busca e comparação {{{1

bool s_igual(Str_c s, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  if(s->qtd_bytes_usados == sb->qtd_bytes_usados &&
    (s->qtd_bytes_usados == 0 || memcmp(s->bytes, sb->bytes, s->qtd_bytes_usados) == 0)) {
    return true;
  } else {
    return false;
  }
}

static int pos_busca(Str_c s, int pos)
{
  if(pos < 0) {
    pos = s_tam(s) + 1 + pos;
  }
  return pos;
}

static bool char_igual_busca(Str_c sb, unichar c)
{
  for(int i = 0; i < s_tam(sb); i++) {
    if (s_ch(sb, i) == c) {
      return true;
    }
  }
  return false;
}

int s_busca_c(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  pos = pos_busca(s, pos);
  if (pos < 0){
    pos = 0;
  }
  for(int i = pos; i < s_tam(s); i++) {
    unichar c = s_ch(s, i);
    if (char_igual_busca(sb, c)) {
      return i;
    }
  }
  return -1;
}

int s_busca_nc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  pos = pos_busca(s, pos);
  if (pos < 0) {
    pos = 0;
  }
  for(int i = pos; i < s_tam(s); i++) {
    unichar c = s_ch(s, i);
    if (!char_igual_busca(sb, c)) {
      return i;
    }
  }
  return -1;
}

int s_busca_rc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  pos = pos_busca(s, pos);
  if(pos > s_tam(s)) {
    pos = s_tam(s);
  }
  for(int i = pos - 1; i >= 0; i--) {
    unichar c = s_ch(s, i);
    if(char_igual_busca(sb, c)) {
      return i;
    }
  }
  return -1;
}

int s_busca_rnc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  pos = pos_busca(s, pos);
  if (pos > s_tam(s)){
    pos = s_tam(s);
  }
  for(int i = pos - 1; i >= 0; i--) {
    unichar c = s_ch(s, i);
    if (!char_igual_busca(sb, c)) {
      return i;
    }
  }
  return -1;
}

static bool substring_igual_busca(Str_c s, int i, Str_c buscada)
{
  for(int j = 0; j < s_tam(buscada); j++) {
    if(s_ch(s, i+j) != s_ch(buscada, j)) {
      return false;
    }
  }
  return true;
}

int s_busca_s(Str_c s, int pos, Str_c buscada)
{
  s_ok(s);
  s_ok(buscada);
  pos = pos_busca(s, pos);
  if(pos < 0) {
    pos = 0;
  }
  if(s_tam(buscada) == 0) {
    return pos;
  } else {
    for(int i = pos; i < s_tam(s); i++) {
      if (substring_igual_busca(s, i, buscada)) {
        return i;
      }
    }
  }
  return -1;
}

static void verifica_posicao_substitui (Str s, int *pos, int *tam)
{
  int tam_s = s_tam(s);
  int p = pos_busca(s, *pos);
  int fim = (*tam < 0) ? tam_s : (p + *tam);
  if(p < 0) {
    p = 0;
  } else if(p > tam_s) {
    p = tam_s;
  }
  if(fim < 0) {
    fim = 0;
  } else if(fim > tam_s) {
    fim = tam_s;
  }
  if(fim < p) {
    fim = p;
  }
  *pos = p;
  *tam = fim - p;
}

static int qtd_bytes_substitui(Str_c sb)
{
  int qtd_bytes_sb;
  if(sb == NULL){
    qtd_bytes_sb = 0;
  } else {
    qtd_bytes_sb = sb->qtd_bytes_usados;
  }
  return qtd_bytes_sb;
}

static void atualiza_alocacao(Str s, int qtd_bytes_atualizada, int alocacao_atualizada)
{
  if(qtd_bytes_atualizada == 0) {
    free(s->bytes);
    s_vazia(s);
  } else if(qtd_bytes_atualizada > s->capacidade_bytes) {
    byte *tam_bytes_atualizada = realloc(s->bytes, alocacao_atualizada);
    assert(tam_bytes_atualizada != NULL);
    s->bytes = tam_bytes_atualizada;
    s->capacidade_bytes = alocacao_atualizada;
  }
}

void s_substitui(Str s, int pos, int tam, Str_c sb)
{
  s_ok(s);
  if (sb != NULL){
    s_ok(sb);
  }
  int qtd_bytes_pre_substitui = s->qtd_bytes_usados;
  verifica_posicao_substitui(s, &pos, &tam);
  byte *p_pos_inicio = u8_avanca_unichar(s->bytes, pos);
  int deslocamento_pos_i = p_pos_inicio - s->bytes;
  byte *p_pos_fim = u8_avanca_unichar(s->bytes, pos + tam);
  int deslocamento_pos_f = p_pos_fim - s->bytes;
  int qtd_bytes_remover = deslocamento_pos_f - deslocamento_pos_i;
  int qtd_bytes_sb = qtd_bytes_substitui(sb);
  int qtd_bytes_atualizada = qtd_bytes_pre_substitui - qtd_bytes_remover + qtd_bytes_sb;
  int alocacao_atualizada = calcula_alocacao(qtd_bytes_atualizada);
  atualiza_alocacao(s, qtd_bytes_atualizada, alocacao_atualizada);
  if(qtd_bytes_atualizada != 0) {
    int qtd_bytes_apos = qtd_bytes_pre_substitui - deslocamento_pos_f;
    int destino_apos = deslocamento_pos_i + qtd_bytes_sb;
    memmove(s->bytes + destino_apos, s->bytes + deslocamento_pos_f, qtd_bytes_apos);
    if(sb != NULL && qtd_bytes_sb > 0) {
      memcpy(s->bytes + deslocamento_pos_i, sb->bytes, qtd_bytes_sb);
    }
    s->qtd_bytes_usados = qtd_bytes_atualizada;
  }
}

static void verifica_posicao_substring(Str_c sb, int *pos, int *tam)
{
  int tam_sb = s_tam(sb);
  int p = pos_busca(sb, *pos);
  int fim = (*tam < 0) ? tam_sb : (p + *tam);
  if(p < 0) {
    p = 0;
  } else if(p > tam_sb) {
    p = tam_sb;
  }
  if(fim < 0) {
    fim = 0;
  } else if(fim > tam_sb) {
    fim = tam_sb;
  }
  if(fim < p) {
    fim = p;
  }
  *pos = p;
  *tam = fim - p;
} 

void s_substring(Str s, Str_c sb, int pos, int tam)
{
  s_ok(s);
  s_ok(sb);
  verifica_posicao_substring(sb, &pos, &tam);
  byte *p_pos_inicio = u8_avanca_unichar(sb->bytes, pos);
  int deslocamento_pos_i = p_pos_inicio - sb->bytes;
  byte *p_pos_fim = u8_avanca_unichar(sb->bytes, pos + tam);
  int deslocamento_pos_f = p_pos_fim - sb->bytes;
  int qtd_bytes_copiar = deslocamento_pos_f - deslocamento_pos_i;
  free(s->bytes);
  if(qtd_bytes_copiar == 0) {
    s_vazia(s);
  } else {
    s->capacidade_bytes = calcula_alocacao(qtd_bytes_copiar);
    s->bytes = (malloc(s->capacidade_bytes));
    assert(s->bytes != NULL);
    s->qtd_bytes_usados = qtd_bytes_copiar;
    memcpy(s->bytes, sb->bytes + deslocamento_pos_i, s->qtd_bytes_usados);
  }
}

void s_copia(Str s, Str_c sb)
{
  s_substring(s, sb, 0, -1);
}

void s_insere(Str s, int pos, Str_c sb)
{
  s_substitui(s, pos, 0, sb);
}

void s_insere_c(Str s, int pos, unichar c)
{
  s_ok(s);
  byte bytes_codificacao[5];
  int n = u8_converte_pra_utf8(c, bytes_codificacao);
  bytes_codificacao[n] = '\0';
  Str nova_insere = s_cria((char const *) bytes_codificacao);
  s_insere(s, pos, nova_insere);
  s_destroi(nova_insere);
}

void s_anexa(Str s, Str_c sb)
{
  s_substitui(s, -1, 0, sb);
}

void s_anexa_c(Str s, unichar c)
{
  s_insere_c(s, -1, c);
}

void s_remove(Str s, int pos, int tam)
{
  s_substitui(s, pos, tam, NULL);
}

void s_apara(Str s, Str_c sobras)
{
  s_ok(s);
  s_ok(sobras);
  int inicio_apara = s_busca_nc(s, 0, sobras);
  int fim_apara = s_busca_rnc(s, s_tam(s), sobras);
  if(inicio_apara == -1 && fim_apara == -1) {
    s_remove(s, 0, s_tam(s));
  } else {
    s_remove(s, fim_apara + 1, -1);
    s_remove(s, 0, inicio_apara);
  }
}

// operações de E/S {{{1

void s_imprime(Str_c s)
{
  s_ok(s);
  fwrite(s->bytes, 1, s->qtd_bytes_usados, stdout);
}

void s_grava_arquivo(Str_c s, char *nome)
{
  s_ok(s);
  FILE *arq = fopen(nome, "w");
  if(arq == NULL) {
    return;
  } else {
    fwrite(s->bytes, 1, s->qtd_bytes_usados, arq);
    fclose(arq);
  }
}

// vim: foldmethod=marker shiftwidth=2