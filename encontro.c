#include "encontro.h"

#include <stdio.h>

int dificuldadeEhValida(Dificuldade dificuldade) {
    return dificuldade >= FACIL && dificuldade <= DIFICIL;
}

int periodoEhValido(Periodo periodo) {
    return periodo >= PERIODO_DIA && periodo <= PERIODO_NOITE;
}

int contextoEhValido(const ContextoEncontro *contexto) {
    return contexto != 0 && terrenoEhValido(contexto->terreno)
        && periodoEhValido(contexto->periodo) && dificuldadeEhValida(contexto->dificuldade);
}

const char *dificuldadeParaTexto(Dificuldade dificuldade) {
    switch (dificuldade) {
        case FACIL: return "Facil";
        case MEDIA: return "Media";
        case DIFICIL: return "Dificil";
        default: return "Dificuldade desconhecida";
    }
}

const char *periodoParaTexto(Periodo periodo) {
    switch (periodo) {
        case PERIODO_DIA: return "Dia";
        case PERIODO_NOITE: return "Noite";
        default: return "Periodo desconhecido";
    }
}

const char *resultadoEncontroParaTexto(ResultadoEncontro resultado) {
    switch (resultado) {
        case ENCONTRO_OK: return "Operacao realizada com sucesso.";
        case ENCONTRO_PARCIAL: return "Menos de tres propostas distintas foram encontradas dentro do limite de tentativas.";
        case ENCONTRO_NENHUMA_PROPOSTA: return "Nenhuma proposta valida foi encontrada dentro do limite de tentativas.";
        case ENCONTRO_SEM_CANDIDATOS: return "Nenhum monstro elegivel para o contexto.";
        case ENCONTRO_GRUPO_INVALIDO: return "Grupo vazio ou com poder invalido.";
        case ENCONTRO_CONTEXTO_INVALIDO: return "Terreno, periodo ou dificuldade invalidos.";
        case ENCONTRO_FAIXA_INVALIDA: return "A faixa de poder da dificuldade e vazia para este grupo.";
        case ENCONTRO_TENTATIVA_REJEITADA: return "Tentativa rejeitada.";
        case ENCONTRO_ESCOLHA_INVALIDA: return "Escolha fora das propostas disponiveis.";
        case ENCONTRO_DADOS_INVALIDOS: return "Dados invalidos.";
        default: return "Resultado desconhecido.";
    }
}

void inicializaEncontro(Encontro *e) {
    if (e == 0) {
        return;
    }
    e->quantidadeEspecies = 0;
    e->quantidadeMonstros = 0;
    e->poderTotal = 0;
}

void inicializaPropostas(ConjuntoPropostas *conjunto) {
    int i;

    if (conjunto == 0) {
        return;
    }
    conjunto->quantidade = 0;
    conjunto->indiceEscolhido = -1;
    for (i = 0; i < NUM_PROPOSTAS; i++) {
        inicializaEncontro(&conjunto->propostas[i]);
    }
}

void calculaFaixaDificuldade(int poderGrupo, Dificuldade dificuldade, int *minimo, int *maximo) {
    int percentualMinimo;
    int percentualMaximo;

    if (minimo == 0 || maximo == 0) {
        return;
    }

    switch (dificuldade) {
        case FACIL: percentualMinimo = 60; percentualMaximo = 80; break;
        case MEDIA: percentualMinimo = 90; percentualMaximo = 110; break;
        case DIFICIL: percentualMinimo = 120; percentualMaximo = 150; break;
        default: *minimo = 0; *maximo = 0; return;
    }

    *minimo = (poderGrupo * percentualMinimo + 99) / 100;
    *maximo = (poderGrupo * percentualMaximo) / 100;
}

static Atividade periodoParaAtividade(Periodo periodo) {
    return periodo == PERIODO_DIA ? ATIVIDADE_DIURNO : ATIVIDADE_NOTURNO;
}

