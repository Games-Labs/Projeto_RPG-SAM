#ifndef MENU_COMBATE_H
#define MENU_COMBATE_H

#include "combate.h"
#include "menu_encontro.h"
#include "som.h"

void menuCombate(Combate *combate, CadastroPersonagens *cadastro, const EstadoEncontro *encontro, const Bestiario *b,
                 Som *som);
void tocarSomFimCombate(Som *som, const Combate *combate);
void usarItemForaDeCombate(Personagem *personagem, char aviso[]);
void usarItemEmCombate(Combate *combate);

#endif
