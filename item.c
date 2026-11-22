#include "item.h"

#include <string.h>

int tipo_item_eh_valido(TipoItem tipo) {
    return tipo >= TIPO_ITEM_ELMO && tipo <= TIPO_ITEM_CONSUMIVEL;
}

int tipo_item_eh_arma(TipoItem tipo) {
    return tipo == TIPO_ITEM_ARMA_UMA_MAO || tipo == TIPO_ITEM_ARMA_DUAS_MAOS;
}

static int dano_nulo(Dado dano) {
    return dano.quantidade == 0 && dano.faces == 0 && dano.modificador == 0;
}

const char *tipo_item_para_texto(TipoItem tipo) {
    switch (tipo) {
        case TIPO_ITEM_ELMO: return "Elmo";
        case TIPO_ITEM_PEITORAL: return "Peitoral";
        case TIPO_ITEM_MANOPLAS: return "Manoplas";
        case TIPO_ITEM_CALCA: return "Calca";
        case TIPO_ITEM_BOTAS: return "Botas";
        case TIPO_ITEM_ANEL: return "Anel";
        case TIPO_ITEM_COLAR: return "Colar";
        case TIPO_ITEM_CINTO: return "Cinto";
        case TIPO_ITEM_ARMA_UMA_MAO: return "Arma de uma mao";
        case TIPO_ITEM_ARMA_DUAS_MAOS: return "Arma de duas maos";
        case TIPO_ITEM_RUNA: return "Runa";
        case TIPO_ITEM_CONSUMIVEL: return "Consumivel";
        default: return "Tipo desconhecido";
    }
}

int efeito_item_eh_valido(EfeitoItem efeito) {
    return efeito >= EFEITO_CURA && efeito <= EFEITO_SUSPEITA;
}

const char *efeito_item_para_texto(EfeitoItem efeito) {
    switch (efeito) {
        case EFEITO_NENHUM: return "Nenhum";
        case EFEITO_CURA: return "Cura";
        case EFEITO_BONUS: return "Bonus temporario";
        case EFEITO_VENENO: return "Veneno";
        case EFEITO_SUSPEITA: return "Suspeita";
        default: return "Efeito desconhecido";
    }
}

int mao_preferida_eh_valida(MaoPreferida mao) {
    return mao >= MAO_QUALQUER && mao <= MAO_PREFERIDA_ESQUERDA;
}

const char *mao_preferida_para_texto(MaoPreferida mao) {
    switch (mao) {
        case MAO_QUALQUER: return "qualquer mao";
        case MAO_PREFERIDA_DIREITA: return "mao direita";
        case MAO_PREFERIDA_ESQUERDA: return "mao esquerda";
        default: return "mao desconhecida";
    }
}

static int mao_valida_para_o_tipo(const Item *item) {
    if (!mao_preferida_eh_valida(item->mao)) {
        return 0;
    }
    return item->mao == MAO_QUALQUER || item->tipo == TIPO_ITEM_ARMA_UMA_MAO;
}

static int dano_e_efeito_validos(const Item *item) {
    if (item->tipo == TIPO_ITEM_CONSUMIVEL) {
        if (!efeito_item_eh_valido(item->efeito)) {
            return 0;
        }
        return item->efeito == EFEITO_BONUS ? dano_nulo(item->dano) : validaDado(item->dano);
    }
    if (item->efeito != EFEITO_NENHUM) {
        return 0;
    }
    return tipo_item_eh_arma(item->tipo) ? validaDado(item->dano) : dano_nulo(item->dano);
}

int item_validar(const Item *item) {
    if (item == 0) {
        return 0;
    }

    return item->id > 0
        && item->nome[0] != '\0'
        && strlen(item->nome) < ITEM_NOME_TAMANHO
        && tipo_item_eh_valido(item->tipo)
        && item->espacos >= ITEM_ESPACOS_MINIMO
        && item->espacos <= ITEM_ESPACOS_MAXIMO
        && item->poder >= 0
        && item->preco >= 0
        && item->preco <= ITEM_PRECO_MAXIMO
        && mao_valida_para_o_tipo(item)
        && dano_e_efeito_validos(item);
}
