#include "monstro.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_VALOR 1024
#define BUFFER_MOTIVO 200

typedef int (*FuncaoDefine)(Monstro *m, char *valor, char *erro, int tamanho);
typedef int (*FuncaoEscreve)(const Monstro *m, char *destino, int tamanho);

typedef struct {
    const char *chave;
    FuncaoDefine define;
    FuncaoEscreve escreve;
} CampoMonstro;

static int falha(char *erro, int tamanho, const char *formato, ...) {
    va_list argumentos;

    va_start(argumentos, formato);
    vsnprintf(erro, (size_t)tamanho, formato, argumentos);
    va_end(argumentos);
    return 0;
}

static int acrescenta(char *destino, int tamanho, const char *formato, ...) {
    va_list argumentos;
    int usado = (int)strlen(destino);
    int escrito;

    if (usado >= tamanho) {
        return 0;
    }
    va_start(argumentos, formato);
    escrito = vsnprintf(destino + usado, (size_t)(tamanho - usado), formato, argumentos);
    va_end(argumentos);
    return escrito >= 0 && escrito < tamanho - usado;
}

static char *aparar(char *texto) {
    char *fim;

    while (*texto == ' ' || *texto == '\t') {
        texto++;
    }
    fim = texto + strlen(texto);
    while (fim > texto && (fim[-1] == ' ' || fim[-1] == '\t')) {
        fim--;
    }
    *fim = '\0';
    return texto;
}

static int divide(char *texto, char separador, char *partes[], int maximo) {
    int quantidade = 0;
    char *atual = texto;
    char *corte;

    for (;;) {
        if (quantidade >= maximo) {
            return -1;
        }
        partes[quantidade++] = atual;
        corte = strchr(atual, separador);
        if (corte == 0) {
            return quantidade;
        }
        *corte = '\0';
        atual = corte + 1;
    }
}

static int lerInteiro(const char *texto, int minimo, int maximo, int *saida, char *erro, int tamanho) {
    char *fim;
    long valor;

    errno = 0;
    valor = strtol(texto, &fim, 10);
    if (texto[0] == '\0' || *fim != '\0' || errno != 0) {
        return falha(erro, tamanho, "'%s' nao e um inteiro", texto);
    }
    if (valor < minimo || valor > maximo) {
        return falha(erro, tamanho, "%ld fora da faixa %d..%d", valor, minimo, maximo);
    }
    *saida = (int)valor;
    return 1;
}

static int lerReal(const char *texto, float *saida, char *erro, int tamanho) {
    char *fim;
    double valor;

    errno = 0;
    valor = strtod(texto, &fim);
    if (texto[0] == '\0' || *fim != '\0' || errno != 0) {
        return falha(erro, tamanho, "'%s' nao e um numero", texto);
    }
    if (valor < 0.0 || valor > ALTURA_MAXIMA) {
        return falha(erro, tamanho, "%g fora da faixa 0..%g", valor, (double)ALTURA_MAXIMA);
    }
    *saida = (float)valor;
    return 1;
}

static int copiaTexto(const char *texto, char *destino, size_t capacidade, char *erro, int tamanho) {
    if (texto[0] == '\0') {
        return falha(erro, tamanho, "valor vazio");
    }
    if (strlen(texto) >= capacidade) {
        return falha(erro, tamanho, "texto excede %d caracteres", (int)capacidade - 1);
    }
    strcpy(destino, texto);
    return 1;
}

#define CAMPO_INTEIRO(nome, campo, minimo, maximo) \
    static int define_##nome(Monstro *m, char *v, char *e, int t) { \
        return lerInteiro(v, minimo, maximo, &m->campo, e, t); \
    } \
    static int escreve_##nome(const Monstro *m, char *d, int t) { \
        return snprintf(d, (size_t)t, "%d", m->campo) < t; \
    }

