#ifndef ITEM_H
#define ITEM_H

#include "dados.h"

#define ITEM_NOME_TAMANHO 50
#define ITEM_ESPACOS_MINIMO 1
#define ITEM_ESPACOS_MAXIMO 50
#define ITEM_PRECO_MAXIMO 100000
#define ITEM_CHANCE_SUSPEITA_BOA 25
#define ITEM_PENALIDADE_MAO_ERRADA 20

typedef enum {
    TIPO_ITEM_ELMO = 1,
    TIPO_ITEM_PEITORAL,
    TIPO_ITEM_MANOPLAS,
    TIPO_ITEM_CALCA,
    TIPO_ITEM_BOTAS,
    TIPO_ITEM_ANEL,
    TIPO_ITEM_COLAR,
    TIPO_ITEM_CINTO,
    TIPO_ITEM_ARMA_UMA_MAO,
    TIPO_ITEM_ARMA_DUAS_MAOS,
    TIPO_ITEM_RUNA,
    TIPO_ITEM_CONSUMIVEL
} TipoItem;

typedef enum {
    EFEITO_NENHUM,
    EFEITO_CURA,
    EFEITO_BONUS,
    EFEITO_VENENO,
    EFEITO_SUSPEITA
} EfeitoItem;

typedef enum {
    MAO_QUALQUER,
    MAO_PREFERIDA_DIREITA,
    MAO_PREFERIDA_ESQUERDA
} MaoPreferida;

typedef struct {
    int id;
    char nome[ITEM_NOME_TAMANHO];
    TipoItem tipo;
    int espacos;
    int bonus_ataque;
    int bonus_defesa;
    int bonus_vida;
    int bonus_iniciativa;
    int poder;
    Dado dano;
    int preco;
    EfeitoItem efeito;
    MaoPreferida mao;
} Item;

int item_validar(const Item *item);
const char *tipo_item_para_texto(TipoItem tipo);
int tipo_item_eh_valido(TipoItem tipo);
int tipo_item_eh_arma(TipoItem tipo);
const char *efeito_item_para_texto(EfeitoItem efeito);
int efeito_item_eh_valido(EfeitoItem efeito);
const char *mao_preferida_para_texto(MaoPreferida mao);
int mao_preferida_eh_valida(MaoPreferida mao);

#endif
