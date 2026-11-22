#include "monstro.h"

#include <stdio.h>
#include <string.h>

static const char *const TABELA_FREQUENCIA[] = {"MUITO_RARO", "RARO", "INCOMUM", "COMUM", "MUITO_COMUM"};
static const char *const TABELA_CLIMA[] = {"QUALQUER", "TEMPERADO", "TROPICAL", "FRIO", "ARIDO"};
static const char *const TABELA_TERRENO[] = {"CAVERNA", "RUINAS", "FLORESTA", "MONTANHA", "PLANICIE", "PANTANO", "URBANO", "DESERTO"};
static const char *const TABELA_ATIVIDADE[] = {"DIURNO", "NOTURNO", "QUALQUER_HORARIO"};
static const char *const TABELA_DIETA[] = {"CARNIVORO", "HERBIVORO", "ONIVORO", "PARASITA", "NAO_SE_ALIMENTA"};
static const char *const TABELA_MOVIMENTO[] = {"TERRESTRE", "VOO", "NATACAO", "ESCAVACAO"};
static const char *const TABELA_MANOBRABILIDADE[] = {"NENHUMA", "A", "B", "C", "D", "E"};
static const char *const TABELA_TAMANHO[] = {"PEQUENO", "MEDIO", "GRANDE", "ENORME", "GIGANTESCO"};
static const char *const TABELA_ORDEM[] = {"LEAL", "NEUTRO_ORDEM", "CAOTICO"};
static const char *const TABELA_MORAL[] = {"BOM", "NEUTRO_MORAL", "MAU"};
static const char *const TABELA_ESPECIAL[] = {
    "ABRACO", "AGARRAR", "ARMADILHA", "ARQUEARIA", "ARREMESSAR_PEDRA", "CAMUFLAGEM", "CANTO",
    "CANTO_ABISSAL", "CARAPACA", "CORROSAO", "DERRUBAR", "DISSOLVER", "DOENCA", "DRENAR_FORCA",
    "DRENAR_SANGUE", "DRENO_NIVEL", "ECOLOCALIZACAO", "EMBOSCADA", "ENCANTO", "ENVOLVER",
    "ESCALADA", "FORMACAO", "FURIA", "ILUSAO", "IMATERIAL", "IMUNIDADE", "INCENDIAR", "INVESTIDA",
    "INVISIBILIDADE", "LINGUA", "MAGIA", "MEDO", "MERGULHO", "MUSICA", "ODOR", "ORIENTACAO",
    "PARALISIA", "PASSO_ARB\303\223REO", "PELE_PEDRA", "PETRIFICACAO", "RASTREAR", "REDEMOINHO",
    "REGENERACAO", "RESISTENCIA_MAGICA", "RESPIRACAO_AQUATICA", "SOPRO", "TEIA", "TENACIDADE",
    "TRANSPARENTE", "TREMOR", "VENENO", "VISAO_AGUCADA"
};

#define DEFINE_ENUM(Tipo, prefixo, tabela, ultimo) \
    _Static_assert(sizeof(tabela) / sizeof(tabela[0]) == (size_t)(ultimo), "tabela de " #prefixo); \
    int prefixo##EhValido(Tipo valor) { \
        return (int)valor >= 1 && (int)valor <= (int)(ultimo); \
    } \
    const char *prefixo##ParaTexto(Tipo valor) { \
        return prefixo##EhValido(valor) ? tabela[(int)valor - 1] : "DESCONHECIDO"; \
    } \
    int prefixo##DeTexto(const char *texto, Tipo *valor) { \
        int i; \
        if (texto == 0 || valor == 0) { \
            return 0; \
        } \
        for (i = 0; i < (int)(ultimo); i++) { \
            if (strcmp(texto, tabela[i]) == 0) { \
                *valor = (Tipo)(i + 1); \
                return 1; \
            } \
        } \
        return 0; \
    }

