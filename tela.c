#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include "tela.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#endif

#define DESCRITOR_ENTRADA 0
#define DESCRITOR_SAIDA 1
#define PASSO_ESPERA_MS 20
#define SEQUENCIA_LIMPAR "\033[2J\033[3J\033[H"

int tela_interativa(void) {
#ifdef _WIN32
    return _isatty(DESCRITOR_ENTRADA) && _isatty(DESCRITOR_SAIDA);
#else
    return isatty(DESCRITOR_ENTRADA) && isatty(DESCRITOR_SAIDA);
#endif
}

void tela_limpar(void) {
    if (!tela_interativa()) {
        return;
    }
    fflush(stdout);
#ifdef _WIN32
    system("cls");
#else
    fputs(SEQUENCIA_LIMPAR, stdout);
    fflush(stdout);
#endif
}

void tela_linha(void) {
    int i;

    for (i = 0; i < TELA_LARGURA; i++) {
        putchar('=');
    }
    putchar('\n');
}

void tela_abrir_pagina(const char *titulo) {
    tela_limpar();
    printf("\n");
    tela_linha();
    if (titulo != 0 && titulo[0] != '\0') {
        printf("  %s\n", titulo);
        tela_linha();
    }
}

void tela_exibir_aviso(char aviso[]) {
    if (aviso != 0 && aviso[0] != '\0') {
        printf(">> %s\n", aviso);
        tela_linha();
        aviso[0] = '\0';
    }
}

void tela_pausar(void) {
    int caractere;

    if (!tela_interativa()) {
        return;
    }
    printf("\nPressione Enter para continuar...");
    fflush(stdout);
    while ((caractere = getchar()) != '\n' && caractere != EOF) {
    }
}

#ifdef _WIN32
static int tecla_pressionada(void) {
    if (_kbhit()) {
        while (_kbhit()) {
            _getch();
        }
        return 1;
    }
    return 0;
}
#else
static int entrada_disponivel(int milissegundos) {
    fd_set leitura;
    struct timeval limite;

    FD_ZERO(&leitura);
    FD_SET(DESCRITOR_ENTRADA, &leitura);
    limite.tv_sec = milissegundos / 1000;
    limite.tv_usec = (milissegundos % 1000) * 1000;
    return select(DESCRITOR_ENTRADA + 1, &leitura, 0, 0, &limite) > 0;
}

static int espera_tecla(int milissegundos) {
    struct termios original;
    struct termios direto;
    char descartado;
    int pressionada;

    if (tcgetattr(DESCRITOR_ENTRADA, &original) != 0) {
        entrada_disponivel(milissegundos);
        return 0;
    }
    direto = original;
    direto.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    direto.c_cc[VMIN] = 0;
    direto.c_cc[VTIME] = 0;
    tcsetattr(DESCRITOR_ENTRADA, TCSANOW, &direto);
    pressionada = entrada_disponivel(milissegundos);
    if (pressionada) {
        while (entrada_disponivel(0) && read(DESCRITOR_ENTRADA, &descartado, 1) == 1) {
        }
    }
    tcsetattr(DESCRITOR_ENTRADA, TCSANOW, &original);
    return pressionada;
}
#endif

int tela_esperar(int milissegundos) {
    int decorrido = 0;

    if (!tela_interativa()) {
        return 0;
    }
    fflush(stdout);
#ifdef _WIN32
    while (decorrido < milissegundos) {
        if (tecla_pressionada()) {
            return 1;
        }
        Sleep(milissegundos - decorrido < PASSO_ESPERA_MS ? (DWORD)(milissegundos - decorrido) : PASSO_ESPERA_MS);
        decorrido += PASSO_ESPERA_MS;
    }
    return 0;
#else
    (void)decorrido;
    return espera_tecla(milissegundos);
#endif
}

void tela_digitar(const char *texto, int *pulado) {
    const unsigned char *p = (const unsigned char *)texto;
    int pular = pulado != 0 && *pulado;

    if (texto == 0) {
        return;
    }
    while (*p != '\0') {
        putchar(*p);
        p++;
        while ((*p & 0xC0) == 0x80) {
            putchar(*p);
            p++;
        }
        if (!pular && p[-1] != ' ' && tela_esperar(TELA_ATRASO_LETRA_MS)) {
            pular = 1;
        }
    }
    putchar('\n');
    fflush(stdout);
    if (pulado != 0) {
        *pulado = pular;
    }
}
