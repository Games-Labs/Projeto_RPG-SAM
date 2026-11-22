#include "personagem.h"

#include <stdio.h>
#include <string.h>

static void equipamentos_inicializar(Equipamentos *equipamentos) {
    memset(equipamentos, 0, sizeof(*equipamentos));
}

static ItemEquipado *obter_slot(Equipamentos *equipamentos, SlotEquipamento slot) {
    if (equipamentos == 0) {
        return 0;
    }

    switch (slot) {
        case SLOT_ELMO: return &equipamentos->elmo;
        case SLOT_PEITORAL: return &equipamentos->peitoral;
        case SLOT_MANOPLAS: return &equipamentos->manoplas;
        case SLOT_CALCA: return &equipamentos->calca;
        case SLOT_BOTAS: return &equipamentos->botas;
        case SLOT_ANEL: return &equipamentos->anel;
        case SLOT_COLAR: return &equipamentos->colar;
        case SLOT_CINTO: return &equipamentos->cinto;
        case SLOT_MAO_DIREITA: return &equipamentos->mao_direita;
        case SLOT_MAO_ESQUERDA: return &equipamentos->mao_esquerda;
        case SLOT_RUNA: return &equipamentos->runa;
        default: return 0;
    }
}

static const ItemEquipado *obter_slot_const(const Equipamentos *equipamentos, SlotEquipamento slot) {
    return obter_slot((Equipamentos *)equipamentos, slot);
}

static int tipo_compativel_com_slot(TipoItem tipo, SlotEquipamento slot) {
    switch (slot) {
        case SLOT_ELMO: return tipo == TIPO_ITEM_ELMO;
        case SLOT_PEITORAL: return tipo == TIPO_ITEM_PEITORAL;
        case SLOT_MANOPLAS: return tipo == TIPO_ITEM_MANOPLAS;
        case SLOT_CALCA: return tipo == TIPO_ITEM_CALCA;
        case SLOT_BOTAS: return tipo == TIPO_ITEM_BOTAS;
        case SLOT_ANEL: return tipo == TIPO_ITEM_ANEL;
        case SLOT_COLAR: return tipo == TIPO_ITEM_COLAR;
        case SLOT_CINTO: return tipo == TIPO_ITEM_CINTO;
        case SLOT_MAO_DIREITA:
        case SLOT_MAO_ESQUERDA:
            return tipo == TIPO_ITEM_ARMA_UMA_MAO || tipo == TIPO_ITEM_ARMA_DUAS_MAOS;
        case SLOT_RUNA: return tipo == TIPO_ITEM_RUNA;
        default: return 0;
    }
}

static void somar_bonus(const ItemEquipado *equipado, AtributosTotais *totais) {
    if (equipado != 0 && equipado->ocupado) {
        totais->ataque += equipado->item.bonus_ataque;
        totais->defesa += equipado->item.bonus_defesa;
        totais->iniciativa += equipado->item.bonus_iniciativa;
        totais->pontos_vida_maximos += equipado->item.bonus_vida;
        totais->poder += equipado->item.poder;
    }
}

int raca_eh_valida(Raca raca) {
    return raca >= RACA_HUMANO && raca <= RACA_ORC;
}

int classe_eh_valida(Classe classe) {
    return classe >= CLASSE_GUERREIRO && classe <= CLASSE_BARDO;
}

int slot_eh_valido(SlotEquipamento slot) {
    return slot >= SLOT_ELMO && slot <= SLOT_RUNA;
}

const char *raca_para_texto(Raca raca) {
    switch (raca) {
        case RACA_HUMANO: return "Humano";
        case RACA_ELFO: return "Elfo";
        case RACA_ANAO: return "Anao";
        case RACA_HALFLING: return "Halfling";
        case RACA_ORC: return "Orc";
        default: return "Raca desconhecida";
    }
}

const char *classe_para_texto(Classe classe) {
    switch (classe) {
        case CLASSE_GUERREIRO: return "Guerreiro";
        case CLASSE_LADINO: return "Ladino";
        case CLASSE_MAGO: return "Mago";
        case CLASSE_CLERIGO: return "Clerigo";
        case CLASSE_BARDO: return "Bardo";
        default: return "Classe desconhecida";
    }
}

