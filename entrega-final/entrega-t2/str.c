// includes, constantes e declarações
#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN_ALLOC 8

struct str
{
  byte *bytes;
  int qtd_bytes_usados;
  int capacidade_bytes;
  int nchars;
};

static void s_ok(Str_c s)
{
  assert(s != NULL);
  assert(s->bytes != NULL || (s->capacidade_bytes == 0 && s->qtd_bytes_usados == 0));
  assert(s->qtd_bytes_usados <= s->capacidade_bytes);
  assert(s->capacidade_bytes == 0 || s->capacidade_bytes >= MIN_ALLOC);
  assert(s->capacidade_bytes == 0 || s->capacidade_bytes <= s->qtd_bytes_usados * 3
        || s->capacidade_bytes == MIN_ALLOC);
  assert(s->capacidade_bytes == 0 || (s->capacidade_bytes & (s->capacidade_bytes - 1)) == 0);
  assert(s->nchars >= 0 && s->nchars <= s->qtd_bytes_usados);
  assert((s->nchars == 0) == (s->qtd_bytes_usados == 0));
}

static int pos_busca(Str_c s, int pos)
{
  if(pos < 0) {
    pos = s->nchars + 1 + pos;
  }
  return pos;
}

static int deslocamento_char(Str_c s, int pos)
{
  if(s->qtd_bytes_usados == 0) {
    return 0;
  }
  return u8_avanca_unichar(s->bytes, pos) - s->bytes;
}

// operações de criação e destruição

static void s_vazia(Str s)
{
  s->bytes = NULL;
  s->capacidade_bytes = 0;
  s->qtd_bytes_usados = 0;
  s->nchars = 0;
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
  if(strC == NULL) {
    return s;
  }
  int numbytes = strlen(strC);
  int nchars = u8_conta_unichar_nos_bytes(numbytes, (byte*) strC);
  if(nchars > 0) {
    s->qtd_bytes_usados = numbytes;
    s->nchars = nchars;
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

Str s_cria_de_arquivo(char *nome)
{
  Str s = s_cria("");
  FILE *arq = fopen(nome, "r");
  if(arq == NULL) {
    return s;
  }
  fseek(arq, 0, SEEK_END);
  long tam_arq = ftell(arq);
  fseek(arq, 0, SEEK_SET);
  if(tam_arq > 0) {
    byte *temp = malloc(tam_arq);
    assert(temp != NULL);
    size_t lidos = fread(temp, 1, tam_arq, arq);
    int nchars = u8_conta_unichar_nos_bytes(lidos, temp);
    if(lidos > 0 && nchars > 0) {
      s->qtd_bytes_usados = lidos;
      s->nchars = nchars;
      s->capacidade_bytes = calcula_alocacao(lidos);
      s->bytes = malloc(s->capacidade_bytes);
      assert(s->bytes != NULL);
      memcpy(s->bytes, temp, lidos);
    }
    free(temp);
  }
  fclose(arq);
  return s;
}

// operações de acesso

int s_tam(Str_c s)
{
  s_ok(s);
  return s->nchars;
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
  pos = pos_busca(s, pos);   // agora aceita pos negativo, como diz o str.h
  if(pos < 0 || pos >= s->nchars) {
    return UNI_INV;
  }
  int desloc = deslocamento_char(s, pos);
  unichar c;
  u8_unichar_nos_bytes(s->qtd_bytes_usados - desloc, s->bytes + desloc, &c);
  return c;
}

// operações de busca e comparação

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

static bool pertence(Str_c sb, unichar c)
{
  int i = 0;
  while(i < sb->qtd_bytes_usados) {
    unichar u;
    int n = u8_unichar_nos_bytes(sb->qtd_bytes_usados - i, sb->bytes + i, &u);
    if(u == c) {
      return true;
    }
    i += n;
  }
  return false;
}

static int busca_frente(Str_c s, int pos, Str_c sb, bool quer)
{
  pos = pos_busca(s, pos);
  if(pos < 0) {
    pos = 0;
  }
  if(pos >= s->nchars) {
    return -1;
  }
  int desloc = deslocamento_char(s, pos);
  for(int i = pos; i < s->nchars; i++) {
    unichar c;
    int n = u8_unichar_nos_bytes(s->qtd_bytes_usados - desloc, s->bytes + desloc, &c);
    if(pertence(sb, c) == quer) {
      return i;
    }
    desloc += n;
  }
  return -1;
}

static int busca_tras(Str_c s, int pos, Str_c sb, bool quer)
{
  pos = pos_busca(s, pos);
  if(pos > s->nchars) {
    pos = s->nchars;
  }
  if(pos <= 0) {
    return -1;
  }
  int desloc = deslocamento_char(s, pos);
  for(int i = pos - 1; i >= 0; i--) {
    desloc--;
    while((s->bytes[desloc] & 0xC0) == 0x80) {
      desloc--;
    }
    unichar c;
    u8_unichar_nos_bytes(s->qtd_bytes_usados - desloc, s->bytes + desloc, &c);
    if(pertence(sb, c) == quer) {
      return i;
    }
  }
  return -1;
}

int s_busca_c(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  return busca_frente(s, pos, sb, true);
}

int s_busca_nc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  return busca_frente(s, pos, sb, false);
}

int s_busca_rc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  return busca_tras(s, pos, sb, true);
}