DEFINE_ENUM(Frequencia, frequencia, TABELA_FREQUENCIA, FREQUENCIA_MUITO_COMUM)
DEFINE_ENUM(Clima, clima, TABELA_CLIMA, CLIMA_ARIDO)
DEFINE_ENUM(Terreno, terreno, TABELA_TERRENO, TERRENO_DESERTO)
DEFINE_ENUM(Atividade, atividade, TABELA_ATIVIDADE, ATIVIDADE_QUALQUER_HORARIO)
DEFINE_ENUM(Dieta, dieta, TABELA_DIETA, DIETA_NAO_SE_ALIMENTA)
DEFINE_ENUM(TipoMovimento, tipoMovimento, TABELA_MOVIMENTO, MOVIMENTO_ESCAVACAO)
DEFINE_ENUM(Manobrabilidade, manobrabilidade, TABELA_MANOBRABILIDADE, MANOBRABILIDADE_E)
DEFINE_ENUM(Tamanho, tamanho, TABELA_TAMANHO, TAMANHO_GIGANTESCO)
DEFINE_ENUM(TipoEspecial, tipoEspecial, TABELA_ESPECIAL, ESPECIAL_VISAO_AGUCADA)
DEFINE_ENUM(EixoOrdem, eixoOrdem, TABELA_ORDEM, ORDEM_CAOTICO)
DEFINE_ENUM(EixoMoral, eixoMoral, TABELA_MORAL, MORAL_MAU)

static int recusa(char *motivo, int tamanho, const char *mensagem) {
    if (motivo != 0 && tamanho > 0) {
        snprintf(motivo, (size_t)tamanho, "%s", mensagem);
    }
    return 0;
}

static int emFaixa(int valor, int minimo, int maximo) {
    return valor >= minimo && valor <= maximo;
}

static int textoCabe(const char *texto, size_t capacidade) {
    size_t i;

    for (i = 0; i < capacidade; i++) {
        if (texto[i] == '\0') {
            return 1;
        }
    }
    return 0;
}

static int textoPreenchido(const char *texto, size_t capacidade) {
    return textoCabe(texto, capacidade) && texto[0] != '\0';
}

static int validaDescricoes(const Monstro *m, char *motivo, int tamanho) {
    if (!textoPreenchido(m->nome, sizeof(m->nome))) {
        return recusa(motivo, tamanho, "nome vazio ou acima da capacidade");
    }
    if (!textoCabe(m->descricao, sizeof(m->descricao))
        || !textoCabe(m->descricaoCombate, sizeof(m->descricaoCombate))
        || !textoCabe(m->habitatSociedade, sizeof(m->habitatSociedade))
        || !textoCabe(m->descricaoEcologia, sizeof(m->descricaoEcologia))) {
        return recusa(motivo, tamanho, "descricao acima da capacidade");
    }
    if (!textoPreenchido(m->organizacao, sizeof(m->organizacao))) {
        return recusa(motivo, tamanho, "organizacao vazia ou acima da capacidade");
    }
    if (!textoPreenchido(m->tesouro, sizeof(m->tesouro))) {
        return recusa(motivo, tamanho, "tesouro vazio ou acima da capacidade");
    }
    if (!textoPreenchido(m->classificacaoMoral, sizeof(m->classificacaoMoral))) {
        return recusa(motivo, tamanho, "classificacaoMoral vazia ou acima da capacidade");
    }
    return 1;
}

