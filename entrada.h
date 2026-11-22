#ifndef ENTRADA_H
#define ENTRADA_H

#include "dados.h"

typedef struct {
    unsigned int saidaOriginal;
    unsigned int entradaOriginal;
    int alterado;
} ConfigConsole;

ConfigConsole configura_console_utf8(void);
void restaura_console(const ConfigConsole *config);

int ler_inteiro(const char *mensagem);
int ler_inteiro_faixa(const char *mensagem, int minimo, int maximo);
int ler_texto(const char *mensagem, char destino[], int tamanho);
int ler_texto_obrigatorio(const char *mensagem, char destino[], int tamanho);
int ler_dado(const char *mensagem, Dado *dado);
int ler_confirmacao(const char *mensagem);

#endif