#define CAMPO_TEXTO(nome, campo) \
    static int define_##nome(Monstro *m, char *v, char *e, int t) { \
        return copiaTexto(v, m->campo, sizeof(m->campo), e, t); \
    } \
    static int escreve_##nome(const Monstro *m, char *d, int t) { \
        return snprintf(d, (size_t)t, "%s", m->campo) < t; \
    }

#define CAMPO_ENUM(nome, campo, prefixo) \
    static int define_##nome(Monstro *m, char *v, char *e, int t) { \
        if (!prefixo##DeTexto(v, &m->campo)) { \
            return falha(e, t, "valor '%s' nao reconhecido", v); \
        } \
        return 1; \
    } \
    static int escreve_##nome(const Monstro *m, char *d, int t) { \
        return snprintf(d, (size_t)t, "%s", prefixo##ParaTexto(m->campo)) < t; \
    }

#define CAMPO_DADO(nome, campo) \
    static int define_##nome(Monstro *m, char *v, char *e, int t) { \
        if (!converteTextoDado(v, &m->campo)) { \
            return falha(e, t, "expressao de dado invalida '%s'", v); \
        } \
        return 1; \
    } \
    static int escreve_##nome(const Monstro *m, char *d, int t) { \
        dadoParaTexto(m->campo, d, t); \
        return (int)strlen(d) < t - 1; \
    }

#define CAMPO_LISTA_ENUM(nome, campo, quantidade, maximo, Tipo, prefixo) \
    static int define_##nome(Monstro *m, char *v, char *e, int t) { \
        char *partes[maximo]; \
        int n = divide(v, ',', partes, maximo); \
        int i; \
        if (n < 0) { \
            return falha(e, t, "mais de %d itens", maximo); \
        } \
        for (i = 0; i < n; i++) { \
            if (!prefixo##DeTexto(aparar(partes[i]), &m->campo[i])) { \
                return falha(e, t, "valor '%s' nao reconhecido", partes[i]); \
            } \
        } \
        m->quantidade = n; \
        return 1; \
    } \
    static int escreve_##nome(const Monstro *m, char *d, int t) { \
        int i; \
        d[0] = '\0'; \
        for (i = 0; i < m->quantidade; i++) { \
            if (!acrescenta(d, t, "%s%s", i > 0 ? "," : "", prefixo##ParaTexto(m->campo[i]))) { \
                return 0; \
            } \
        } \
        return 1; \
    }

CAMPO_INTEIRO(id, id, 1, 2147483647)
CAMPO_TEXTO(nome, nome)
CAMPO_TEXTO(descricao, descricao)
CAMPO_ENUM(frequencia, frequencia, frequencia)
CAMPO_DADO(numeroEncontrado, numeroEncontrado)
CAMPO_LISTA_ENUM(climas, climas, quantidadeClimas, MAX_CLIMAS, Clima, clima)
CAMPO_LISTA_ENUM(terrenos, terrenos, quantidadeTerrenos, MAX_TERRENOS, Terreno, terreno)
CAMPO_TEXTO(organizacao, organizacao)
CAMPO_ENUM(atividade, atividade, atividade)
CAMPO_ENUM(dieta, dieta, dieta)
CAMPO_INTEIRO(inteligencia, inteligencia, INTELIGENCIA_MINIMA, INTELIGENCIA_MAXIMA)
CAMPO_TEXTO(tesouro, tesouro)
CAMPO_ENUM(ordem, tendencia.ordem, eixoOrdem)
CAMPO_ENUM(moralidade, tendencia.moral, eixoMoral)
CAMPO_INTEIRO(ca, classeArmadura, CA_MINIMA, CA_MAXIMA)
CAMPO_DADO(dv, dadosVida)
CAMPO_INTEIRO(thac0, thac0, THAC0_MINIMO, THAC0_MAXIMO)
CAMPO_INTEIRO(resistenciaMagia, resistenciaMagia, RESISTENCIA_MAGIA_MINIMA, RESISTENCIA_MAGIA_MAXIMA)
CAMPO_ENUM(tamanho, tamanho, tamanho)
CAMPO_INTEIRO(moral, moral, MORAL_MINIMA, MORAL_MAXIMA)
CAMPO_TEXTO(classificacaoMoral, classificacaoMoral)
CAMPO_INTEIRO(xp, valorXP, XP_MINIMO, XP_MAXIMO)
CAMPO_INTEIRO(poderBase, poder.poderBase, PODER_BASE_MINIMO, PODER_BASE_MAXIMO)
CAMPO_INTEIRO(modDia, poder.modDia, -MODIFICADOR_PODER_MAXIMO, MODIFICADOR_PODER_MAXIMO)
CAMPO_INTEIRO(modNoite, poder.modNoite, -MODIFICADOR_PODER_MAXIMO, MODIFICADOR_PODER_MAXIMO)
CAMPO_INTEIRO(bonusPorAdicional, poder.bonusPorAdicional, 0, BONUS_PODER_MAXIMO)
CAMPO_INTEIRO(limiteGrupoGrande, poder.limiteGrupoGrande, 0, LIMITE_GRUPO_GRANDE_MAXIMO)
CAMPO_INTEIRO(bonusGrupoGrande, poder.bonusGrupoGrande, 0, BONUS_PODER_MAXIMO)
CAMPO_TEXTO(descricaoCombate, descricaoCombate)
CAMPO_TEXTO(habitatSociedade, habitatSociedade)
CAMPO_TEXTO(descricaoEcologia, descricaoEcologia)

