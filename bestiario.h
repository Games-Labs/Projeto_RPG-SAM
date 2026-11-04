#ifndef BESTIARIO_H
#define BESTIARIO_H

#include "monstro.h"

#define MAX_MONSTROS 100
#define BESTIARIO_ERRO_TAMANHO 200

typedef enum {
    BESTIARIO_OK,
    BESTIARIO_ARQUIVO_INEXISTENTE,
    BESTIARIO_ERRO_FORMATO,
    BESTIARIO_REGISTRO_INVALIDO,
    BESTIARIO_REGISTRO_INCOMPLETO,
    BESTIARIO_CHEIO,
    BESTIARIO_ID_DUPLICADO,
    BESTIARIO_NAO_ENCONTRADO,
    BESTIARIO_DADOS_INVALIDOS,
    BESTIARIO_ERRO_ESCRITA
} ResultadoBestiario;

typedef struct {
    Monstro monstros[MAX_MONSTROS];
    int quantidade;
    int rejeitados;
    char primeiroErro[BESTIARIO_ERRO_TAMANHO];
} Bestiario;

void inicializaBestiario(Bestiario *b);
ResultadoBestiario carregaBestiario(Bestiario *b, const char *nomeArquivo);
ResultadoBestiario salvaBestiario(const Bestiario *b, const char *nomeArquivo);

ResultadoBestiario insereMonstro(Bestiario *b, Monstro monstro);
Monstro *buscaMonstroPorId(Bestiario *b, int id);
const Monstro *consultaMonstroPorId(const Bestiario *b, int id);
Monstro *buscaMonstroPorNome(Bestiario *b, const char *nome);
ResultadoBestiario alteraMonstro(Bestiario *b, int id, Monstro novosDados);
ResultadoBestiario removeMonstro(Bestiario *b, int id);
void listaMonstros(const Bestiario *b);

int listaMonstrosPorTerreno(const Bestiario *b, Terreno terreno, const Monstro *resultado[], int capacidadeResultado);
int listaMonstrosPorPeriodo(const Bestiario *b, Atividade periodo, const Monstro *resultado[], int capacidadeResultado);

const char *resultadoBestiarioParaTexto(ResultadoBestiario resultado);

#endif
