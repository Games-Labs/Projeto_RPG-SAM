#include "cidade.h"

static const Dado DADO_BAR_CURA = {1, 8, 0};
static const Dado DADO_BAR_BRIGA = {1, 4, 0};
static const Dado DADO_ROUBO_GANHO = {2, 10, 0};
static const Dado DADO_ROUBO_SURRA = {1, 6, 0};

int treino_eh_valido(Treino treino) {
    return treino >= TREINO_ATAQUE && treino <= TREINO_INICIATIVA;
}

const char *treino_para_texto(Treino treino) {
    switch (treino) {
        case TREINO_ATAQUE: return "Ataque";
        case TREINO_DEFESA: return "Defesa";
        case TREINO_INICIATIVA: return "Iniciativa";
        default: return "Treino desconhecido";
    }
}

const char *resultado_cidade_para_texto(ResultadoCidade resultado) {
    switch (resultado) {
        case CIDADE_SUCESSO: return "Deu certo.";
        case CIDADE_FRACASSO: return "Nao deu certo.";
        case CIDADE_SEM_MOEDAS: return "Moedas insuficientes.";
        case CIDADE_SEM_EFEITO: return "Nada a fazer: o personagem ja esta no limite.";
        case CIDADE_PERSONAGEM_CAIDO: return "O personagem esta com 0 PV e precisa de tratamento no hospital.";
        case CIDADE_DADOS_INVALIDOS: return "Dados invalidos.";
        case CIDADE_ITEM_NAO_ENCONTRADO: return "Item nao encontrado na bolsa.";
        case CIDADE_SEM_VALOR: return "Ninguem paga por esse item (ele nao tem preco).";
        default: return "Resultado desconhecido.";
    }
}

static int perde_vida(Personagem *personagem, int pontos) {
    if (personagem->pontos_vida_atuais - pontos < PERSONAGEM_PV_MINIMO_VENENO) {
        pontos = personagem->pontos_vida_atuais - PERSONAGEM_PV_MINIMO_VENENO;
    }
    if (pontos < 0) {
        pontos = 0;
    }
    personagem->pontos_vida_atuais -= pontos;
    return pontos;
}

static int ganha_vida(Personagem *personagem, int pontos) {
    if (personagem->pontos_vida_atuais + pontos > personagem->pontos_vida_maximos) {
        pontos = personagem->pontos_vida_maximos - personagem->pontos_vida_atuais;
    }
    if (pontos < 0) {
        pontos = 0;
    }
    personagem->pontos_vida_atuais += pontos;
    return pontos;
}

ResultadoCidade cidade_tratar(Personagem *personagem) {
    if (personagem == 0) {
        return CIDADE_DADOS_INVALIDOS;
    }
    if (personagem->pontos_vida_atuais >= personagem->pontos_vida_maximos) {
        return CIDADE_SEM_EFEITO;
    }
    if (personagem_alterar_moedas(personagem, -CIDADE_PRECO_TRATAMENTO) != RESULTADO_OK) {
        return CIDADE_SEM_MOEDAS;
    }
    personagem->pontos_vida_atuais = personagem->pontos_vida_maximos;
    return CIDADE_SUCESSO;
}

ResultadoCidade cidade_treinar(Personagem *personagem, Treino treino) {
    int *atributo;
    int maximo;

    if (personagem == 0 || !treino_eh_valido(treino)) {
        return CIDADE_DADOS_INVALIDOS;
    }
    if (treino == TREINO_ATAQUE) {
        atributo = &personagem->ataque;
        maximo = PERSONAGEM_ATAQUE_MAXIMO;
    } else if (treino == TREINO_DEFESA) {
        atributo = &personagem->defesa;
        maximo = PERSONAGEM_DEFESA_MAXIMA;
    } else {
        atributo = &personagem->iniciativa;
        maximo = PERSONAGEM_INICIATIVA_MAXIMA;
    }
    if (*atributo >= maximo) {
        return CIDADE_SEM_EFEITO;
    }
    if (personagem_alterar_moedas(personagem, -CIDADE_PRECO_ACADEMIA) != RESULTADO_OK) {
        return CIDADE_SEM_MOEDAS;
    }
    if (sorteiaInteiro(1, 100) > CIDADE_CHANCE_ACADEMIA) {
        return CIDADE_FRACASSO;
    }
    (*atributo)++;
    return CIDADE_SUCESSO;
}

