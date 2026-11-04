#ifndef DADOS_H
#define DADOS_H

#include "monstro.h"

void inicializaAleatorio(unsigned int semente);
unsigned int inicializaAleatorioPorTempo(void);
int sorteiaInteiro(int minimo, int maximo);
int rolaDado(Dado dado);

#endif
