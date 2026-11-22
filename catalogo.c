#include "catalogo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINHA_TAMANHO 256
#define NUM_TIPOS 12
#define NUM_EFEITOS 5
#define NUM_LOCAIS 4
#define NUM_MAOS 3

static const char *const NOMES_TIPO[NUM_TIPOS] = {
    "ELMO", "PEITORAL", "MANOPLAS", "CALCA", "BOTAS", "ANEL", "COLAR", "CINTO",
    "ARMA_UMA_MAO", "ARMA_DUAS_MAOS", "RUNA", "CONSUMIVEL"
};

static const char *const NOMES_EFEITO[NUM_EFEITOS] = {
    "NENHUM", "CURA", "BONUS", "VENENO", "SUSPEITA"
};

static const char *const NOMES_LOCAL[NUM_LOCAIS] = {
    "LOJA", "HOSPITAL", "CABANA", "FERREIRO"
};

static const char *const NOMES_MAO[NUM_MAOS] = {
    "QUALQUER", "DIREITA", "ESQUERDA"
};

int local_venda_eh_valido(LocalVenda local) {
    return local >= LOCAL_LOJA && local <= LOCAL_FERREIRO;
}

const char *local_venda_para_texto(LocalVenda local) {
    switch (local) {
        case LOCAL_LOJA: return "Loja de itens";
        case LOCAL_HOSPITAL: return "Hospital";
        case LOCAL_CABANA: return "Cabana escura";
        case LOCAL_FERREIRO: return "Ferreiro";
        default: return "Local desconhecido";
    }
}

void catalogo_inicializar(CatalogoItens *catalogo) {
    if (catalogo != 0) {
        catalogo->quantidade = 0;
        catalogo->rejeitados = 0;
        catalogo->linhaPrimeiroErro = 0;
    }
}

static char *aparar(char *texto) {
    char *fim;

    while (*texto == ' ' || *texto == '\t') {
        texto++;
    }
    fim = texto + strlen(texto);
    while (fim > texto && (fim[-1] == ' ' || fim[-1] == '\t' || fim[-1] == '\r' || fim[-1] == '\n')) {
        fim--;
    }
    *fim = '\0';
    return texto;
}

static int procura(const char *texto, const char *const nomes[], int quantidade) {
    int i;

    for (i = 0; i < quantidade; i++) {
        if (strcmp(texto, nomes[i]) == 0) {
            return i;
        }
    }
    return -1;
}

static int inteiro(const char *texto, int *valor) {
    char *fim;
    long numero;

    if (texto[0] == '\0') {
        return 0;
    }
    numero = strtol(texto, &fim, 10);
    if (*fim != '\0' || numero < -ITEM_PRECO_MAXIMO || numero > ITEM_PRECO_MAXIMO) {
        return 0;
    }
    *valor = (int)numero;
    return 1;
}

static int aplicaCampo(ItemCatalogo *entrada, const char *chave, const char *valor) {
    Item *item = &entrada->item;
    int indice;

    if (strcmp(chave, "id") == 0) {
        return inteiro(valor, &item->id);
    }
    if (strcmp(chave, "nome") == 0) {
        if (valor[0] == '\0' || strlen(valor) >= ITEM_NOME_TAMANHO) {
            return 0;
        }
        strcpy(item->nome, valor);
        return 1;
    }
    if (strcmp(chave, "tipo") == 0) {
        indice = procura(valor, NOMES_TIPO, NUM_TIPOS);
        item->tipo = (TipoItem)(indice + 1);
        return indice >= 0;
    }
    if (strcmp(chave, "local") == 0) {
        indice = procura(valor, NOMES_LOCAL, NUM_LOCAIS);
        entrada->local = (LocalVenda)(indice + 1);
        return indice >= 0;
    }
    if (strcmp(chave, "efeito") == 0) {
        indice = procura(valor, NOMES_EFEITO, NUM_EFEITOS);
        item->efeito = (EfeitoItem)indice;
        return indice >= 0;
    }
    if (strcmp(chave, "mao") == 0) {
        indice = procura(valor, NOMES_MAO, NUM_MAOS);
        item->mao = (MaoPreferida)indice;
        return indice >= 0;
    }
    if (strcmp(chave, "dano") == 0 || strcmp(chave, "dado") == 0) {
        return converteTextoDado(valor, &item->dano);
    }
    if (strcmp(chave, "espacos") == 0) {
        return inteiro(valor, &item->espacos);
    }
    if (strcmp(chave, "ataque") == 0) {
        return inteiro(valor, &item->bonus_ataque);
    }
    if (strcmp(chave, "defesa") == 0) {
        return inteiro(valor, &item->bonus_defesa);
    }
    if (strcmp(chave, "vida") == 0) {
        return inteiro(valor, &item->bonus_vida);
    }
    if (strcmp(chave, "iniciativa") == 0) {
        return inteiro(valor, &item->bonus_iniciativa);
    }
    if (strcmp(chave, "poder") == 0) {
        return inteiro(valor, &item->poder);
    }
    if (strcmp(chave, "preco") == 0) {
        return inteiro(valor, &item->preco);
    }
    return 0;
}

