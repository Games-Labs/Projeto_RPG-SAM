#ifndef CIDADE_H
#define CIDADE_H

#include "catalogo.h"
#include "som.h"

#define CIDADE_PRECO_TRATAMENTO 30
#define CIDADE_PRECO_ACADEMIA 60
#define CIDADE_CHANCE_ACADEMIA 50
#define CIDADE_PRECO_BAR 5
#define CIDADE_CHANCE_BAR_CURA 50
#define CIDADE_CHANCE_ROUBO 25
#define CIDADE_PERCENTUAL_VENDA 50
#define CIDADE_PRECO_ESTUDO 70
#define CIDADE_CHANCE_ESTUDO 50
#define CIDADE_PV_ESTUDO 2

typedef enum {
    TREINO_ATAQUE = 1,
    TREINO_DEFESA,
    TREINO_INICIATIVA
} Treino;

typedef enum {
    ESTUDO_MAGIA = 1,
    ESTUDO_MEDICINA
} Estudo;

typedef enum {
    CIDADE_SUCESSO,
    CIDADE_FRACASSO,
    CIDADE_SEM_MOEDAS,
    CIDADE_SEM_EFEITO,
    CIDADE_PERSONAGEM_CAIDO,
    CIDADE_DADOS_INVALIDOS,
    CIDADE_ITEM_NAO_ENCONTRADO,
    CIDADE_SEM_VALOR
} ResultadoCidade;

int treino_eh_valido(Treino treino);
const char *treino_para_texto(Treino treino);
const char *resultado_cidade_para_texto(ResultadoCidade resultado);
ResultadoCidade cidade_tratar(Personagem *personagem);
ResultadoCidade cidade_treinar(Personagem *personagem, Treino treino);
ResultadoCidade cidade_bar(Personagem *personagem, int *pontos);
ResultadoCidade cidade_roubar(Personagem *personagem, int *valor);
int cidade_item_importante(const Item *item);
int estudo_eh_valido(Estudo estudo);
const char *estudo_para_texto(Estudo estudo);
int cidade_valor_venda(const Item *item);
ResultadoCidade cidade_vender_item(Personagem *personagem, int id_item, int *valor);
ResultadoCidade cidade_estudar(Personagem *personagem, Estudo estudo);
void menuCidade(Personagem *personagem, const CatalogoItens *catalogo, int capitulosLiberados,
                Som *som);

#endif