static int define_covil(Monstro *m, char *v, char *e, int t) {
    char *partes[2];
    int minimo;
    int maximo;

    if (divide(v, ',', partes, 2) != 2) {
        return falha(e, t, "esperado minimo,maximo");
    }
    if (!lerInteiro(aparar(partes[0]), 0, COVIL_MAXIMO, &minimo, e, t)
        || !lerInteiro(aparar(partes[1]), 0, COVIL_MAXIMO, &maximo, e, t)) {
        return 0;
    }
    if (maximo < minimo) {
        return falha(e, t, "maximo menor que o minimo");
    }
    m->minimoCovil = minimo;
    m->maximoCovil = maximo;
    return 1;
}

static int escreve_covil(const Monstro *m, char *d, int t) {
    return snprintf(d, (size_t)t, "%d,%d", m->minimoCovil, m->maximoCovil) < t;
}

static int define_altura(Monstro *m, char *v, char *e, int t) {
    char *partes[2];
    float minimo;
    float maximo;

    if (divide(v, ',', partes, 2) != 2) {
        return falha(e, t, "esperado minima,maxima");
    }
    if (!lerReal(aparar(partes[0]), &minimo, e, t) || !lerReal(aparar(partes[1]), &maximo, e, t)) {
        return 0;
    }
    if (maximo < minimo) {
        return falha(e, t, "altura maxima menor que a minima");
    }
    m->alturaMinima = minimo;
    m->alturaMaxima = maximo;
    return 1;
}

static int escreve_altura(const Monstro *m, char *d, int t) {
    return snprintf(d, (size_t)t, "%g,%g", (double)m->alturaMinima, (double)m->alturaMaxima) < t;
}

static int define_movimentos(Monstro *m, char *v, char *e, int t) {
    char *itens[MAX_MOVIMENTOS];
    char *campos[3];
    Movimento lista[MAX_MOVIMENTOS];
    int n = divide(v, '|', itens, MAX_MOVIMENTOS);
    int partes;
    int i;

    if (n < 0) {
        return falha(e, t, "mais de %d movimentos", MAX_MOVIMENTOS);
    }
    for (i = 0; i < n; i++) {
        partes = divide(itens[i], ':', campos, 3);
        if (partes < 2) {
            return falha(e, t, "movimento '%s' deve ser TIPO:deslocamento[:manobrabilidade]", itens[i]);
        }
        lista[i].manobrabilidade = MANOBRABILIDADE_NENHUMA;
        if (!tipoMovimentoDeTexto(aparar(campos[0]), &lista[i].tipo)) {
            return falha(e, t, "tipo de movimento '%s' nao reconhecido", campos[0]);
        }
        if (!lerInteiro(aparar(campos[1]), 1, DESLOCAMENTO_MAXIMO, &lista[i].deslocamento, e, t)) {
            return 0;
        }
        if (partes == 3) {
            if (lista[i].tipo != MOVIMENTO_VOO || !manobrabilidadeDeTexto(aparar(campos[2]), &lista[i].manobrabilidade)) {
                return falha(e, t, "manobrabilidade '%s' invalida", campos[2]);
            }
        }
    }
    memcpy(m->movimentos, lista, sizeof(Movimento) * (size_t)n);
    m->quantidadeMovimentos = n;
    return 1;
}

