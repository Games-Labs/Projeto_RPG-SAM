#include "historia.h"

#include "tela.h"

#include <stdio.h>
#include <string.h>

#define LINHA_TAMANHO 1024
#define MARCA_SECAO '@'
#define MARCA_COMENTARIO '#'

typedef struct {
    int pontos_vida;
    int ataque;
    int defesa;
    int iniciativa;
    int poder;
    const char *arma;
    Dado dano;
} BaseClasse;

static const Dado DADO_DESCANSO = {1, 8, 0};

static const Missao MISSOES_PRINCIPAIS[HISTORIA_NUM_PRINCIPAIS] = {
    {"Os desaparecidos da estrada", TERRENO_PLANICIE, FACIL, 30, 40},
    {"Segredos sob as ruinas", TERRENO_RUINAS, MEDIA, 50, 80},
    {"O pacto nas sombras", TERRENO_URBANO, MEDIA, 70, 120},
    {"O despertar da magia antiga", TERRENO_CAVERNA, DIFICIL, 120, 200}
};

static const Missao MISSOES_SECUNDARIAS[HISTORIA_NUM_SECUNDARIAS] = {
    {"A carga perdida do comerciante", TERRENO_FLORESTA, FACIL, 20, 25},
    {"O eremita do pantano", TERRENO_PANTANO, MEDIA, 35, 50},
    {"Ecos na montanha", TERRENO_MONTANHA, MEDIA, 40, 60}
};

void historia_inicializa_progresso(ProgressoHistoria *progresso) {
    if (progresso != 0) {
        progresso->principais = 0;
        progresso->secundarias = 0;
    }
}

void historia_define_progresso(ProgressoHistoria *progresso, int principais, int secundarias) {
    if (progresso == 0) {
        return;
    }
    if (principais < 0) {
        principais = 0;
    }
    if (principais > HISTORIA_NUM_PRINCIPAIS) {
        principais = HISTORIA_NUM_PRINCIPAIS;
    }
    if (secundarias < 0) {
        secundarias = 0;
    }
    progresso->principais = principais;
    progresso->secundarias = secundarias & ((1 << HISTORIA_NUM_SECUNDARIAS) - 1);
}

const Missao *historia_missao_principal(int numero) {
    if (numero < 1 || numero > HISTORIA_NUM_PRINCIPAIS) {
        return 0;
    }
    return &MISSOES_PRINCIPAIS[numero - 1];
}

const Missao *historia_missao_secundaria(int numero) {
    if (numero < 1 || numero > HISTORIA_NUM_SECUNDARIAS) {
        return 0;
    }
    return &MISSOES_SECUNDARIAS[numero - 1];
}

int historia_capitulos_liberados(const ProgressoHistoria *progresso) {
    if (progresso == 0 || progresso->principais < 0) {
        return 0;
    }
    return progresso->principais > HISTORIA_NUM_CAPITULOS ? HISTORIA_NUM_CAPITULOS : progresso->principais;
}

int historia_secundaria_concluida(const ProgressoHistoria *progresso, int numero) {
    if (progresso == 0 || numero < 1 || numero > HISTORIA_NUM_SECUNDARIAS) {
        return 0;
    }
    return (progresso->secundarias >> (numero - 1)) & 1;
}

int historia_conta_secundarias(const ProgressoHistoria *progresso) {
    int total = 0;
    int i;

    for (i = 1; i <= HISTORIA_NUM_SECUNDARIAS; i++) {
        total += historia_secundaria_concluida(progresso, i);
    }
    return total;
}

int historia_conclui_principal(ProgressoHistoria *progresso, int numero) {
    if (progresso == 0 || numero != progresso->principais + 1 || numero > HISTORIA_NUM_PRINCIPAIS) {
        return 0;
    }
    progresso->principais = numero;
    return 1;
}

int historia_conclui_secundaria(ProgressoHistoria *progresso, int numero) {
    if (progresso == 0 || numero < 1 || numero > HISTORIA_NUM_SECUNDARIAS
        || historia_secundaria_concluida(progresso, numero)) {
        return 0;
    }
    progresso->secundarias |= 1 << (numero - 1);
    return 1;
}

int historia_recompensa_grupo(CadastroPersonagens *cadastro, const GrupoAventura *grupo, const Missao *missao,
                              int *niveis) {
    Personagem *personagem;
    int premiados = 0;
    int ganhos = 0;
    int moedas;
    int i;

    if (cadastro == 0 || grupo == 0 || missao == 0) {
        return 0;
    }
    for (i = 0; i < grupo->quantidade; i++) {
        personagem = cadastro_buscar_por_id(cadastro, grupo->idsPersonagens[i]);
        if (personagem == 0) {
            continue;
        }
        moedas = missao->moedas;
        if (moedas > PERSONAGEM_MOEDAS_MAXIMO - personagem->moedas) {
            moedas = PERSONAGEM_MOEDAS_MAXIMO - personagem->moedas;
        }
        personagem_alterar_moedas(personagem, moedas);
        ganhos += personagem_ganhar_experiencia(personagem, missao->experiencia);
        premiados++;
    }
    if (niveis != 0) {
        *niveis = ganhos;
    }
    return premiados;
}