const char *slot_para_texto(SlotEquipamento slot) {
    switch (slot) {
        case SLOT_ELMO: return "Elmo";
        case SLOT_PEITORAL: return "Peitoral";
        case SLOT_MANOPLAS: return "Manoplas";
        case SLOT_CALCA: return "Calca";
        case SLOT_BOTAS: return "Botas";
        case SLOT_ANEL: return "Anel";
        case SLOT_COLAR: return "Colar";
        case SLOT_CINTO: return "Cinto";
        case SLOT_MAO_DIREITA: return "Mao direita";
        case SLOT_MAO_ESQUERDA: return "Mao esquerda";
        case SLOT_RUNA: return "Runa";
        default: return "Slot desconhecido";
    }
}

const char *resultado_operacao_para_texto(ResultadoOperacao resultado) {
    switch (resultado) {
        case RESULTADO_OK: return "Operacao realizada com sucesso.";
        case RESULTADO_CADASTRO_CHEIO: return "Cadastro cheio.";
        case RESULTADO_ID_DUPLICADO: return "ID duplicado.";
        case RESULTADO_NAO_ENCONTRADO: return "Registro nao encontrado.";
        case RESULTADO_DADOS_INVALIDOS: return "Dados invalidos.";
        case RESULTADO_INVENTARIO_SEM_ESPACO: return "Inventario sem espaco suficiente.";
        case RESULTADO_ITEM_INCOMPATIVEL: return "Item incompativel com a posicao escolhida.";
        case RESULTADO_ITEM_NAO_ENCONTRADO: return "Item nao encontrado.";
        case RESULTADO_CONFLITO_ARMA_DUAS_MAOS: return "Conflito com arma de duas maos.";
        case RESULTADO_ITEM_NAO_UTILIZAVEL: return "Este item nao pode ser usado. Apenas consumiveis sao usados.";
        case RESULTADO_MOEDAS_INSUFICIENTES: return "Moedas insuficientes ou acima do limite.";
        default: return "Resultado desconhecido.";
    }
}

void cadastro_inicializar(CadastroPersonagens *cadastro) {
    if (cadastro != 0) {
        cadastro->quantidade = 0;
    }
}

int cadastro_obter_quantidade(const CadastroPersonagens *cadastro) {
    if (cadastro == 0) {
        return 0;
    }
    return cadastro->quantidade;
}

int cadastro_buscar_indice_por_id(const CadastroPersonagens *cadastro, int id_personagem) {
    int i;

    if (cadastro == 0) {
        return -1;
    }

    for (i = 0; i < cadastro->quantidade; i++) {
        if (cadastro->personagens[i].id == id_personagem) {
            return i;
        }
    }

    return -1;
}

Personagem *cadastro_buscar_por_id(CadastroPersonagens *cadastro, int id_personagem) {
    int indice = cadastro_buscar_indice_por_id(cadastro, id_personagem);
    if (indice < 0) {
        return 0;
    }
    return &cadastro->personagens[indice];
}

const Personagem *cadastro_buscar_por_id_const(const CadastroPersonagens *cadastro, int id_personagem) {
    int indice = cadastro_buscar_indice_por_id(cadastro, id_personagem);
    if (indice < 0) {
        return 0;
    }
    return &cadastro->personagens[indice];
}

Personagem personagem_criar_ficha_basica(int id, const char *nome, Raca raca, Classe classe,
                                         int nivel, int pv_maximos, int pv_atuais,
                                         int ataque, int defesa, int iniciativa, int poder) {
    Personagem personagem;

    memset(&personagem, 0, sizeof(personagem));
    personagem.id = id;
    if (nome != 0) {
        strncpy(personagem.nome, nome, PERSONAGEM_NOME_TAMANHO - 1);
        personagem.nome[PERSONAGEM_NOME_TAMANHO - 1] = '\0';
    }
    personagem.raca = raca;
    personagem.classe = classe;
    personagem.nivel = nivel;
    personagem.pontos_vida_maximos = pv_maximos;
    personagem.pontos_vida_atuais = pv_atuais;
    personagem.ataque = ataque;
    personagem.defesa = defesa;
    personagem.iniciativa = iniciativa;
    personagem.poder = poder;
    inventario_inicializar(&personagem.inventario);
    equipamentos_inicializar(&personagem.equipamentos);
    return personagem;
}

