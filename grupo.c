#include "grupo.h"

#include <stdio.h>

const char *resultadoGrupoParaTexto(ResultadoGrupo resultado) {
    switch (resultado) {
        case GRUPO_OK: return "Operacao realizada com sucesso.";
        case GRUPO_CHEIO: return "Grupo cheio.";
        case GRUPO_PERSONAGEM_REPETIDO: return "Personagem ja esta no grupo.";
        case GRUPO_PERSONAGEM_INEXISTENTE: return "Personagem nao cadastrado.";
        case GRUPO_PERSONAGEM_NAO_ESTA_NO_GRUPO: return "Personagem nao esta no grupo.";
        case GRUPO_DADOS_INVALIDOS: return "Dados invalidos.";
        default: return "Resultado desconhecido.";
    }
}

void inicializaGrupo(GrupoAventura *grupo) {
    if (grupo == 0) {
        return;
    }
    grupo->quantidade = 0;
    grupo->poderTotal = 0;
}

static int posicaoNoGrupo(const GrupoAventura *grupo, int idPersonagem) {
    int i;

    for (i = 0; i < grupo->quantidade; i++) {
        if (grupo->idsPersonagens[i] == idPersonagem) {
            return i;
        }
    }
    return -1;
}

ResultadoGrupo adicionaPersonagemGrupo(GrupoAventura *grupo, const CadastroPersonagens *cadastro, int idPersonagem) {
    if (grupo == 0 || cadastro == 0 || idPersonagem <= 0) {
        return GRUPO_DADOS_INVALIDOS;
    }
    if (grupo->quantidade >= MAX_PERSONAGENS_GRUPO) {
        return GRUPO_CHEIO;
    }
    if (posicaoNoGrupo(grupo, idPersonagem) >= 0) {
        return GRUPO_PERSONAGEM_REPETIDO;
    }
    if (cadastro_buscar_por_id_const(cadastro, idPersonagem) == 0) {
        return GRUPO_PERSONAGEM_INEXISTENTE;
    }

    grupo->idsPersonagens[grupo->quantidade] = idPersonagem;
    grupo->quantidade++;
    calculaPoderGrupo(grupo, cadastro);
    return GRUPO_OK;
}

ResultadoGrupo removePersonagemGrupo(GrupoAventura *grupo, int idPersonagem) {
    int posicao;
    int i;

    if (grupo == 0) {
        return GRUPO_DADOS_INVALIDOS;
    }
    posicao = posicaoNoGrupo(grupo, idPersonagem);
    if (posicao < 0) {
        return GRUPO_PERSONAGEM_NAO_ESTA_NO_GRUPO;
    }
    for (i = posicao; i < grupo->quantidade - 1; i++) {
        grupo->idsPersonagens[i] = grupo->idsPersonagens[i + 1];
    }
    grupo->quantidade--;
    if (grupo->quantidade == 0) {
        grupo->poderTotal = 0;
    }
    return GRUPO_OK;
}

int calculaPoderGrupo(GrupoAventura *grupo, const CadastroPersonagens *cadastro) {
    const Personagem *personagem;
    int total = 0;
    int i;

    if (grupo == 0 || cadastro == 0) {
        return -1;
    }
    for (i = 0; i < grupo->quantidade; i++) {
        personagem = cadastro_buscar_por_id_const(cadastro, grupo->idsPersonagens[i]);
        if (personagem == 0) {
            grupo->poderTotal = 0;
            return -1;
        }
        total += personagem_calcular_atributos_totais(personagem).poder;
    }
    grupo->poderTotal = total;
    return total;
}

void exibeGrupo(const GrupoAventura *grupo, const CadastroPersonagens *cadastro) {
    const Personagem *personagem;
    int i;

    if (grupo == 0 || cadastro == 0 || grupo->quantidade == 0) {
        printf("Grupo vazio.\n");
        return;
    }
    printf("Grupo (%d/%d):\n", grupo->quantidade, MAX_PERSONAGENS_GRUPO);
    for (i = 0; i < grupo->quantidade; i++) {
        personagem = cadastro_buscar_por_id_const(cadastro, grupo->idsPersonagens[i]);
        if (personagem == 0) {
            printf("  ID %d | personagem removido do cadastro\n", grupo->idsPersonagens[i]);
        } else {
            printf("  ID %d | %s | %s %s | nivel %d | poder %d\n", personagem->id, personagem->nome,
                   raca_para_texto(personagem->raca), classe_para_texto(personagem->classe), personagem->nivel,
                   personagem_calcular_atributos_totais(personagem).poder);
        }
    }
    printf("Poder total do grupo: %d\n", grupo->poderTotal);
}