static int validaEcologia(const Monstro *m, char *motivo, int tamanho) {
    int i;
    int j;

    if (!frequenciaEhValido(m->frequencia)) {
        return recusa(motivo, tamanho, "frequencia invalida");
    }
    if (!validaDado(m->numeroEncontrado)) {
        return recusa(motivo, tamanho, "numeroEncontrado invalido");
    }
    if (m->minimoCovil < 0 || m->maximoCovil < m->minimoCovil) {
        return recusa(motivo, tamanho, "covil deve ser uma faixa ordenada e nao negativa");
    }
    if (!emFaixa(m->quantidadeClimas, 1, MAX_CLIMAS)) {
        return recusa(motivo, tamanho, "quantidade de climas fora dos limites");
    }
    for (i = 0; i < m->quantidadeClimas; i++) {
        if (!climaEhValido(m->climas[i])) {
            return recusa(motivo, tamanho, "clima invalido");
        }
    }
    if (!emFaixa(m->quantidadeTerrenos, 1, MAX_TERRENOS)) {
        return recusa(motivo, tamanho, "quantidade de terrenos fora dos limites");
    }
    for (i = 0; i < m->quantidadeTerrenos; i++) {
        if (!terrenoEhValido(m->terrenos[i])) {
            return recusa(motivo, tamanho, "terreno invalido");
        }
        for (j = 0; j < i; j++) {
            if (m->terrenos[j] == m->terrenos[i]) {
                return recusa(motivo, tamanho, "terreno repetido");
            }
        }
    }
    if (!atividadeEhValido(m->atividade) || !dietaEhValido(m->dieta)) {
        return recusa(motivo, tamanho, "atividade ou dieta invalida");
    }
    if (!emFaixa(m->inteligencia, INTELIGENCIA_MINIMA, INTELIGENCIA_MAXIMA)) {
        return recusa(motivo, tamanho, "inteligencia fora da faixa");
    }
    if (!eixoOrdemEhValido(m->tendencia.ordem) || !eixoMoralEhValido(m->tendencia.moral)) {
        return recusa(motivo, tamanho, "tendencia invalida");
    }
    return 1;
}

static int validaEspeciais(const HabilidadeEspecial *lista, int quantidade, char *motivo, int tamanho) {
    int i;

    if (!emFaixa(quantidade, 0, MAX_ESPECIAIS)) {
        return recusa(motivo, tamanho, "quantidade de habilidades especiais fora dos limites");
    }
    for (i = 0; i < quantidade; i++) {
        if (!tipoEspecialEhValido(lista[i].tipo) || !textoCabe(lista[i].descricao, sizeof(lista[i].descricao))) {
            return recusa(motivo, tamanho, "habilidade especial invalida");
        }
    }
    return 1;
}

static int validaCombate(const Monstro *m, char *motivo, int tamanho) {
    int i;

    if (!emFaixa(m->classeArmadura, CA_MINIMA, CA_MAXIMA)) {
        return recusa(motivo, tamanho, "ca fora da faixa");
    }
    if (!emFaixa(m->quantidadeMovimentos, 1, MAX_MOVIMENTOS)) {
        return recusa(motivo, tamanho, "quantidade de movimentos fora dos limites");
    }
    for (i = 0; i < m->quantidadeMovimentos; i++) {
        if (!tipoMovimentoEhValido(m->movimentos[i].tipo)
            || !manobrabilidadeEhValido(m->movimentos[i].manobrabilidade)
            || !emFaixa(m->movimentos[i].deslocamento, 1, DESLOCAMENTO_MAXIMO)) {
            return recusa(motivo, tamanho, "movimento invalido");
        }
    }
    if (!validaDado(m->dadosVida) || m->dadosVida.faces != DADOS_VIDA_FACES) {
        return recusa(motivo, tamanho, "dv deve usar dados de 8 faces");
    }
    if (!emFaixa(m->thac0, THAC0_MINIMO, THAC0_MAXIMO)) {
        return recusa(motivo, tamanho, "thac0 fora da faixa");
    }
    if (!emFaixa(m->quantidadeAtaques, 1, MAX_ATAQUES)) {
        return recusa(motivo, tamanho, "quantidade de ataques fora dos limites");
    }
    for (i = 0; i < m->quantidadeAtaques; i++) {
        if (!textoPreenchido(m->ataques[i].nome, sizeof(m->ataques[i].nome)) || !validaDado(m->ataques[i].dano)) {
            return recusa(motivo, tamanho, "ataque invalido");
        }
    }
    if (!validaEspeciais(m->ataquesEspeciais, m->quantidadeAtaquesEspeciais, motivo, tamanho)
        || !validaEspeciais(m->defesasEspeciais, m->quantidadeDefesasEspeciais, motivo, tamanho)) {
        return 0;
    }
    if (!emFaixa(m->resistenciaMagia, RESISTENCIA_MAGIA_MINIMA, RESISTENCIA_MAGIA_MAXIMA)) {
        return recusa(motivo, tamanho, "resistenciaMagia fora da faixa 0..100");
    }
    if (!tamanhoEhValido(m->tamanho)) {
        return recusa(motivo, tamanho, "tamanho invalido");
    }
    if (m->alturaMinima < 0.0f || m->alturaMaxima < m->alturaMinima || m->alturaMaxima > ALTURA_MAXIMA) {
        return recusa(motivo, tamanho, "altura invalida");
    }
    if (!emFaixa(m->moral, MORAL_MINIMA, MORAL_MAXIMA)) {
        return recusa(motivo, tamanho, "moral fora da faixa");
    }
    if (!emFaixa(m->valorXP, XP_MINIMO, XP_MAXIMO)) {
        return recusa(motivo, tamanho, "xp fora da faixa");
    }
    return 1;
}