int s_busca_rnc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  return busca_tras(s, pos, sb, false);
}

int s_busca_s(Str_c s, int pos, Str_c buscada)
{
  s_ok(s);
  s_ok(buscada);
  pos = pos_busca(s, pos);
  if(pos < 0) {
    pos = 0;
  }
  if(buscada->nchars == 0) {
    if(pos > s->nchars) {
      return s->nchars;
    } else {
      return pos;
    }
  }
  if(pos >= s->nchars) {
    return -1;
  }
  int desloc = deslocamento_char(s, pos);
  int m = buscada->qtd_bytes_usados;
  for(int i = pos; i < s->nchars; i++) {
    if(s->qtd_bytes_usados - desloc < m) {
      return -1;
    }
    if(memcmp(s->bytes + desloc, buscada->bytes, m) == 0) {
      return i;
    }
    desloc += u8_nbytes_no_unichar_que_comeca_com(s->bytes[desloc]);
  }
  return -1;
}

static void verifica_posicao_substitui (Str s, int *pos, int *tam)
{
  int tam_s = s_tam(s);
  int p = pos_busca(s, *pos);
  int fim;
  if(*tam < 0) {
    fim = tam_s;
  } else {
    fim = p + *tam;
  }
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

void s_substitui(Str s, int pos, int tam, Str_c sb)
{
  s_ok(s);
  if(sb != NULL) {
    s_ok(sb);
  }
  verifica_posicao_substitui(s, &pos, &tam);
  int ini = deslocamento_char(s, pos);
  int fim = deslocamento_char(s, pos + tam);
  int qtd_remover = fim - ini;
  int qtd_sb;
  int nchars_sb;
  if(sb == NULL) {
    qtd_sb = 0;
    nchars_sb = 0;
  } else {
    qtd_sb = sb->qtd_bytes_usados;
    nchars_sb = sb->nchars;
  }
  int qtd_antes = s->qtd_bytes_usados;
  int qtd_nova = qtd_antes - qtd_remover + qtd_sb;
  int nchars_novo = s->nchars - tam + nchars_sb;
  byte *copia_sb = NULL;
  if(qtd_sb > 0) {
    copia_sb = malloc(qtd_sb);
    assert(copia_sb != NULL);
    memcpy(copia_sb, sb->bytes, qtd_sb);
  }
  if(qtd_nova == 0) {
    free(s->bytes);
    s_vazia(s);
    free(copia_sb);
    return;
  }
  int cap_nova = calcula_alocacao(qtd_nova);
  int qtd_apos = qtd_antes - fim;
  if(cap_nova > s->capacidade_bytes) {
    byte *p = realloc(s->bytes, cap_nova);
    assert(p != NULL);
    s->bytes = p;
    s->capacidade_bytes = cap_nova;
  }
  memmove(s->bytes + ini + qtd_sb, s->bytes + fim, qtd_apos);
  if(qtd_sb > 0) {
    memcpy(s->bytes + ini, copia_sb, qtd_sb);
  }
  if(cap_nova < s->capacidade_bytes) {
    byte *p = realloc(s->bytes, cap_nova);
    assert(p != NULL);
    s->bytes = p;
    s->capacidade_bytes = cap_nova;
  }
  s->qtd_bytes_usados = qtd_nova;
  s->nchars = nchars_novo;
  free(copia_sb);
}

static void verifica_posicao_substring(Str_c sb, int *pos, int *tam)
{
  int tam_sb = s_tam(sb);
  int p = pos_busca(sb, *pos);
  int fim;
  if(*tam < 0) {
    fim = tam_sb;
  } else {
    fim = p + *tam;
  }
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
  int ini = deslocamento_char(sb, pos);
  int fim = deslocamento_char(sb, pos + tam);
  int qtd = fim - ini;
  byte *novos = NULL;
  int cap = 0;
  if(qtd > 0) {
    cap = calcula_alocacao(qtd);
    novos = malloc(cap);
    assert(novos != NULL);
    memcpy(novos, sb->bytes + ini, qtd);
  }
  free(s->bytes);
  s->bytes = novos;
  s->capacidade_bytes = cap;
  s->qtd_bytes_usados = qtd;
  s->nchars = tam;
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
  byte b[MIN_ALLOC];
  int n = u8_converte_pra_utf8(c, b);
  if(n < 0) {
    return;
  }
  struct str tmp;
  tmp.bytes = b;
  tmp.qtd_bytes_usados = n;
  tmp.capacidade_bytes = MIN_ALLOC;
  tmp.nchars = 1;
  s_insere(s, pos, &tmp);
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

// operações de E/S

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