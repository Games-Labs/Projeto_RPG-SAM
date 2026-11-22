#ifndef TEXTO_H
#define TEXTO_H

#include <stdio.h>

int texto_largura_utf8(const char *texto);
void texto_imprime_alinhado(FILE *saida, const char *texto, int largura);

#endif