static int validaPoder(const Monstro *m, char *motivo, int tamanho) {
    const PoderEncontro *p = &m->poder;
    int i;
    int j;

    if (!emFaixa(p->poderBase, PODER_BASE_MINIMO, PODER_BASE_MAXIMO)) {
        return recusa(motivo, tamanho, "poderBase fora da faixa");
    }
    if (!emFaixa(p->modDia, -MODIFICADOR_PODER_MAXIMO, MODIFICADOR_PODER_MAXIMO)
        || !emFaixa(p->modNoite, -MODIFICADOR_PODER_MAXIMO, MODIFICADOR_PODER_MAXIMO)) {
        return recusa(motivo, tamanho, "modDia ou modNoite fora da faixa");
    }
    if (!emFaixa(p->quantidadeModTerreno, 0, MAX_TERRENOS)) {
        return recusa(motivo, tamanho, "quantidade de modTerreno fora dos limites");
    }
    for (i = 0; i < p->quantidadeModTerreno; i++) {
        if (!emFaixa(p->modTerreno[i].modificador, -MODIFICADOR_PODER_MAXIMO, MODIFICADOR_PODER_MAXIMO)
            || !viveNoTerreno(m, p->modTerreno[i].terreno)) {
            return recusa(motivo, tamanho, "modTerreno invalido ou de terreno nao habitado");
        }
        for (j = 0; j < i; j++) {
            if (p->modTerreno[j].terreno == p->modTerreno[i].terreno) {
                return recusa(motivo, tamanho, "modTerreno repetido");
            }
        }
    }
    if (!emFaixa(p->bonusPorAdicional, 0, BONUS_PODER_MAXIMO) || !emFaixa(p->bonusGrupoGrande, 0, BONUS_PODER_MAXIMO)) {
        return recusa(motivo, tamanho, "bonus de poder fora da faixa");
    }
    if (p->limiteGrupoGrande != 0 && !emFaixa(p->limiteGrupoGrande, 2, LIMITE_GRUPO_GRANDE_MAXIMO)) {
        return recusa(motivo, tamanho, "limiteGrupoGrande deve ser 0 ou estar entre 2 e 50");
    }
    return 1;
}

int validaMonstroMotivo(const Monstro *m, char *motivo, int tamanho) {
    if (m == 0) {
        return recusa(motivo, tamanho, "monstro nulo");
    }
    if (m->id <= 0) {
        return recusa(motivo, tamanho, "id deve ser positivo");
    }
    return validaDescricoes(m, motivo, tamanho)
        && validaEcologia(m, motivo, tamanho)
        && validaCombate(m, motivo, tamanho)
        && validaPoder(m, motivo, tamanho);
}

int validaMonstro(const Monstro *m) {
    return validaMonstroMotivo(m, 0, 0);
}

int viveNoTerreno(const Monstro *m, Terreno terreno) {
    int i;

    if (m == 0) {
        return 0;
    }
    for (i = 0; i < m->quantidadeTerrenos && i < MAX_TERRENOS; i++) {
        if (m->terrenos[i] == terreno) {
            return 1;
        }
    }
    return 0;
}

int ativoNoPeriodo(const Monstro *m, Atividade periodo) {
    if (m == 0 || !atividadeEhValido(periodo)) {
        return 0;
    }
    return m->atividade == ATIVIDADE_QUALQUER_HORARIO
        || periodo == ATIVIDADE_QUALQUER_HORARIO
        || m->atividade == periodo;
}