static void registraErro(CatalogoItens *catalogo, int linha) {
    catalogo->rejeitados++;
    if (catalogo->linhaPrimeiroErro == 0) {
        catalogo->linhaPrimeiroErro = linha;
    }
}

int catalogo_carregar(CatalogoItens *catalogo, const char *nomeArquivo) {
    FILE *arquivo;
    char linha[LINHA_TAMANHO];
    char *texto;
    char *valor;
    ItemCatalogo atual;
    int numero = 0;
    int dentro = 0;
    int invalido = 0;
    int inicio = 0;

    if (catalogo == 0 || nomeArquivo == 0) {
        return CATALOGO_ERRO_ARQUIVO;
    }
    catalogo_inicializar(catalogo);
    arquivo = fopen(nomeArquivo, "r");
    if (arquivo == 0) {
        return CATALOGO_ERRO_ARQUIVO;
    }

    while (fgets(linha, sizeof(linha), arquivo) != 0) {
        numero++;
        texto = aparar(linha);
        if (texto[0] == '\0' || texto[0] == '#') {
            continue;
        }
        if (strcmp(texto, "[ITEM]") == 0) {
            if (dentro) {
                registraErro(catalogo, inicio);
            }
            memset(&atual, 0, sizeof(atual));
            dentro = 1;
            invalido = 0;
            inicio = numero;
        } else if (strcmp(texto, "[/ITEM]") == 0) {
            if (!dentro || invalido || !item_validar(&atual.item) || !local_venda_eh_valido(atual.local)
                || catalogo_buscar_por_id(catalogo, atual.item.id) != 0
                || catalogo->quantidade >= CATALOGO_MAX_ITENS) {
                registraErro(catalogo, dentro ? inicio : numero);
            } else {
                catalogo->itens[catalogo->quantidade++] = atual;
            }
            dentro = 0;
        } else {
            valor = strchr(texto, '=');
            if (!dentro || valor == 0) {
                if (dentro) {
                    invalido = 1;
                } else {
                    registraErro(catalogo, numero);
                }
                continue;
            }
            *valor++ = '\0';
            if (!aplicaCampo(&atual, aparar(texto), aparar(valor))) {
                invalido = 1;
            }
        }
    }
    if (dentro) {
        registraErro(catalogo, inicio);
    }
    fclose(arquivo);
    return 0;
}

const ItemCatalogo *catalogo_buscar_por_id(const CatalogoItens *catalogo, int id_item) {
    int i;

    if (catalogo == 0) {
        return 0;
    }
    for (i = 0; i < catalogo->quantidade; i++) {
        if (catalogo->itens[i].item.id == id_item) {
            return &catalogo->itens[i];
        }
    }
    return 0;
}

int catalogo_listar_local(const CatalogoItens *catalogo, LocalVenda local, const ItemCatalogo *saida[], int maximo) {
    int quantidade = 0;
    int i;

    if (catalogo == 0 || saida == 0) {
        return 0;
    }
    for (i = 0; i < catalogo->quantidade && quantidade < maximo; i++) {
        if (catalogo->itens[i].local == local) {
            saida[quantidade++] = &catalogo->itens[i];
        }
    }
    return quantidade;
}

ResultadoOperacao catalogo_comprar(const CatalogoItens *catalogo, int id_item, LocalVenda local,
                                   Personagem *comprador) {
    const ItemCatalogo *entrada = catalogo_buscar_por_id(catalogo, id_item);
    ResultadoOperacao resultado;
    Item novo;

    if (comprador == 0) {
        return RESULTADO_DADOS_INVALIDOS;
    }
    if (entrada == 0 || entrada->local != local) {
        return RESULTADO_ITEM_NAO_ENCONTRADO;
    }
    if (comprador->moedas < entrada->item.preco) {
        return RESULTADO_MOEDAS_INSUFICIENTES;
    }
    novo = entrada->item;
    novo.id = personagem_proximo_id_item(comprador);
    resultado = personagem_adicionar_item(comprador, novo);
    if (resultado != RESULTADO_OK) {
        return resultado;
    }
    return personagem_alterar_moedas(comprador, -entrada->item.preco);
}

void catalogo_exibir_item(const ItemCatalogo *entrada) {
    const Item *item;
    char dado[32] = "-";

    if (entrada == 0) {
        return;
    }
    item = &entrada->item;
    if (validaDado(item->dano)) {
        dadoParaTexto(item->dano, dado, sizeof(dado));
    }
    printf("%3d | %5d moedas | %s | %s", item->id, item->preco, item->nome, tipo_item_para_texto(item->tipo));
    if (item->tipo == TIPO_ITEM_CONSUMIVEL) {
        printf(" (%s)", efeito_item_para_texto(item->efeito));
    }
    printf(" | espacos %d | atk %+d def %+d vida %+d ini %+d | dado %s", item->espacos, item->bonus_ataque,
           item->bonus_defesa, item->bonus_vida, item->bonus_iniciativa, dado);
    if (item->mao != MAO_QUALQUER) {
        printf(" | %s", mao_preferida_para_texto(item->mao));
    }
    printf("\n");
}