int personagem_validar_dados_basicos(const Personagem *personagem) {
    if (personagem == 0) {
        return 0;
    }

    return personagem->id > 0
        && personagem->nome[0] != '\0'
        && strlen(personagem->nome) < PERSONAGEM_NOME_TAMANHO
        && raca_eh_valida(personagem->raca)
        && classe_eh_valida(personagem->classe)
        && personagem->nivel >= PERSONAGEM_NIVEL_MINIMO
        && personagem->nivel <= PERSONAGEM_NIVEL_MAXIMO
        && personagem->pontos_vida_maximos >= PERSONAGEM_PV_MINIMO
        && personagem->pontos_vida_maximos <= PERSONAGEM_PV_MAXIMO
        && personagem->pontos_vida_atuais >= 0
        && personagem->pontos_vida_atuais <= personagem->pontos_vida_maximos
        && personagem->ataque >= PERSONAGEM_ATAQUE_MINIMO
        && personagem->ataque <= PERSONAGEM_ATAQUE_MAXIMO
        && personagem->defesa >= PERSONAGEM_DEFESA_MINIMA
        && personagem->defesa <= PERSONAGEM_DEFESA_MAXIMA
        && personagem->iniciativa >= PERSONAGEM_INICIATIVA_MINIMA
        && personagem->iniciativa <= PERSONAGEM_INICIATIVA_MAXIMA
        && personagem->poder >= PERSONAGEM_PODER_MINIMO
        && personagem->poder <= PERSONAGEM_PODER_MAXIMO
        && personagem->moedas >= 0
        && personagem->moedas <= PERSONAGEM_MOEDAS_MAXIMO
        && personagem->experiencia >= 0
        && personagem->experiencia <= PERSONAGEM_XP_MAXIMO;
}

ResultadoOperacao cadastro_cadastrar(CadastroPersonagens *cadastro, Personagem personagem) {
    if (cadastro == 0 || !personagem_validar_dados_basicos(&personagem)) {
        return RESULTADO_DADOS_INVALIDOS;
    }

    if (cadastro->quantidade >= CADASTRO_MAX_PERSONAGENS) {
        return RESULTADO_CADASTRO_CHEIO;
    }

    if (cadastro_buscar_indice_por_id(cadastro, personagem.id) >= 0) {
        return RESULTADO_ID_DUPLICADO;
    }

    cadastro->personagens[cadastro->quantidade] = personagem;
    cadastro->quantidade++;
    return RESULTADO_OK;
}

ResultadoOperacao cadastro_alterar(CadastroPersonagens *cadastro, int id_atual, Personagem novos_dados) {
    int indice;

    if (cadastro == 0 || !personagem_validar_dados_basicos(&novos_dados)) {
        return RESULTADO_DADOS_INVALIDOS;
    }

    indice = cadastro_buscar_indice_por_id(cadastro, id_atual);
    if (indice < 0) {
        return RESULTADO_NAO_ENCONTRADO;
    }

    if (novos_dados.id != id_atual && cadastro_buscar_indice_por_id(cadastro, novos_dados.id) >= 0) {
        return RESULTADO_ID_DUPLICADO;
    }

    novos_dados.inventario = cadastro->personagens[indice].inventario;
    novos_dados.equipamentos = cadastro->personagens[indice].equipamentos;
    novos_dados.moedas = cadastro->personagens[indice].moedas;
    novos_dados.experiencia = cadastro->personagens[indice].experiencia;
    novos_dados.bonus_temporario_ataque = cadastro->personagens[indice].bonus_temporario_ataque;
    novos_dados.bonus_temporario_defesa = cadastro->personagens[indice].bonus_temporario_defesa;
    novos_dados.bonus_temporario_iniciativa = cadastro->personagens[indice].bonus_temporario_iniciativa;
    cadastro->personagens[indice] = novos_dados;
    return RESULTADO_OK;
}

ResultadoOperacao cadastro_remover(CadastroPersonagens *cadastro, int id_personagem) {
    int indice;
    int i;

    if (cadastro == 0 || id_personagem <= 0) {
        return RESULTADO_DADOS_INVALIDOS;
    }

    indice = cadastro_buscar_indice_por_id(cadastro, id_personagem);
    if (indice < 0) {
        return RESULTADO_NAO_ENCONTRADO;
    }

    for (i = indice; i < cadastro->quantidade - 1; i++) {
        cadastro->personagens[i] = cadastro->personagens[i + 1];
    }

    cadastro->quantidade--;
    return RESULTADO_OK;
}

void cadastro_listar(const CadastroPersonagens *cadastro) {
    int i;

    if (cadastro == 0 || cadastro->quantidade == 0) {
        printf("Nenhum personagem cadastrado.\n");
        return;
    }

    printf("Quantidade cadastrada: %d/%d\n", cadastro->quantidade, CADASTRO_MAX_PERSONAGENS);
    for (i = 0; i < cadastro->quantidade; i++) {
        personagem_exibir(&cadastro->personagens[i]);
    }
}