int monstroCompativel(const Monstro *m, const ContextoEncontro *contexto) {
    return m != 0 && contextoEhValido(contexto)
        && validaMonstro(m)
        && viveNoTerreno(m, contexto->terreno)
        && ativoNoPeriodo(m, periodoParaAtividade(contexto->periodo));
}

ResultadoEncontro filtraCandidatos(const Bestiario *b, const ContextoEncontro *contexto, ListaCandidatos *saida) {
    int i;

    if (b == 0 || saida == 0) {
        return ENCONTRO_DADOS_INVALIDOS;
    }
    saida->quantidade = 0;
    if (!contextoEhValido(contexto)) {
        return ENCONTRO_CONTEXTO_INVALIDO;
    }
    for (i = 0; i < b->quantidade && saida->quantidade < MAX_CANDIDATOS; i++) {
        if (monstroCompativel(&b->monstros[i], contexto)) {
            saida->itens[saida->quantidade++] = &b->monstros[i];
        }
    }
    return saida->quantidade == 0 ? ENCONTRO_SEM_CANDIDATOS : ENCONTRO_OK;
}

static int modificadorDoTerreno(const Monstro *m, Terreno terreno) {
    int i;

    for (i = 0; i < m->poder.quantidadeModTerreno; i++) {
        if (m->poder.modTerreno[i].terreno == terreno) {
            return m->poder.modTerreno[i].modificador;
        }
    }
    return 0;
}

int poderIndividualContextual(const Monstro *m, const ContextoEncontro *contexto) {
    int poder;

    if (m == 0 || !contextoEhValido(contexto)) {
        return 0;
    }
    poder = m->poder.poderBase + modificadorDoTerreno(m, contexto->terreno)
          + (contexto->periodo == PERIODO_DIA ? m->poder.modDia : m->poder.modNoite);
    return poder < 1 ? 1 : poder;
}

int poderComponente(const Monstro *m, int quantidade, const ContextoEncontro *contexto) {
    int poder;

    if (m == 0 || quantidade <= 0 || !contextoEhValido(contexto)) {
        return 0;
    }
    poder = quantidade * poderIndividualContextual(m, contexto)
          + (quantidade - 1) * m->poder.bonusPorAdicional;
    if (m->poder.limiteGrupoGrande > 0 && quantidade >= m->poder.limiteGrupoGrande) {
        poder += m->poder.bonusGrupoGrande;
    }
    return poder;
}

int calculaPoderEncontro(Encontro *e, const Bestiario *b, const ContextoEncontro *contexto) {
    const Monstro *m;
    int total = 0;
    int monstros = 0;
    int poderes[MAX_ESPECIES_ENCONTRO];
    int i;

    if (e == 0 || b == 0 || !contextoEhValido(contexto)
        || e->quantidadeEspecies < 0 || e->quantidadeEspecies > MAX_ESPECIES_ENCONTRO) {
        return -1;
    }
    for (i = 0; i < e->quantidadeEspecies; i++) {
        m = consultaMonstroPorId(b, e->componentes[i].idMonstro);
        if (m == 0 || e->componentes[i].quantidade <= 0) {
            return -1;
        }
        poderes[i] = poderComponente(m, e->componentes[i].quantidade, contexto);
        total += poderes[i];
        monstros += e->componentes[i].quantidade;
    }
    for (i = 0; i < e->quantidadeEspecies; i++) {
        e->componentes[i].poderCalculado = poderes[i];
    }
    e->quantidadeMonstros = monstros;
    e->poderTotal = total;
    return total;
}

int pesoFrequencia(Frequencia frequencia) {
    switch (frequencia) {
        case FREQUENCIA_MUITO_RARO: return 1;
        case FREQUENCIA_RARO: return 2;
        case FREQUENCIA_INCOMUM: return 4;
        case FREQUENCIA_COMUM: return 8;
        case FREQUENCIA_MUITO_COMUM: return 12;
        default: return 0;
    }
}

