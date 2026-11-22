#include "persistencia.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINHA_TAMANHO 512
#define CAMPOS_MAXIMOS 16
#define CAMPOS_PERSONAGEM 13
#define CAMPOS_PERSONAGEM_SEM_XP 12
#define CAMPOS_PERSONAGEM_ANTIGO 11
#define CAMPOS_ITEM 13
#define CAMPOS_ITEM_SEM_MAO 12
#define CAMPOS_ITEM_ANTIGO 10
#define CAMPOS_BONUS 3
#define CAMPOS_HISTORIA 2

void inicializaEstadoSalvo(EstadoSalvo *estado) {
    if (estado != 0) {
        memset(estado, 0, sizeof(*estado));
    }
}

static int quantidadeItemValida(int quantidade) {
    return quantidade == CAMPOS_ITEM || quantidade == CAMPOS_ITEM_SEM_MAO || quantidade == CAMPOS_ITEM_ANTIGO;
}

static void copiaSemSeparador(char *destino, const char *origem, int tamanho) {
    int i;

    strncpy(destino, origem, (size_t)tamanho - 1);
    destino[tamanho - 1] = '\0';
    for (i = 0; destino[i] != '\0'; i++) {
        if (destino[i] == ';') {
            destino[i] = ',';
        }
    }
}

static void escreveItem(FILE *f, const char *rotulo, int slot, const Item *item) {
    char nome[ITEM_NOME_TAMANHO];
    char dano[32];

    copiaSemSeparador(nome, item->nome, sizeof(nome));
    if (validaDado(item->dano)) {
        dadoParaTexto(item->dano, dano, sizeof(dano));
    } else {
        strcpy(dano, "-");
    }
    fprintf(f, "%s=", rotulo);
    if (slot > 0) {
        fprintf(f, "%d;", slot);
    }
    fprintf(f, "%d;%s;%d;%d;%d;%d;%d;%d;%d;%s;%d;%d;%d\n", item->id, nome, (int)item->tipo, item->espacos,
            item->bonus_ataque, item->bonus_defesa, item->bonus_vida, item->bonus_iniciativa, item->poder, dano,
            item->preco, (int)item->efeito, (int)item->mao);
}

static void escreveEquipado(FILE *f, SlotEquipamento slot, const ItemEquipado *equipado) {
    if (equipado->ocupado) {
        escreveItem(f, "equipado", (int)slot, &equipado->item);
    }
}

static void escreveEstado(FILE *f, const EstadoSalvo *estado) {
    int i;

    if (estado == 0) {
        return;
    }
    if (estado->quantidadeGrupo > 0) {
        fprintf(f, "grupo=");
        for (i = 0; i < estado->quantidadeGrupo; i++) {
            fprintf(f, "%s%d", i > 0 ? ";" : "", estado->idsGrupo[i]);
        }
        fprintf(f, "\n");
    }
    if (estado->missoesPrincipais != 0 || estado->missoesSecundarias != 0) {
        fprintf(f, "historia=%d;%d\n", estado->missoesPrincipais, estado->missoesSecundarias);
    }
}

int salvaPersonagens(const CadastroPersonagens *cadastro, const char *nomeArquivo) {
    return salvaJogo(cadastro, 0, nomeArquivo);
}

int salvaJogo(const CadastroPersonagens *cadastro, const EstadoSalvo *estado, const char *nomeArquivo) {
    FILE *f;
    const Personagem *p;
    const Equipamentos *e;
    char nomePersonagem[PERSONAGEM_NOME_TAMANHO];
    int i;
    int j;

    if (cadastro == 0 || nomeArquivo == 0) {
        return PERSISTENCIA_ERRO_ARQUIVO;
    }
    f = fopen(nomeArquivo, "w");
    if (f == 0) {
        return PERSISTENCIA_ERRO_ARQUIVO;
    }
    for (i = 0; i < cadastro->quantidade; i++) {
        p = &cadastro->personagens[i];
        e = &p->equipamentos;
        copiaSemSeparador(nomePersonagem, p->nome, sizeof(nomePersonagem));
        fprintf(f, "personagem=%d;%s;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n", p->id, nomePersonagem, (int)p->raca,
                (int)p->classe, p->nivel, p->pontos_vida_maximos, p->pontos_vida_atuais, p->ataque,
                p->defesa, p->iniciativa, p->poder, p->moedas, p->experiencia);
        if (p->bonus_temporario_ataque != 0 || p->bonus_temporario_defesa != 0
            || p->bonus_temporario_iniciativa != 0) {
            fprintf(f, "bonus=%d;%d;%d\n", p->bonus_temporario_ataque, p->bonus_temporario_defesa,
                    p->bonus_temporario_iniciativa);
        }
        for (j = 0; j < p->inventario.quantidade; j++) {
            escreveItem(f, "item", 0, &p->inventario.itens[j]);
        }
        escreveEquipado(f, SLOT_ELMO, &e->elmo);
        escreveEquipado(f, SLOT_PEITORAL, &e->peitoral);
        escreveEquipado(f, SLOT_MANOPLAS, &e->manoplas);
        escreveEquipado(f, SLOT_CALCA, &e->calca);
        escreveEquipado(f, SLOT_BOTAS, &e->botas);
        escreveEquipado(f, SLOT_ANEL, &e->anel);
        escreveEquipado(f, SLOT_COLAR, &e->colar);
        escreveEquipado(f, SLOT_CINTO, &e->cinto);
        escreveEquipado(f, SLOT_MAO_DIREITA, &e->mao_direita);
        if (!e->arma_duas_maos_equipada) {
            escreveEquipado(f, SLOT_MAO_ESQUERDA, &e->mao_esquerda);
        }
        escreveEquipado(f, SLOT_RUNA, &e->runa);
    }
    escreveEstado(f, estado);
    return fclose(f) == 0 ? 0 : PERSISTENCIA_ERRO_ARQUIVO;
}