static int id_esta_equipado(const Personagem *personagem, int id_item) {
    int slot;

    for (slot = SLOT_ELMO; slot <= SLOT_RUNA; slot++) {
        const ItemEquipado *equipado = obter_slot_const(&personagem->equipamentos, (SlotEquipamento)slot);
        if (equipado != 0 && equipado->ocupado && equipado->item.id == id_item) {
            return 1;
        }
    }
    return 0;
}

static ResultadoOperacao devolver_ao_inventario(Personagem *personagem, Item item) {
    ResultadoInventario resultado = inventario_adicionar_item(&personagem->inventario, item);

    if (resultado == INVENTARIO_OK) {
        return RESULTADO_OK;
    }
    return resultado == INVENTARIO_ID_DUPLICADO ? RESULTADO_ID_DUPLICADO : RESULTADO_INVENTARIO_SEM_ESPACO;
}

ResultadoOperacao personagem_adicionar_item(Personagem *personagem, Item item) {
    ResultadoInventario resultado;

    if (personagem == 0) {
        return RESULTADO_DADOS_INVALIDOS;
    }
    if (item_validar(&item) && id_esta_equipado(personagem, item.id)) {
        return RESULTADO_ID_DUPLICADO;
    }

    resultado = inventario_adicionar_item(&personagem->inventario, item);
    if (resultado == INVENTARIO_OK) {
        return RESULTADO_OK;
    }
    if (resultado == INVENTARIO_SEM_ESPACO || resultado == INVENTARIO_CHEIO) {
        return RESULTADO_INVENTARIO_SEM_ESPACO;
    }
    if (resultado == INVENTARIO_ID_DUPLICADO) {
        return RESULTADO_ID_DUPLICADO;
    }
    return RESULTADO_DADOS_INVALIDOS;
}

ResultadoOperacao personagem_remover_item(Personagem *personagem, int id_item, Item *item_removido) {
    ResultadoInventario resultado;

    if (personagem == 0) {
        return RESULTADO_DADOS_INVALIDOS;
    }

    resultado = inventario_remover_item(&personagem->inventario, id_item, item_removido);
    if (resultado == INVENTARIO_OK) {
        return RESULTADO_OK;
    }
    if (resultado == INVENTARIO_ITEM_NAO_ENCONTRADO) {
        return RESULTADO_ITEM_NAO_ENCONTRADO;
    }
    return RESULTADO_DADOS_INVALIDOS;
}

ResultadoOperacao personagem_equipar_item(Personagem *personagem, int id_item, SlotEquipamento slot) {
    Item item_novo;
    ItemEquipado *destino;
    ResultadoInventario resultado_remocao;
    int ocupacao_apos_retirar_item;

    if (personagem == 0 || !slot_eh_valido(slot)) {
        return RESULTADO_DADOS_INVALIDOS;
    }

    if (inventario_buscar_item_por_id(&personagem->inventario, id_item) == 0) {
        return RESULTADO_ITEM_NAO_ENCONTRADO;
    }

    item_novo = *inventario_buscar_item_por_id(&personagem->inventario, id_item);
    if (!tipo_compativel_com_slot(item_novo.tipo, slot)) {
        return RESULTADO_ITEM_INCOMPATIVEL;
    }

    if (personagem->equipamentos.arma_duas_maos_equipada) {
        if (slot == SLOT_MAO_DIREITA || slot == SLOT_MAO_ESQUERDA) {
            return RESULTADO_CONFLITO_ARMA_DUAS_MAOS;
        }
    }

    if (item_novo.tipo == TIPO_ITEM_ARMA_DUAS_MAOS
        && (personagem->equipamentos.mao_direita.ocupado || personagem->equipamentos.mao_esquerda.ocupado)) {
        return RESULTADO_CONFLITO_ARMA_DUAS_MAOS;
    }

    destino = obter_slot(&personagem->equipamentos, slot);
    if (destino == 0) {
        return RESULTADO_DADOS_INVALIDOS;
    }

    ocupacao_apos_retirar_item = inventario_obter_ocupacao(&personagem->inventario) - item_novo.espacos;
    if (destino->ocupado && ocupacao_apos_retirar_item + destino->item.espacos > INVENTARIO_MAX_ESPACOS) {
        return RESULTADO_INVENTARIO_SEM_ESPACO;
    }

    resultado_remocao = inventario_remover_item(&personagem->inventario, id_item, &item_novo);
    if (resultado_remocao != INVENTARIO_OK) {
        return RESULTADO_ITEM_NAO_ENCONTRADO;
    }

    if (destino->ocupado) {
        ResultadoInventario resultado_adicao = inventario_adicionar_item(&personagem->inventario, destino->item);
        if (resultado_adicao != INVENTARIO_OK) {
            inventario_adicionar_item(&personagem->inventario, item_novo);
            return RESULTADO_INVENTARIO_SEM_ESPACO;
        }
    }

    destino->item = item_novo;
    destino->ocupado = 1;
    if (item_novo.tipo == TIPO_ITEM_ARMA_DUAS_MAOS) {
        personagem->equipamentos.mao_direita.item = item_novo;
        personagem->equipamentos.mao_direita.ocupado = 1;
        personagem->equipamentos.mao_esquerda.item = item_novo;
        personagem->equipamentos.mao_esquerda.ocupado = 1;
        personagem->equipamentos.arma_duas_maos_equipada = 1;
    }

    return RESULTADO_OK;
}