const Monstro *sorteiaMonstroPonderado(const ListaCandidatos *candidatos) {
    int somaPesos = 0;
    int sorteado;
    int i;

    if (candidatos == 0 || candidatos->quantidade <= 0) {
        return 0;
    }
    for (i = 0; i < candidatos->quantidade; i++) {
        somaPesos += pesoFrequencia(candidatos->itens[i]->frequencia);
    }
    if (somaPesos <= 0) {
        return 0;
    }

    sorteado = sorteiaInteiro(0, somaPesos - 1);
    for (i = 0; i < candidatos->quantidade; i++) {
        sorteado -= pesoFrequencia(candidatos->itens[i]->frequencia);
        if (sorteado < 0) {
            return candidatos->itens[i];
        }
    }
    return 0;
}

static int especieJaUsada(const Encontro *e, int idMonstro) {
    int i;

    for (i = 0; i < e->quantidadeEspecies; i++) {
        if (e->componentes[i].idMonstro == idMonstro) {
            return 1;
        }
    }
    return 0;
}

static const Monstro *sorteiaEspecieNaoUsada(const ListaCandidatos *candidatos, const Encontro *e) {
    const Monstro *m;
    int i;

    for (i = 0; i < MAX_SORTEIOS_ESPECIE; i++) {
        m = sorteiaMonstroPonderado(candidatos);
        if (m != 0 && !especieJaUsada(e, m->id)) {
            return m;
        }
    }
    return 0;
}

ResultadoEncontro tentaGerarEncontro(const Bestiario *b, const ListaCandidatos *candidatos,
                                     const ContextoEncontro *contexto, int poderMinimo, int poderMaximo,
                                     Encontro *saida) {
    Encontro temporario;
    const Monstro *m;
    ComponenteEncontro *componente;
    int quantidade;
    int poder;

    if (b == 0 || candidatos == 0 || saida == 0 || poderMinimo > poderMaximo) {
        return ENCONTRO_DADOS_INVALIDOS;
    }
    if (!contextoEhValido(contexto)) {
        return ENCONTRO_CONTEXTO_INVALIDO;
    }
    if (candidatos->quantidade <= 0) {
        return ENCONTRO_SEM_CANDIDATOS;
    }

    inicializaEncontro(&temporario);
    while (temporario.quantidadeEspecies < MAX_ESPECIES_ENCONTRO) {
        m = sorteiaEspecieNaoUsada(candidatos, &temporario);
        if (m == 0) {
            return ENCONTRO_TENTATIVA_REJEITADA;
        }

        quantidade = rolaDado(m->numeroEncontrado);
        if (quantidade <= 0 || temporario.quantidadeMonstros + quantidade > MAX_MONSTROS_ENCONTRO) {
            return ENCONTRO_TENTATIVA_REJEITADA;
        }
        poder = poderComponente(m, quantidade, contexto);
        if (temporario.poderTotal + poder > poderMaximo) {
            return ENCONTRO_TENTATIVA_REJEITADA;
        }

        componente = &temporario.componentes[temporario.quantidadeEspecies++];
        componente->idMonstro = m->id;
        componente->quantidade = quantidade;
        componente->poderCalculado = poder;
        temporario.quantidadeMonstros += quantidade;
        temporario.poderTotal += poder;

        if (temporario.poderTotal >= poderMinimo) {
            if (calculaPoderEncontro(&temporario, b, contexto) < poderMinimo
                || temporario.poderTotal > poderMaximo) {
                return ENCONTRO_TENTATIVA_REJEITADA;
            }
            *saida = temporario;
            return ENCONTRO_OK;
        }
    }
    return ENCONTRO_TENTATIVA_REJEITADA;
}

static int quantidadeDaEspecie(const Encontro *e, int idMonstro) {
    int i;

    for (i = 0; i < e->quantidadeEspecies; i++) {
        if (e->componentes[i].idMonstro == idMonstro) {
            return e->componentes[i].quantidade;
        }
    }
    return 0;
}