static void exibeEspeciais(const char *titulo, const HabilidadeEspecial *lista, int quantidade) {
    int i;

    printf("%s:", titulo);
    if (quantidade == 0) {
        printf(" nenhuma\n");
        return;
    }
    printf("\n");
    for (i = 0; i < quantidade; i++) {
        printf("  - %s (%d): %s\n", tipoEspecialParaTexto(lista[i].tipo), lista[i].valor, lista[i].descricao);
    }
}

void exibeMonstro(const Monstro *m) {
    char dado[32];
    int i;

    if (m == 0) {
        return;
    }

    printf("\n[%d] %s\n", m->id, m->nome);
    printf("%s\n", m->descricao);
    dadoParaTexto(m->numeroEncontrado, dado, sizeof(dado));
    printf("Frequencia: %s | Numero encontrado: %s | Covil: %d a %d\n",
           frequenciaParaTexto(m->frequencia), dado, m->minimoCovil, m->maximoCovil);
    printf("Climas:");
    for (i = 0; i < m->quantidadeClimas; i++) {
        printf(" %s", climaParaTexto(m->climas[i]));
    }
    printf("\nTerrenos:");
    for (i = 0; i < m->quantidadeTerrenos; i++) {
        printf(" %s", terrenoParaTexto(m->terrenos[i]));
    }
    printf("\nOrganizacao: %s | Atividade: %s | Dieta: %s | Inteligencia: %d\n",
           m->organizacao, atividadeParaTexto(m->atividade), dietaParaTexto(m->dieta), m->inteligencia);
    printf("Tesouro: %s | Tendencia: %s / %s\n", m->tesouro,
           eixoOrdemParaTexto(m->tendencia.ordem), eixoMoralParaTexto(m->tendencia.moral));
    dadoParaTexto(m->dadosVida, dado, sizeof(dado));
    printf("CA: %d | DV: %s | THAC0: %d | Resistencia a magia: %d%%\n",
           m->classeArmadura, dado, m->thac0, m->resistenciaMagia);
    printf("Movimentos:");
    for (i = 0; i < m->quantidadeMovimentos; i++) {
        printf(" %s %d", tipoMovimentoParaTexto(m->movimentos[i].tipo), m->movimentos[i].deslocamento);
        if (m->movimentos[i].manobrabilidade != MANOBRABILIDADE_NENHUMA) {
            printf("(%s)", manobrabilidadeParaTexto(m->movimentos[i].manobrabilidade));
        }
    }
    printf("\nAtaques:");
    for (i = 0; i < m->quantidadeAtaques; i++) {
        dadoParaTexto(m->ataques[i].dano, dado, sizeof(dado));
        printf(" %s %s", m->ataques[i].nome, dado);
    }
    printf("\n");
    exibeEspeciais("Ataques especiais", m->ataquesEspeciais, m->quantidadeAtaquesEspeciais);
    exibeEspeciais("Defesas especiais", m->defesasEspeciais, m->quantidadeDefesasEspeciais);
    printf("Tamanho: %s (%.1f a %.1f m) | Moral: %d (%s) | XP: %d\n",
           tamanhoParaTexto(m->tamanho), m->alturaMinima, m->alturaMaxima,
           m->moral, m->classificacaoMoral, m->valorXP);
    printf("Poder base: %d | Dia: %+d | Noite: %+d | Por adicional: %d | Grupo grande (>= %d): +%d\n",
           m->poder.poderBase, m->poder.modDia, m->poder.modNoite, m->poder.bonusPorAdicional,
           m->poder.limiteGrupoGrande, m->poder.bonusGrupoGrande);
    printf("Modificadores de terreno:");
    if (m->poder.quantidadeModTerreno == 0) {
        printf(" nenhum");
    }
    for (i = 0; i < m->poder.quantidadeModTerreno; i++) {
        printf(" %s %+d", terrenoParaTexto(m->poder.modTerreno[i].terreno), m->poder.modTerreno[i].modificador);
    }
    printf("\nCombate: %s\nHabitat e sociedade: %s\nEcologia: %s\n",
           m->descricaoCombate, m->habitatSociedade, m->descricaoEcologia);
}
