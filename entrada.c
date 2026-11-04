#include "entrada.h"

#include <stdio.h>
#include <string.h>

void limpar_entrada(void) {
    int caractere;
    while ((caractere = getchar()) != '\n' && caractere != EOF) {
    }
}

int ler_inteiro(const char *mensagem) {
    int valor;
    int lidos;

    do {
        printf("%s", mensagem);
        lidos = scanf("%d", &valor);
        if (lidos == EOF) {
            printf("\nEntrada encerrada.\n");
            return 0;
        }
        limpar_entrada();
        if (lidos != 1) {
            printf("Entrada invalida. Digite um numero inteiro.\n");
        }
    } while (lidos != 1);

    return valor;
}

void ler_texto(const char *mensagem, char destino[], int tamanho) {
    size_t tamanho_lido;

    printf("%s", mensagem);
    if (fgets(destino, tamanho, stdin) == 0) {
        destino[0] = '\0';
        return;
    }

    tamanho_lido = strlen(destino);
    if (tamanho_lido > 0 && destino[tamanho_lido - 1] == '\n') {
        destino[tamanho_lido - 1] = '\0';
    } else {
        limpar_entrada();
    }
}