static int escreve_movimentos(const Movimento *lista, int quantidade, char *d, int t) {
    int i;

    d[0] = '\0';
    for (i = 0; i < quantidade; i++) {
        if (!acrescenta(d, t, "%s%s:%d", i > 0 ? "|" : "", tipoMovimentoParaTexto(lista[i].tipo), lista[i].deslocamento)) {
            return 0;
        }
        if (lista[i].manobrabilidade != MANOBRABILIDADE_NENHUMA
            && !acrescenta(d, t, ":%s", manobrabilidadeParaTexto(lista[i].manobrabilidade))) {
            return 0;
        }
    }
    return 1;
}

static int escreve_movimentos_campo(const Monstro *m, char *d, int t) {
    return escreve_movimentos(m->movimentos, m->quantidadeMovimentos, d, t);
}

static int define_ataques(Monstro *m, char *v, char *e, int t) {
    char *itens[MAX_ATAQUES];
    char *campos[2];
    Ataque lista[MAX_ATAQUES];
    int n = divide(v, '|', itens, MAX_ATAQUES);
    int i;
    char *nome;

    if (n < 0) {
        return falha(e, t, "mais de %d ataques", MAX_ATAQUES);
    }
    for (i = 0; i < n; i++) {
        if (divide(itens[i], ':', campos, 2) != 2) {
            return falha(e, t, "ataque '%s' deve ser NOME:dado", itens[i]);
        }
        nome = aparar(campos[0]);
        if (nome[0] == '\0' || strlen(nome) >= ATAQUE_NOME_TAMANHO) {
            return falha(e, t, "nome de ataque vazio ou longo demais");
        }
        strcpy(lista[i].nome, nome);
        if (!converteTextoDado(aparar(campos[1]), &lista[i].dano)) {
            return falha(e, t, "dano '%s' invalido", campos[1]);
        }
    }
    memcpy(m->ataques, lista, sizeof(Ataque) * (size_t)n);
    m->quantidadeAtaques = n;
    return 1;
}

static int escreve_ataques(const Monstro *m, char *d, int t) {
    char dado[32];
    int i;

    d[0] = '\0';
    for (i = 0; i < m->quantidadeAtaques; i++) {
        dadoParaTexto(m->ataques[i].dano, dado, sizeof(dado));
        if (!acrescenta(d, t, "%s%s:%s", i > 0 ? "|" : "", m->ataques[i].nome, dado)) {
            return 0;
        }
    }
    return 1;
}

