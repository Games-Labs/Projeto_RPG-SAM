#include "texto.h"

int texto_largura_utf8(const char *texto) {
    int largura = 0;

    if (texto == 0) {
        return 0;
    }
    for (; *texto != '\0'; texto++) {
        if (((unsigned char)*texto & 0xC0) != 0x80) {
            largura++;
        }
    }
    return largura;
}

void texto_imprime_alinhado(FILE *saida, const char *texto, int largura) {
    int usado = texto_largura_utf8(texto);

    fputs(texto == 0 ? "" : texto, saida);
    while (usado < largura) {
        fputc(' ', saida);
        usado++;
    }
}
