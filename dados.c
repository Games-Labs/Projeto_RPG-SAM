#include "dados.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int lerNumero(const char **posicao, int *saida) {
    int digitos = 0;
    int valor = 0;

    while (isdigit((unsigned char)**posicao)) {
        if (digitos >= 6) {
            return 0;
        }
        valor = valor * 10 + (**posicao - '0');
        digitos++;
        (*posicao)++;
    }

    if (digitos == 0) {
        return 0;
    }

    *saida = valor;
    return 1;
}

int validaDado(Dado dado) {
    return dado.quantidade >= 1 && dado.quantidade <= DADO_QUANTIDADE_MAXIMA
        && dado.faces >= 1 && dado.faces <= DADO_FACES_MAXIMAS
        && dado.modificador >= -DADO_MODIFICADOR_MAXIMO && dado.modificador <= DADO_MODIFICADOR_MAXIMO;
}

int converteTextoDado(const char *texto, Dado *dado) {
    Dado temporario;
    const char *posicao;
    int modificador;
    int sinal;

    if (texto == 0 || dado == 0) {
        return 0;
    }

    posicao = texto;
    if (!lerNumero(&posicao, &temporario.quantidade) || *posicao != 'd') {
        return 0;
    }
    posicao++;
    if (!lerNumero(&posicao, &temporario.faces)) {
        return 0;
    }

    temporario.modificador = 0;
    if (*posicao == '+' || *posicao == '-') {
        sinal = *posicao == '-' ? -1 : 1;
        posicao++;
        if (!lerNumero(&posicao, &modificador)) {
            return 0;
        }
        temporario.modificador = sinal * modificador;
    }

    if (*posicao != '\0' || !validaDado(temporario)) {
        return 0;
    }

    *dado = temporario;
    return 1;
}

void dadoParaTexto(Dado dado, char *destino, int tamanho) {
    if (destino == 0 || tamanho <= 0) {
        return;
    }

    if (dado.modificador > 0) {
        snprintf(destino, (size_t)tamanho, "%dd%d+%d", dado.quantidade, dado.faces, dado.modificador);
    } else if (dado.modificador < 0) {
        snprintf(destino, (size_t)tamanho, "%dd%d%d", dado.quantidade, dado.faces, dado.modificador);
    } else {
        snprintf(destino, (size_t)tamanho, "%dd%d", dado.quantidade, dado.faces);
    }
}

void inicializaAleatorio(unsigned int semente) {
    srand(semente);
}

unsigned int inicializaAleatorioPorTempo(void) {
    unsigned int semente = (unsigned int)time(0);

    srand(semente);
    return semente;
}

int sorteiaInteiro(int minimo, int maximo) {
    if (maximo <= minimo) {
        return minimo;
    }
    return minimo + rand() % (maximo - minimo + 1);
}

int rolaDado(Dado dado) {
    int resultado = dado.modificador;
    int i;

    if (dado.quantidade <= 0 || dado.faces <= 0) {
        return 0;
    }
    for (i = 0; i < dado.quantidade; i++) {
        resultado += 1 + rand() % dado.faces;
    }
    return resultado;
}
