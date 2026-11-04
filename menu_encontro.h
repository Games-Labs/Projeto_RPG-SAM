#ifndef MENU_ENCONTRO_H
#define MENU_ENCONTRO_H

#include "encontro.h"

typedef struct {
    GrupoAventura grupo;
    ContextoEncontro contexto;
    int contextoDefinido;
    ConjuntoPropostas propostas;
    int faixaMinima;
    int faixaMaxima;
} EstadoEncontro;

void inicializaEstadoEncontro(EstadoEncontro *estado);
void menuEncontro(EstadoEncontro *estado, const CadastroPersonagens *cadastro, const Bestiario *b);

#endif