ResultadoCidade cidade_bar(Personagem *personagem, int *pontos) {
    int valor;
    ResultadoCidade resultado;

    if (personagem == 0) {
        return CIDADE_DADOS_INVALIDOS;
    }
    if (personagem->pontos_vida_atuais <= 0) {
        return CIDADE_PERSONAGEM_CAIDO;
    }
    if (personagem_alterar_moedas(personagem, -CIDADE_PRECO_BAR) != RESULTADO_OK) {
        return CIDADE_SEM_MOEDAS;
    }
    if (sorteiaInteiro(1, 100) <= CIDADE_CHANCE_BAR_CURA) {
        valor = ganha_vida(personagem, rolaDado(DADO_BAR_CURA));
        resultado = CIDADE_SUCESSO;
    } else {
        valor = perde_vida(personagem, rolaDado(DADO_BAR_BRIGA));
        resultado = CIDADE_FRACASSO;
    }
    if (pontos != 0) {
        *pontos = valor;
    }
    return resultado;
}

ResultadoCidade cidade_roubar(Personagem *personagem, int *valor) {
    int quantia;
    ResultadoCidade resultado;

    if (personagem == 0) {
        return CIDADE_DADOS_INVALIDOS;
    }
    if (personagem->pontos_vida_atuais <= 0) {
        return CIDADE_PERSONAGEM_CAIDO;
    }
    if (sorteiaInteiro(1, 100) <= CIDADE_CHANCE_ROUBO) {
        quantia = rolaDado(DADO_ROUBO_GANHO);
        if (quantia > PERSONAGEM_MOEDAS_MAXIMO - personagem->moedas) {
            quantia = PERSONAGEM_MOEDAS_MAXIMO - personagem->moedas;
        }
        personagem_alterar_moedas(personagem, quantia);
        resultado = CIDADE_SUCESSO;
    } else {
        quantia = perde_vida(personagem, rolaDado(DADO_ROUBO_SURRA));
        resultado = CIDADE_FRACASSO;
    }
    if (valor != 0) {
        *valor = quantia;
    }
    return resultado;
}

int estudo_eh_valido(Estudo estudo) {
    return estudo >= ESTUDO_MAGIA && estudo <= ESTUDO_MEDICINA;
}

const char *estudo_para_texto(Estudo estudo) {
    switch (estudo) {
        case ESTUDO_MAGIA: return "Magia antiga (+1 de poder)";
        case ESTUDO_MEDICINA: return "Medicina (+2 PV maximos)";
        default: return "Estudo desconhecido";
    }
}

int cidade_valor_venda(const Item *item) {
    if (item == 0 || item->preco <= 0) {
        return 0;
    }
    return item->preco * CIDADE_PERCENTUAL_VENDA / 100;
}

ResultadoCidade cidade_vender_item(Personagem *personagem, int id_item, int *valor) {
    const Item *item;
    int quantia;

    if (personagem == 0) {
        return CIDADE_DADOS_INVALIDOS;
    }
    item = inventario_buscar_item_por_id(&personagem->inventario, id_item);
    if (item == 0) {
        return CIDADE_ITEM_NAO_ENCONTRADO;
    }
    quantia = cidade_valor_venda(item);
    if (quantia <= 0) {
        return CIDADE_SEM_VALOR;
    }
    if (quantia > PERSONAGEM_MOEDAS_MAXIMO - personagem->moedas) {
        return CIDADE_SEM_EFEITO;
    }
    if (personagem_remover_item(personagem, id_item, 0) != RESULTADO_OK) {
        return CIDADE_ITEM_NAO_ENCONTRADO;
    }
    personagem_alterar_moedas(personagem, quantia);
    if (valor != 0) {
        *valor = quantia;
    }
    return CIDADE_SUCESSO;
}

ResultadoCidade cidade_estudar(Personagem *personagem, Estudo estudo) {
    if (personagem == 0 || !estudo_eh_valido(estudo)) {
        return CIDADE_DADOS_INVALIDOS;
    }
    if ((estudo == ESTUDO_MAGIA && personagem->poder >= PERSONAGEM_PODER_MAXIMO)
        || (estudo == ESTUDO_MEDICINA && personagem->pontos_vida_maximos + CIDADE_PV_ESTUDO > PERSONAGEM_PV_MAXIMO)) {
        return CIDADE_SEM_EFEITO;
    }
    if (personagem_alterar_moedas(personagem, -CIDADE_PRECO_ESTUDO) != RESULTADO_OK) {
        return CIDADE_SEM_MOEDAS;
    }
    if (sorteiaInteiro(1, 100) > CIDADE_CHANCE_ESTUDO) {
        return CIDADE_FRACASSO;
    }
    if (estudo == ESTUDO_MAGIA) {
        personagem->poder++;
    } else {
        personagem->pontos_vida_maximos += CIDADE_PV_ESTUDO;
    }
    return CIDADE_SUCESSO;
}

int cidade_item_importante(const Item *item) {
    return item != 0 && item->tipo != TIPO_ITEM_CONSUMIVEL;
}