static int defineEspeciais(HabilidadeEspecial *destino, int *quantidade, char *v, char *e, int t) {
    char *itens[MAX_ESPECIAIS];
    HabilidadeEspecial lista[MAX_ESPECIAIS];
    char *segundo;
    char *terceiro;
    int n;
    int i;

    if (strcmp(v, "NENHUM") == 0 || strcmp(v, "NENHUMA") == 0) {
        *quantidade = 0;
        return 1;
    }
    n = divide(v, '|', itens, MAX_ESPECIAIS);
    if (n < 0) {
        return falha(e, t, "mais de %d habilidades", MAX_ESPECIAIS);
    }
    for (i = 0; i < n; i++) {
        segundo = strchr(itens[i], ':');
        if (segundo == 0) {
            return falha(e, t, "habilidade '%s' deve ser TIPO:valor:descricao", itens[i]);
        }
        *segundo++ = '\0';
        terceiro = strchr(segundo, ':');
        if (terceiro != 0) {
            *terceiro++ = '\0';
        }
        if (!tipoEspecialDeTexto(aparar(itens[i]), &lista[i].tipo)) {
            return falha(e, t, "tipo de habilidade '%s' nao reconhecido", itens[i]);
        }
        if (!lerInteiro(aparar(segundo), 0, 1000, &lista[i].valor, e, t)) {
            return 0;
        }
        lista[i].descricao[0] = '\0';
        if (terceiro != 0) {
            terceiro = aparar(terceiro);
            if (strlen(terceiro) >= ESPECIAL_DESCRICAO_TAMANHO) {
                return falha(e, t, "descricao de habilidade longa demais");
            }
            strcpy(lista[i].descricao, terceiro);
        }
    }
    memcpy(destino, lista, sizeof(HabilidadeEspecial) * (size_t)n);
    *quantidade = n;
    return 1;
}

static int escreveEspeciais(const HabilidadeEspecial *lista, int quantidade, const char *vazio, char *d, int t) {
    int i;

    d[0] = '\0';
    if (quantidade == 0) {
        return snprintf(d, (size_t)t, "%s", vazio) < t;
    }
    for (i = 0; i < quantidade; i++) {
        if (!acrescenta(d, t, "%s%s:%d:%s", i > 0 ? "|" : "", tipoEspecialParaTexto(lista[i].tipo),
                        lista[i].valor, lista[i].descricao)) {
            return 0;
        }
    }
    return 1;
}

static int define_ataquesEspeciais(Monstro *m, char *v, char *e, int t) {
    return defineEspeciais(m->ataquesEspeciais, &m->quantidadeAtaquesEspeciais, v, e, t);
}

static int escreve_ataquesEspeciais(const Monstro *m, char *d, int t) {
    return escreveEspeciais(m->ataquesEspeciais, m->quantidadeAtaquesEspeciais, "NENHUM", d, t);
}

static int define_defesasEspeciais(Monstro *m, char *v, char *e, int t) {
    return defineEspeciais(m->defesasEspeciais, &m->quantidadeDefesasEspeciais, v, e, t);
}

static int escreve_defesasEspeciais(const Monstro *m, char *d, int t) {
    return escreveEspeciais(m->defesasEspeciais, m->quantidadeDefesasEspeciais, "NENHUMA", d, t);
}

static int define_modTerreno(Monstro *m, char *v, char *e, int t) {
    char *itens[MAX_TERRENOS];
    char *campos[2];
    ModificadorTerreno lista[MAX_TERRENOS];
    int n;
    int i;

    if (strcmp(v, "NENHUM") == 0 || strcmp(v, "NENHUMA") == 0) {
        m->poder.quantidadeModTerreno = 0;
        return 1;
    }
    n = divide(v, ',', itens, MAX_TERRENOS);
    if (n < 0) {
        return falha(e, t, "mais de %d modificadores", MAX_TERRENOS);
    }
    for (i = 0; i < n; i++) {
        if (divide(itens[i], ':', campos, 2) != 2) {
            return falha(e, t, "modificador '%s' deve ser TERRENO:valor", itens[i]);
        }
        if (!terrenoDeTexto(aparar(campos[0]), &lista[i].terreno)) {
            return falha(e, t, "terreno '%s' nao reconhecido", campos[0]);
        }
        if (!lerInteiro(aparar(campos[1]), -MODIFICADOR_PODER_MAXIMO, MODIFICADOR_PODER_MAXIMO,
                        &lista[i].modificador, e, t)) {
            return 0;
        }
    }
    memcpy(m->poder.modTerreno, lista, sizeof(ModificadorTerreno) * (size_t)n);
    m->poder.quantidadeModTerreno = n;
    return 1;
}

