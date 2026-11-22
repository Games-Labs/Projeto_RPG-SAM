#include "entrada.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define LINHA_TAMANHO 128

typedef enum {
    LINHA_OK,
    LINHA_LONGA,
    LINHA_FIM
} StatusLinha;

ConfigConsole configura_console_utf8(void) {
    ConfigConsole config;

    config.saidaOriginal = 0;
    config.entradaOriginal = 0;
    config.alterado = 0;

#ifdef _WIN32
    config.saidaOriginal = GetConsoleOutputCP();
    config.entradaOriginal = GetConsoleCP();
    if (config.saidaOriginal != 0 && config.entradaOriginal != 0
        && SetConsoleOutputCP(CP_UTF8) && SetConsoleCP(CP_UTF8)) {
        config.alterado = 1;
    }
#endif

    return config;
}

void restaura_console(const ConfigConsole *config) {
#ifdef _WIN32
    if (config != 0 && config->alterado) {
        SetConsoleOutputCP(config->saidaOriginal);
        SetConsoleCP(config->entradaOriginal);
    }
#else
    (void)config;
#endif
}

static StatusLinha ler_linha(char destino[], int tamanho) {
    size_t lido;
    int caractere;
    StatusLinha status = LINHA_OK;

    if (fgets(destino, tamanho, stdin) == 0) {
        destino[0] = '\0';
        return LINHA_FIM;
    }

    lido = strlen(destino);
    if (lido > 0 && destino[lido - 1] == '\n') {
        destino[lido - 1] = '\0';
        lido--;
    } else if (!feof(stdin)) {
        status = LINHA_LONGA;
        while ((caractere = getchar()) != '\n' && caractere != EOF) {
        }
    }
    if (lido > 0 && destino[lido - 1] == '\r') {
        destino[lido - 1] = '\0';
    }
    return status;
}

static char *aparar(char *texto) {
    char *fim;

    while (*texto == ' ' || *texto == '\t') {
        texto++;
    }
    fim = texto + strlen(texto);
    while (fim > texto && (fim[-1] == ' ' || fim[-1] == '\t')) {
        fim--;
    }
    *fim = '\0';
    return texto;
}

static int ler_inteiro_status(const char *mensagem, int *valor) {
    char linha[LINHA_TAMANHO];
    char *texto;
    char *fim;
    long numero;
    StatusLinha status;

    for (;;) {
        printf("%s", mensagem);
        status = ler_linha(linha, sizeof(linha));
        if (status == LINHA_FIM) {
            printf("\nEntrada encerrada.\n");
            return 0;
        }
        texto = aparar(linha);
        errno = 0;
        numero = strtol(texto, &fim, 10);
        if (status == LINHA_LONGA || texto[0] == '\0' || *fim != '\0' || errno != 0
            || numero < -2147483647L || numero > 2147483647L) {
            printf("Entrada invalida. Digite um numero inteiro.\n");
            continue;
        }
        *valor = (int)numero;
        return 1;
    }
}

int ler_inteiro(const char *mensagem) {
    int valor = 0;

    ler_inteiro_status(mensagem, &valor);
    return valor;
}

int ler_inteiro_faixa(const char *mensagem, int minimo, int maximo) {
    int valor;

    for (;;) {
        if (!ler_inteiro_status(mensagem, &valor)) {
            return minimo - 1;
        }
        if (valor >= minimo && valor <= maximo) {
            return valor;
        }
        printf("Valor fora da faixa permitida (%d a %d).\n", minimo, maximo);
    }
}

int ler_texto(const char *mensagem, char destino[], int tamanho) {
    StatusLinha status;

    printf("%s", mensagem);
    status = ler_linha(destino, tamanho);
    return status == LINHA_OK;
}

int ler_texto_obrigatorio(const char *mensagem, char destino[], int tamanho) {
    char linha[LINHA_TAMANHO];
    char *texto;
    StatusLinha status;

    for (;;) {
        printf("%s", mensagem);
        status = ler_linha(linha, sizeof(linha));
        if (status == LINHA_FIM) {
            destino[0] = '\0';
            printf("\nEntrada encerrada.\n");
            return 0;
        }
        texto = aparar(linha);
        if (status == LINHA_LONGA || strlen(texto) >= (size_t)tamanho) {
            printf("Texto longo demais. Use ate %d caracteres.\n", tamanho - 1);
        } else if (texto[0] == '\0') {
            printf("Este campo nao pode ficar vazio.\n");
        } else {
            strcpy(destino, texto);
            return 1;
        }
    }
}

int ler_confirmacao(const char *mensagem) {
    char linha[LINHA_TAMANHO];
    char *texto;
    int i;

    printf("%s", mensagem);
    if (ler_linha(linha, sizeof(linha)) != LINHA_OK) {
        return 0;
    }
    texto = aparar(linha);
    for (i = 0; texto[i] != '\0'; i++) {
        if (texto[i] >= 'A' && texto[i] <= 'Z') {
            texto[i] = (char)(texto[i] - 'A' + 'a');
        }
    }
    return strcmp(texto, "1") == 0 || strcmp(texto, "sim") == 0 || strcmp(texto, "s") == 0;
}

int ler_dado(const char *mensagem, Dado *dado) {
    char linha[LINHA_TAMANHO];
    StatusLinha status;

    for (;;) {
        printf("%s", mensagem);
        status = ler_linha(linha, sizeof(linha));
        if (status == LINHA_FIM) {
            printf("\nEntrada encerrada.\n");
            return 0;
        }
        if (status == LINHA_OK && converteTextoDado(aparar(linha), dado)) {
            return 1;
        }
        printf("Expressao invalida. Use o formato QdF ou QdF+M (exemplo: 1d6, 1d8+1, 2d4-1).\n");
    }
}