int encontrosIguais(const Encontro *a, const Encontro *b) {
    int i;

    if (a == 0 || b == 0 || a->quantidadeEspecies != b->quantidadeEspecies) {
        return 0;
    }
    for (i = 0; i < a->quantidadeEspecies; i++) {
        if (quantidadeDaEspecie(b, a->componentes[i].idMonstro) != a->componentes[i].quantidade) {
            return 0;
        }
    }
    return 1;
}

static int propostaRepetida(const ConjuntoPropostas *conjunto, const Encontro *candidato) {
    int i;

    for (i = 0; i < conjunto->quantidade; i++) {
        if (encontrosIguais(&conjunto->propostas[i], candidato)) {
            return 1;
        }
    }
    return 0;
}

ResultadoEncontro geraPropostas(const Bestiario *b, const GrupoAventura *grupo,
                                const ContextoEncontro *contexto, ConjuntoPropostas *saida) {
    ConjuntoPropostas temporario;
    ListaCandidatos candidatos;
    Encontro proposta;
    ResultadoEncontro resultado;
    int minimo;
    int maximo;
    int tentativa;
    int achou;

    if (b == 0 || grupo == 0 || saida == 0) {
        return ENCONTRO_DADOS_INVALIDOS;
    }
    if (!contextoEhValido(contexto)) {
        return ENCONTRO_CONTEXTO_INVALIDO;
    }
    if (grupo->quantidade <= 0 || grupo->poderTotal <= 0) {
        return ENCONTRO_GRUPO_INVALIDO;
    }

    calculaFaixaDificuldade(grupo->poderTotal, contexto->dificuldade, &minimo, &maximo);
    if (minimo <= 0 || maximo < minimo) {
        return ENCONTRO_FAIXA_INVALIDA;
    }

    resultado = filtraCandidatos(b, contexto, &candidatos);
    if (resultado != ENCONTRO_OK) {
        return resultado;
    }

    inicializaPropostas(&temporario);
    while (temporario.quantidade < NUM_PROPOSTAS) {
        achou = 0;
        for (tentativa = 0; tentativa < MAX_TENTATIVAS_PROPOSTA && !achou; tentativa++) {
            if (tentaGerarEncontro(b, &candidatos, contexto, minimo, maximo, &proposta) == ENCONTRO_OK
                && !propostaRepetida(&temporario, &proposta)) {
                temporario.propostas[temporario.quantidade++] = proposta;
                achou = 1;
            }
        }
        if (!achou) {
            break;
        }
    }

    *saida = temporario;
    if (temporario.quantidade == NUM_PROPOSTAS) {
        return ENCONTRO_OK;
    }
    return temporario.quantidade == 0 ? ENCONTRO_NENHUMA_PROPOSTA : ENCONTRO_PARCIAL;
}

ResultadoEncontro escolheProposta(ConjuntoPropostas *conjunto, int opcao) {
    if (conjunto == 0) {
        return ENCONTRO_DADOS_INVALIDOS;
    }
    if (opcao < 1 || opcao > conjunto->quantidade) {
        return ENCONTRO_ESCOLHA_INVALIDA;
    }
    conjunto->indiceEscolhido = opcao - 1;
    return ENCONTRO_OK;
}

void exibeEncontro(const Encontro *e, const Bestiario *b) {
    const Monstro *m;
    int i;

    if (e == 0 || e->quantidadeEspecies == 0) {
        printf("  Encontro vazio.\n");
        return;
    }
    for (i = 0; i < e->quantidadeEspecies; i++) {
        m = b == 0 ? 0 : consultaMonstroPorId(b, e->componentes[i].idMonstro);
        printf("  %d x %s (ID %d) | poder %d\n", e->componentes[i].quantidade,
               m == 0 ? "?" : m->nome, e->componentes[i].idMonstro, e->componentes[i].poderCalculado);
    }
    printf("  Total: %d monstro(s), poder %d\n", e->quantidadeMonstros, e->poderTotal);
}