static int divideCampos(char *texto, char *campos[], int maximo) {
    int quantidade = 0;

    campos[quantidade++] = texto;
    while (*texto != '\0') {
        if (*texto == ';') {
            if (quantidade >= maximo) {
                return maximo + 1;
            }
            *texto = '\0';
            campos[quantidade++] = texto + 1;
        }
        texto++;
    }
    return quantidade;
}

static int inteiro(const char *texto, int *valor) {
    char *fim;
    long v;

    if (texto[0] == '\0') {
        return 0;
    }
    v = strtol(texto, &fim, 10);
    if (*fim != '\0' || v < -PERSONAGEM_MOEDAS_MAXIMO || v > PERSONAGEM_MOEDAS_MAXIMO) {
        return 0;
    }
    *valor = (int)v;
    return 1;
}

static int leItem(char *campos[], int quantidade, Item *item) {
    int tipo;
    int efeito = 0;
    int mao = 0;

    if (!quantidadeItemValida(quantidade) || strlen(campos[1]) >= ITEM_NOME_TAMANHO) {
        return 0;
    }
    memset(item, 0, sizeof(*item));
    strcpy(item->nome, campos[1]);
    if (!inteiro(campos[0], &item->id) || !inteiro(campos[2], &tipo) || !inteiro(campos[3], &item->espacos)
        || !inteiro(campos[4], &item->bonus_ataque) || !inteiro(campos[5], &item->bonus_defesa)
        || !inteiro(campos[6], &item->bonus_vida) || !inteiro(campos[7], &item->bonus_iniciativa)
        || !inteiro(campos[8], &item->poder)) {
        return 0;
    }
    item->tipo = (TipoItem)tipo;
    if (strcmp(campos[9], "-") != 0 && !converteTextoDado(campos[9], &item->dano)) {
        return 0;
    }
    if (quantidade >= CAMPOS_ITEM_SEM_MAO) {
        if (!inteiro(campos[10], &item->preco) || !inteiro(campos[11], &efeito)) {
            return 0;
        }
        item->efeito = (EfeitoItem)efeito;
    }
    if (quantidade == CAMPOS_ITEM) {
        if (!inteiro(campos[12], &mao)) {
            return 0;
        }
        item->mao = (MaoPreferida)mao;
    }
    return item_validar(item);
}

static int lePersonagem(char *campos[], int quantidade, Personagem *saida) {
    int v[CAMPOS_PERSONAGEM];
    int i;

    if (quantidade < CAMPOS_PERSONAGEM_ANTIGO || quantidade > CAMPOS_PERSONAGEM
        || strlen(campos[1]) >= PERSONAGEM_NOME_TAMANHO) {
        return 0;
    }
    v[CAMPOS_PERSONAGEM_SEM_XP - 1] = 0;
    v[CAMPOS_PERSONAGEM - 1] = 0;
    for (i = 0; i < quantidade; i++) {
        if (i != 1 && !inteiro(campos[i], &v[i])) {
            return 0;
        }
    }
    *saida = personagem_criar_ficha_basica(v[0], campos[1], (Raca)v[2], (Classe)v[3], v[4], v[5], v[6], v[7],
                                           v[8], v[9], v[10]);
    saida->moedas = v[11];
    saida->experiencia = v[12];
    return 1;
}

