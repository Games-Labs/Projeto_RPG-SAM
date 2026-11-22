#ifndef TELA_H
#define TELA_H

#define TELA_LARGURA 62
#define TELA_ATRASO_LETRA_MS 22
#define TELA_ATRASO_LINHA_MS 1000
#define TELA_AVISO_TAMANHO 240

int tela_interativa(void);
void tela_limpar(void);
void tela_linha(void);
void tela_abrir_pagina(const char *titulo);
void tela_exibir_aviso(char aviso[]);
void tela_pausar(void);
int tela_esperar(int milissegundos);
void tela_digitar(const char *texto, int *pulado);

#endif
