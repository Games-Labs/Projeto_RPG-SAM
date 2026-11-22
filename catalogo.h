#ifndef CATALOGO_H
#define CATALOGO_H

#include "personagem.h"

#define CATALOGO_MAX_ITENS 100
#define ARQUIVO_CATALOGO "DadosItens.txt"
#define CATALOGO_ERRO_ARQUIVO -1

typedef enum {
    LOCAL_LOJA = 1,
    LOCAL_HOSPITAL,
    LOCAL_CABANA,
    LOCAL_FERREIRO
} LocalVenda;

typedef struct {
    Item item;
    LocalVenda local;
} ItemCatalogo;

typedef struct {
    ItemCatalogo itens[CATALOGO_MAX_ITENS];
    int quantidade;
    int rejeitados;
    int linhaPrimeiroErro;
} CatalogoItens;

void catalogo_inicializar(CatalogoItens *catalogo);
int catalogo_carregar(CatalogoItens *catalogo, const char *nomeArquivo);
const ItemCatalogo *catalogo_buscar_por_id(const CatalogoItens *catalogo, int id_item);
int catalogo_listar_local(const CatalogoItens *catalogo, LocalVenda local, const ItemCatalogo *saida[], int maximo);
ResultadoOperacao catalogo_comprar(const CatalogoItens *catalogo, int id_item, LocalVenda local,
                                   Personagem *comprador);
void catalogo_exibir_item(const ItemCatalogo *entrada);
const char *local_venda_para_texto(LocalVenda local);
int local_venda_eh_valido(LocalVenda local);

#endif