static BaseClasse base_da_classe(Classe classe) {
    BaseClasse base;

    switch (classe) {
        case CLASSE_GUERREIRO:
            base.pontos_vida = 30; base.ataque = 4; base.defesa = 14; base.iniciativa = 1; base.poder = 12;
            base.arma = "Espada de recruta"; base.dano.quantidade = 1; base.dano.faces = 8;
            break;
        case CLASSE_LADINO:
            base.pontos_vida = 22; base.ataque = 3; base.defesa = 13; base.iniciativa = 4; base.poder = 10;
            base.arma = "Adaga afiada"; base.dano.quantidade = 1; base.dano.faces = 6;
            break;
        case CLASSE_MAGO:
            base.pontos_vida = 16; base.ataque = 3; base.defesa = 11; base.iniciativa = 2; base.poder = 10;
            base.arma = "Cajado de aprendiz"; base.dano.quantidade = 1; base.dano.faces = 6;
            break;
        case CLASSE_CLERIGO:
            base.pontos_vida = 24; base.ataque = 3; base.defesa = 13; base.iniciativa = 0; base.poder = 10;
            base.arma = "Maca de madeira"; base.dano.quantidade = 1; base.dano.faces = 6;
            break;
        default:
            base.pontos_vida = 20; base.ataque = 2; base.defesa = 12; base.iniciativa = 3; base.poder = 9;
            base.arma = "Florete simples"; base.dano.quantidade = 1; base.dano.faces = 6;
            break;
    }
    base.dano.modificador = 0;
    return base;
}

Personagem historia_cria_heroi(int id, const char *nome, Raca raca, Classe classe) {
    BaseClasse base = base_da_classe(classe);
    Personagem heroi = personagem_criar_ficha_basica(id, nome, raca, classe, PERSONAGEM_NIVEL_MINIMO,
                                                     base.pontos_vida, base.pontos_vida, base.ataque, base.defesa,
                                                     base.iniciativa, base.poder);
    Item arma;

    heroi.moedas = HISTORIA_MOEDAS_INICIAIS;
    memset(&arma, 0, sizeof(arma));
    arma.id = 1;
    strncpy(arma.nome, base.arma, ITEM_NOME_TAMANHO - 1);
    arma.tipo = TIPO_ITEM_ARMA_UMA_MAO;
    arma.espacos = 2;
    arma.poder = 1;
    arma.dano = base.dano;
    if (personagem_adicionar_item(&heroi, arma) == RESULTADO_OK) {
        personagem_equipar_item(&heroi, arma.id, SLOT_MAO_DIREITA);
    }
    return heroi;
}

int historia_proximo_id(const CadastroPersonagens *cadastro) {
    int maior = 0;
    int i;

    if (cadastro == 0) {
        return 1;
    }
    for (i = 0; i < cadastro->quantidade; i++) {
        if (cadastro->personagens[i].id > maior) {
            maior = cadastro->personagens[i].id;
        }
    }
    return maior + 1;
}

int historia_descansa_grupo(CadastroPersonagens *cadastro, const GrupoAventura *grupo) {
    Personagem *personagem;
    int total = 0;
    int pontos;
    int i;

    if (cadastro == 0 || grupo == 0) {
        return 0;
    }
    for (i = 0; i < grupo->quantidade; i++) {
        personagem = cadastro_buscar_por_id(cadastro, grupo->idsPersonagens[i]);
        if (personagem == 0) {
            continue;
        }
        pontos = rolaDado(DADO_DESCANSO);
        if (personagem->pontos_vida_atuais + pontos > personagem->pontos_vida_maximos) {
            pontos = personagem->pontos_vida_maximos - personagem->pontos_vida_atuais;
        }
        if (pontos > 0) {
            personagem->pontos_vida_atuais += pontos;
            total += pontos;
        }
    }
    return total;
}

static void remove_marcas(char *texto) {
    char *leitura = texto;
    char *escrita = texto;

    while (*leitura != '\0') {
        if (leitura[0] == '*' && leitura[1] == '*') {
            leitura += 2;
        } else {
            *escrita++ = *leitura++;
        }
    }
    *escrita = '\0';
}

int historia_exibe_secao(const char *nomeArquivo, const char *secao, int *pulado) {
    FILE *arquivo;
    char linha[LINHA_TAMANHO];
    int dentro = 0;
    int linhas = 0;
    int branco = 0;
    int pular = pulado != 0 && *pulado;

    if (nomeArquivo == 0 || secao == 0) {
        return HISTORIA_ERRO_ARQUIVO;
    }
    arquivo = fopen(nomeArquivo, "r");
    if (arquivo == 0) {
        return HISTORIA_ERRO_ARQUIVO;
    }
    while (fgets(linha, sizeof(linha), arquivo) != 0) {
        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] == MARCA_COMENTARIO) {
            continue;
        }
        if (linha[0] == MARCA_SECAO) {
            if (dentro) {
                break;
            }
            dentro = strcmp(linha + 1, secao) == 0;
            continue;
        }
        if (!dentro) {
            continue;
        }
        if (linha[0] == '\0') {
            branco = linhas > 0;
            continue;
        }
        if (branco) {
            printf("\n");
            if (!pular && tela_esperar(TELA_ATRASO_LINHA_MS)) {
                pular = 1;
            }
            branco = 0;
        }
        remove_marcas(linha);
        tela_digitar(linha, &pular);
        linhas++;
    }
    fclose(arquivo);
    if (pulado != 0) {
        *pulado = pular;
    }
    return linhas;
}