static int leGrupo(char *campos[], int quantidade, const CadastroPersonagens *cadastro, EstadoSalvo *estado) {
    int id;
    int i;
    int j;

    if (quantidade < 1 || quantidade > MAX_PERSONAGENS_GRUPO || estado->quantidadeGrupo != 0) {
        return 0;
    }
    for (i = 0; i < quantidade; i++) {
        if (!inteiro(campos[i], &id) || cadastro_buscar_indice_por_id(cadastro, id) < 0) {
            return 0;
        }
        for (j = 0; j < i; j++) {
            if (estado->idsGrupo[j] == id) {
                return 0;
            }
        }
        estado->idsGrupo[i] = id;
    }
    estado->quantidadeGrupo = quantidade;
    return 1;
}

static int leHistoria(char *campos[], int quantidade, EstadoSalvo *estado) {
    int principais;
    int secundarias;

    if (quantidade != CAMPOS_HISTORIA || !inteiro(campos[0], &principais) || !inteiro(campos[1], &secundarias)
        || principais < 0 || principais > PERSISTENCIA_PROGRESSO_MAXIMO || secundarias < 0) {
        return 0;
    }
    estado->missoesPrincipais = principais;
    estado->missoesSecundarias = secundarias;
    return 1;
}

static int leBonus(char *campos[], int quantidade, Personagem *personagem) {
    int v[CAMPOS_BONUS];
    int i;

    if (quantidade != CAMPOS_BONUS) {
        return 0;
    }
    for (i = 0; i < CAMPOS_BONUS; i++) {
        if (!inteiro(campos[i], &v[i]) || v[i] < -PERSONAGEM_PODER_MAXIMO || v[i] > PERSONAGEM_PODER_MAXIMO) {
            return 0;
        }
    }
    personagem->bonus_temporario_ataque = v[0];
    personagem->bonus_temporario_defesa = v[1];
    personagem->bonus_temporario_iniciativa = v[2];
    return 1;
}

static int processaLinha(CadastroPersonagens *cadastro, EstadoSalvo *estado, Personagem **atual, char *linha) {
    char *campos[CAMPOS_MAXIMOS];
    char *valor = strchr(linha, '=');
    Personagem novo;
    Item item;
    int quantidade;
    int slot;

    if (valor == 0) {
        return 0;
    }
    *valor++ = '\0';
    quantidade = divideCampos(valor, campos, CAMPOS_MAXIMOS);

    if (strcmp(linha, "personagem") == 0) {
        if (!lePersonagem(campos, quantidade, &novo) || cadastro_cadastrar(cadastro, novo) != RESULTADO_OK) {
            return 0;
        }
        *atual = &cadastro->personagens[cadastro->quantidade - 1];
        return 1;
    }
    if (strcmp(linha, "grupo") == 0) {
        return leGrupo(campos, quantidade, cadastro, estado);
    }
    if (strcmp(linha, "historia") == 0) {
        return leHistoria(campos, quantidade, estado);
    }
    if (*atual == 0) {
        return 0;
    }
    if (strcmp(linha, "bonus") == 0) {
        return leBonus(campos, quantidade, *atual);
    }
    if (strcmp(linha, "item") == 0) {
        return leItem(campos, quantidade, &item) && personagem_adicionar_item(*atual, item) == RESULTADO_OK;
    }
    if (strcmp(linha, "equipado") == 0 && quantidadeItemValida(quantidade - 1)) {
        return inteiro(campos[0], &slot) && leItem(campos + 1, quantidade - 1, &item)
               && personagem_adicionar_item(*atual, item) == RESULTADO_OK
               && personagem_equipar_item(*atual, item.id, (SlotEquipamento)slot) == RESULTADO_OK;
    }
    return 0;
}

int carregaPersonagens(CadastroPersonagens *cadastro, const char *nomeArquivo) {
    return carregaJogo(cadastro, 0, nomeArquivo);
}

int carregaJogo(CadastroPersonagens *cadastro, EstadoSalvo *estado, const char *nomeArquivo) {
    CadastroPersonagens *temporario;
    EstadoSalvo lido;
    FILE *f;
    char linha[LINHA_TAMANHO];
    Personagem *atual = 0;
    int numero = 0;
    int erro = 0;

    if (cadastro == 0 || nomeArquivo == 0) {
        return PERSISTENCIA_ERRO_ARQUIVO;
    }
    f = fopen(nomeArquivo, "r");
    if (f == 0) {
        return PERSISTENCIA_ERRO_ARQUIVO;
    }
    temporario = malloc(sizeof(*temporario));
    if (temporario == 0) {
        fclose(f);
        return PERSISTENCIA_ERRO_ARQUIVO;
    }
    cadastro_inicializar(temporario);
    inicializaEstadoSalvo(&lido);

    while (!erro && fgets(linha, sizeof(linha), f) != 0) {
        numero++;
        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] != '\0' && !processaLinha(temporario, &lido, &atual, linha)) {
            erro = numero;
        }
    }
    fclose(f);

    if (!erro) {
        *cadastro = *temporario;
        if (estado != 0) {
            *estado = lido;
        }
    }
    free(temporario);
    return erro;
}
