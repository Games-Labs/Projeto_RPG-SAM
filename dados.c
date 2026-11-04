#include "dados.h"

#include <stdlib.h>
#include <time.h>

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