static int escreve_modTerreno(const Monstro *m, char *d, int t) {
    int i;

    d[0] = '\0';
    if (m->poder.quantidadeModTerreno == 0) {
        return snprintf(d, (size_t)t, "NENHUM") < t;
    }
    for (i = 0; i < m->poder.quantidadeModTerreno; i++) {
        if (!acrescenta(d, t, "%s%s:%d", i > 0 ? "," : "",
                        terrenoParaTexto(m->poder.modTerreno[i].terreno), m->poder.modTerreno[i].modificador)) {
            return 0;
        }
    }
    return 1;
}

#define CAMPO(nome) {#nome, define_##nome, escreve_##nome}

static const CampoMonstro CAMPOS[] = {
    CAMPO(id), CAMPO(nome), CAMPO(descricao), CAMPO(frequencia), CAMPO(numeroEncontrado),
    CAMPO(covil), CAMPO(climas), CAMPO(terrenos), CAMPO(organizacao), CAMPO(atividade),
    CAMPO(dieta), CAMPO(inteligencia), CAMPO(tesouro), CAMPO(ordem), CAMPO(moralidade),
    CAMPO(ca), {"movimentos", define_movimentos, escreve_movimentos_campo}, CAMPO(dv), CAMPO(thac0),
    CAMPO(ataques), CAMPO(ataquesEspeciais), CAMPO(defesasEspeciais), CAMPO(resistenciaMagia),
    CAMPO(tamanho), CAMPO(altura), CAMPO(moral), CAMPO(classificacaoMoral), CAMPO(xp),
    CAMPO(poderBase), CAMPO(modDia), CAMPO(modNoite), CAMPO(modTerreno), CAMPO(bonusPorAdicional),
    CAMPO(limiteGrupoGrande), CAMPO(bonusGrupoGrande), CAMPO(descricaoCombate),
    CAMPO(habitatSociedade), CAMPO(descricaoEcologia)
};

_Static_assert(sizeof(CAMPOS) / sizeof(CAMPOS[0]) == NUM_CAMPOS_MONSTRO, "tabela de campos");

int monstroIndiceCampo(const char *chave) {
    int i;

    if (chave == 0) {
        return -1;
    }
    for (i = 0; i < NUM_CAMPOS_MONSTRO; i++) {
        if (strcmp(chave, CAMPOS[i].chave) == 0) {
            return i;
        }
    }
    return -1;
}

const char *monstroNomeCampo(int indice) {
    if (indice < 0 || indice >= NUM_CAMPOS_MONSTRO) {
        return "";
    }
    return CAMPOS[indice].chave;
}

int monstroDefineCampo(Monstro *m, const char *chave, const char *valor, char *erro, int tamanhoErro) {
    char buffer[BUFFER_VALOR];
    char motivo[BUFFER_MOTIVO];
    Monstro copia;
    char *texto;
    int indice;

    if (m == 0 || chave == 0 || valor == 0 || erro == 0 || tamanhoErro <= 0) {
        return -1;
    }

    indice = monstroIndiceCampo(chave);
    if (indice < 0) {
        snprintf(erro, (size_t)tamanhoErro, "chave desconhecida '%s'", chave);
        return -1;
    }
    if (strlen(valor) >= sizeof(buffer)) {
        snprintf(erro, (size_t)tamanhoErro, "%s: valor longo demais", chave);
        return -1;
    }

    strcpy(buffer, valor);
    texto = aparar(buffer);
    copia = *m;
    motivo[0] = '\0';
    if (!CAMPOS[indice].define(&copia, texto, motivo, sizeof(motivo))) {
        snprintf(erro, (size_t)tamanhoErro, "%s: %s", chave, motivo);
        return -1;
    }

    *m = copia;
    return indice;
}

int monstroCampoParaTexto(const Monstro *m, int indice, char *destino, int tamanho) {
    if (m == 0 || destino == 0 || tamanho <= 0 || indice < 0 || indice >= NUM_CAMPOS_MONSTRO) {
        return 0;
    }
    destino[0] = '\0';
    return CAMPOS[indice].escreve(m, destino, tamanho);
}