ResultadoOperacao personagem_desequipar_item(Personagem *personagem, SlotEquipamento slot) {
    ItemEquipado *origem;
    ResultadoOperacao devolucao;
    Item item;

    if (personagem == 0 || !slot_eh_valido(slot)) {
        return RESULTADO_DADOS_INVALIDOS;
    }

    if (personagem->equipamentos.arma_duas_maos_equipada
        && (slot == SLOT_MAO_DIREITA || slot == SLOT_MAO_ESQUERDA)) {
        item = personagem->equipamentos.mao_direita.item;
        if (inventario_obter_ocupacao(&personagem->inventario) + item.espacos > INVENTARIO_MAX_ESPACOS) {
            return RESULTADO_INVENTARIO_SEM_ESPACO;
        }
        devolucao = devolver_ao_inventario(personagem, item);
        if (devolucao != RESULTADO_OK) {
            return devolucao;
        }
        personagem->equipamentos.mao_direita.ocupado = 0;
        personagem->equipamentos.mao_esquerda.ocupado = 0;
        personagem->equipamentos.arma_duas_maos_equipada = 0;
        return RESULTADO_OK;
    }

    origem = obter_slot(&personagem->equipamentos, slot);
    if (origem == 0 || !origem->ocupado) {
        return RESULTADO_ITEM_NAO_ENCONTRADO;
    }

    item = origem->item;
    if (inventario_obter_ocupacao(&personagem->inventario) + item.espacos > INVENTARIO_MAX_ESPACOS) {
        return RESULTADO_INVENTARIO_SEM_ESPACO;
    }

    devolucao = devolver_ao_inventario(personagem, item);
    if (devolucao != RESULTADO_OK) {
        return devolucao;
    }
    origem->ocupado = 0;
    return RESULTADO_OK;
}

ResultadoOperacao personagem_consumir_item(Personagem *personagem, int id_item, int *pv_atuais, int pv_maximos,
                                           UsoItem *uso) {
    const Item *guardado;
    UsoItem local;
    int pontos;

    if (personagem == 0 || pv_atuais == 0 || pv_maximos < 1) {
        return RESULTADO_DADOS_INVALIDOS;
    }
    guardado = inventario_buscar_item_por_id(&personagem->inventario, id_item);
    if (guardado == 0) {
        return RESULTADO_ITEM_NAO_ENCONTRADO;
    }
    if (guardado->tipo != TIPO_ITEM_CONSUMIVEL || !efeito_item_eh_valido(guardado->efeito)) {
        return RESULTADO_ITEM_NAO_UTILIZAVEL;
    }

    local.item = *guardado;
    local.efeito_aplicado = local.item.efeito;
    local.pontos = 0;
    if (local.efeito_aplicado == EFEITO_SUSPEITA) {
        local.efeito_aplicado = sorteiaInteiro(1, 100) <= ITEM_CHANCE_SUSPEITA_BOA ? EFEITO_CURA : EFEITO_VENENO;
    }

    if (local.efeito_aplicado == EFEITO_BONUS) {
        personagem->bonus_temporario_ataque += local.item.bonus_ataque;
        personagem->bonus_temporario_defesa += local.item.bonus_defesa;
        personagem->bonus_temporario_iniciativa += local.item.bonus_iniciativa;
    } else {
        pontos = rolaDado(local.item.dano);
        if (pontos < 0) {
            pontos = 0;
        }
        if (local.efeito_aplicado == EFEITO_CURA) {
            if (*pv_atuais + pontos > pv_maximos) {
                pontos = pv_maximos - *pv_atuais;
            }
            if (pontos < 0) {
                pontos = 0;
            }
            *pv_atuais += pontos;
        } else {
            if (*pv_atuais - pontos < PERSONAGEM_PV_MINIMO_VENENO) {
                pontos = *pv_atuais - PERSONAGEM_PV_MINIMO_VENENO;
            }
            if (pontos < 0) {
                pontos = 0;
            }
            *pv_atuais -= pontos;
        }
        local.pontos = pontos;
    }

    inventario_remover_item(&personagem->inventario, id_item, 0);
    if (uso != 0) {
        *uso = local;
    }
    return RESULTADO_OK;
}

ResultadoOperacao personagem_usar_item(Personagem *personagem, int id_item, UsoItem *uso) {
    int pv_maximos;

    if (personagem == 0) {
        return RESULTADO_DADOS_INVALIDOS;
    }
    pv_maximos = personagem->pontos_vida_maximos;
    return personagem_consumir_item(personagem, id_item, &personagem->pontos_vida_atuais, pv_maximos, uso);
}

void personagem_descrever_uso(const Personagem *personagem, const UsoItem *uso, char *destino, int tamanho) {
    if (destino == 0 || tamanho <= 0) {
        return;
    }
    if (personagem == 0 || uso == 0) {
        destino[0] = '\0';
        return;
    }
    if (uso->efeito_aplicado == EFEITO_CURA) {
        snprintf(destino, (size_t)tamanho, "%s usou %s e recuperou %d PV.", personagem->nome, uso->item.nome,
                 uso->pontos);
    } else if (uso->efeito_aplicado == EFEITO_VENENO) {
        snprintf(destino, (size_t)tamanho, "%s usou %s: era veneno! Perdeu %d PV.", personagem->nome,
                 uso->item.nome, uso->pontos);
    } else {
        snprintf(destino, (size_t)tamanho, "%s usou %s: atk %+d def %+d ini %+d ate o fim do proximo combate.",
                 personagem->nome, uso->item.nome, uso->item.bonus_ataque, uso->item.bonus_defesa,
                 uso->item.bonus_iniciativa);
    }
}

ResultadoOperacao personagem_alterar_moedas(Personagem *personagem, int variacao) {
    long novo;

    if (personagem == 0) {
        return RESULTADO_DADOS_INVALIDOS;
    }
    novo = (long)personagem->moedas + variacao;
    if (novo < 0 || novo > PERSONAGEM_MOEDAS_MAXIMO) {
        return RESULTADO_MOEDAS_INSUFICIENTES;
    }
    personagem->moedas = (int)novo;
    return RESULTADO_OK;
}

void personagem_limpar_bonus_temporarios(Personagem *personagem) {
    if (personagem != 0) {
        personagem->bonus_temporario_ataque = 0;
        personagem->bonus_temporario_defesa = 0;
        personagem->bonus_temporario_iniciativa = 0;
    }
}

int personagem_xp_proximo_nivel(const Personagem *personagem) {
    if (personagem == 0 || personagem->nivel < PERSONAGEM_NIVEL_MINIMO) {
        return PERSONAGEM_XP_BASE_NIVEL;
    }
    return PERSONAGEM_XP_BASE_NIVEL * personagem->nivel;
}

static void sobe_nivel(Personagem *personagem) {
    int ganho = PERSONAGEM_PV_POR_NIVEL;

    personagem->nivel++;
    if (ganho > PERSONAGEM_PV_MAXIMO - personagem->pontos_vida_maximos) {
        ganho = PERSONAGEM_PV_MAXIMO - personagem->pontos_vida_maximos;
    }
    personagem->pontos_vida_maximos += ganho;
    if (personagem->pontos_vida_atuais > 0) {
        personagem->pontos_vida_atuais += ganho;
    }
    if (personagem->poder < PERSONAGEM_PODER_MAXIMO) {
        personagem->poder++;
    }
    if (personagem->nivel % 2 == 0) {
        if (personagem->ataque < PERSONAGEM_ATAQUE_MAXIMO) {
            personagem->ataque++;
        }
    } else if (personagem->defesa < PERSONAGEM_DEFESA_MAXIMA) {
        personagem->defesa++;
    }
}

int personagem_ganhar_experiencia(Personagem *personagem, int experiencia) {
    int niveis = 0;

    if (personagem == 0 || experiencia <= 0) {
        return 0;
    }
    if (experiencia > PERSONAGEM_XP_MAXIMO - personagem->experiencia) {
        experiencia = PERSONAGEM_XP_MAXIMO - personagem->experiencia;
    }
    personagem->experiencia += experiencia;
    while (personagem->nivel < PERSONAGEM_NIVEL_MAXIMO
           && personagem->experiencia >= personagem_xp_proximo_nivel(personagem)) {
        personagem->experiencia -= personagem_xp_proximo_nivel(personagem);
        sobe_nivel(personagem);
        niveis++;
    }
    if (personagem->nivel >= PERSONAGEM_NIVEL_MAXIMO) {
        personagem->experiencia = 0;
    }
    return niveis;
}

int personagem_item_em_mao_errada(const Item *item, SlotEquipamento slot) {
    if (item == 0 || item->tipo != TIPO_ITEM_ARMA_UMA_MAO) {
        return 0;
    }
    return (item->mao == MAO_PREFERIDA_DIREITA && slot == SLOT_MAO_ESQUERDA)
        || (item->mao == MAO_PREFERIDA_ESQUERDA && slot == SLOT_MAO_DIREITA);
}

int personagem_usa_mao_errada(const Personagem *personagem) {
    const Equipamentos *equipamentos;

    if (personagem == 0 || personagem->equipamentos.arma_duas_maos_equipada) {
        return 0;
    }
    equipamentos = &personagem->equipamentos;
    return (equipamentos->mao_direita.ocupado
            && personagem_item_em_mao_errada(&equipamentos->mao_direita.item, SLOT_MAO_DIREITA))
        || (equipamentos->mao_esquerda.ocupado
            && personagem_item_em_mao_errada(&equipamentos->mao_esquerda.item, SLOT_MAO_ESQUERDA));
}

static int aplicar_penalidade_mao(int valor) {
    if (valor <= 0) {
        return valor;
    }
    return valor - (valor * ITEM_PENALIDADE_MAO_ERRADA + 99) / 100;
}

int personagem_proximo_id_item(const Personagem *personagem) {
    int maior = 0;
    int slot;
    int i;

    if (personagem == 0) {
        return 1;
    }
    for (i = 0; i < personagem->inventario.quantidade; i++) {
        if (personagem->inventario.itens[i].id > maior) {
            maior = personagem->inventario.itens[i].id;
        }
    }
    for (slot = SLOT_ELMO; slot <= SLOT_RUNA; slot++) {
        const ItemEquipado *equipado = obter_slot_const(&personagem->equipamentos, (SlotEquipamento)slot);
        if (equipado != 0 && equipado->ocupado && equipado->item.id > maior) {
            maior = equipado->item.id;
        }
    }
    return maior + 1;
}

AtributosTotais personagem_calcular_atributos_totais(const Personagem *personagem) {
    AtributosTotais totais;

    totais.ataque = 0;
    totais.defesa = 0;
    totais.iniciativa = 0;
    totais.pontos_vida_maximos = 0;
    totais.poder = 0;

    if (personagem == 0) {
        return totais;
    }

    totais.ataque = personagem->ataque;
    totais.defesa = personagem->defesa;
    totais.iniciativa = personagem->iniciativa;
    totais.pontos_vida_maximos = personagem->pontos_vida_maximos;
    totais.poder = personagem->poder;
    totais.ataque += personagem->bonus_temporario_ataque;
    totais.defesa += personagem->bonus_temporario_defesa;
    totais.iniciativa += personagem->bonus_temporario_iniciativa;

    somar_bonus(&personagem->equipamentos.elmo, &totais);
    somar_bonus(&personagem->equipamentos.peitoral, &totais);
    somar_bonus(&personagem->equipamentos.manoplas, &totais);
    somar_bonus(&personagem->equipamentos.calca, &totais);
    somar_bonus(&personagem->equipamentos.botas, &totais);
    somar_bonus(&personagem->equipamentos.anel, &totais);
    somar_bonus(&personagem->equipamentos.colar, &totais);
    somar_bonus(&personagem->equipamentos.cinto, &totais);
    somar_bonus(&personagem->equipamentos.mao_direita, &totais);
    if (!personagem->equipamentos.arma_duas_maos_equipada) {
        somar_bonus(&personagem->equipamentos.mao_esquerda, &totais);
    }
    somar_bonus(&personagem->equipamentos.runa, &totais);
    if (personagem_usa_mao_errada(personagem)) {
        totais.ataque = aplicar_penalidade_mao(totais.ataque);
        totais.defesa = aplicar_penalidade_mao(totais.defesa);
    }

    return totais;
}

int personagem_obter_arma_principal(const Personagem *personagem, Item *arma) {
    const ItemEquipado *escolhida;

    if (personagem == 0) {
        return 0;
    }

    if (personagem->equipamentos.mao_direita.ocupado) {
        escolhida = &personagem->equipamentos.mao_direita;
    } else if (personagem->equipamentos.mao_esquerda.ocupado) {
        escolhida = &personagem->equipamentos.mao_esquerda;
    } else {
        return 0;
    }

    if (!tipo_item_eh_arma(escolhida->item.tipo) || !validaDado(escolhida->item.dano)) {
        return 0;
    }

    if (arma != 0) {
        *arma = escolhida->item;
    }
    return 1;
}

void personagem_listar_inventario(const Personagem *personagem) {
    if (personagem == 0) {
        printf("Personagem inexistente.\n");
        return;
    }
    inventario_listar(&personagem->inventario);
}

void personagem_exibir(const Personagem *personagem) {
    if (personagem == 0) {
        printf("Personagem inexistente.\n");
        return;
    }

    printf("\nID %d | %s | %s %s | Nivel %d\n",
           personagem->id,
           personagem->nome,
           raca_para_texto(personagem->raca),
           classe_para_texto(personagem->classe),
           personagem->nivel);
    printf("PV %d/%d | Ataque %d | Defesa %d | Iniciativa %d | Poder %d\n",
           personagem->pontos_vida_atuais,
           personagem->pontos_vida_maximos,
           personagem->ataque,
           personagem->defesa,
           personagem->iniciativa,
           personagem->poder);
    printf("Moedas %d | XP %d/%d", personagem->moedas, personagem->experiencia,
           personagem_xp_proximo_nivel(personagem));
    if (personagem->bonus_temporario_ataque != 0 || personagem->bonus_temporario_defesa != 0
        || personagem->bonus_temporario_iniciativa != 0) {
        printf(" | Bonus temporarios: atk %+d def %+d ini %+d", personagem->bonus_temporario_ataque,
               personagem->bonus_temporario_defesa, personagem->bonus_temporario_iniciativa);
    }
    printf("\n");
}

static void exibir_slot(const Equipamentos *equipamentos, SlotEquipamento slot) {
    const ItemEquipado *equipado = obter_slot_const(equipamentos, slot);

    printf("%s: ", slot_para_texto(slot));
    if (equipado != 0 && equipado->ocupado) {
        if (equipamentos->arma_duas_maos_equipada && slot == SLOT_MAO_ESQUERDA) {
            printf("bloqueada por %s\n", equipado->item.nome);
        } else {
            printf("%s (%s, ID %d)%s\n",
                   equipado->item.nome,
                   tipo_item_para_texto(equipado->item.tipo),
                   equipado->item.id,
                   personagem_item_em_mao_errada(&equipado->item, slot) ? " [mao errada: -20% ataque e defesa]" : "");
        }
    } else {
        printf("vazio\n");
    }
}

void personagem_exibir_equipamentos(const Personagem *personagem) {
    int slot;

    if (personagem == 0) {
        printf("Personagem inexistente.\n");
        return;
    }

    printf("Equipamentos de %s:\n", personagem->nome);
    for (slot = SLOT_ELMO; slot <= SLOT_RUNA; slot++) {
        exibir_slot(&personagem->equipamentos, (SlotEquipamento)slot);
    }
}

void personagem_exibir_atributos_totais(const Personagem *personagem) {
    AtributosTotais totais = personagem_calcular_atributos_totais(personagem);

    if (personagem == 0) {
        printf("Personagem inexistente.\n");
        return;
    }

    printf("Atributos totais de %s:\n", personagem->nome);
    printf("Ataque: %d\n", totais.ataque);
    printf("Defesa: %d\n", totais.defesa);
    printf("Iniciativa: %d\n", totais.iniciativa);
    printf("PV maximos: %d\n", totais.pontos_vida_maximos);
    printf("Poder: %d\n", totais.poder);
    if (personagem_usa_mao_errada(personagem)) {
        printf("Arma na mao errada: ataque e defesa ja estao com -%d%%.\n", ITEM_PENALIDADE_MAO_ERRADA);
    }
}
