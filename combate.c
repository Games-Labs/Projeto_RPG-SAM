#include "combate.h"

#include "texto.h"

#include <stdio.h>
#include <string.h>

static int rolaD20(void) {
    Dado d20;

    d20.quantidade = 1;
    d20.faces = D20_FACES;
    d20.modificador = 0;
    return rolaDado(d20);
}

const char *resultadoCombateParaTexto(ResultadoCombate resultado) {
    switch (resultado) {
        case COMBATE_OK: return "Operacao realizada com sucesso.";
        case COMBATE_ARGUMENTO_NULO: return "Argumento nulo.";
        case COMBATE_GRUPO_INVALIDO: return "Grupo vazio, repetido ou acima da capacidade.";
        case COMBATE_PERSONAGEM_INEXISTENTE: return "Personagem do grupo nao esta cadastrado.";
        case COMBATE_SEM_PERSONAGEM_ATIVO: return "Nenhum personagem do grupo possui PV positivos.";
        case COMBATE_ENCONTRO_INVALIDO: return "Encontro vazio, repetido ou fora dos limites.";
        case COMBATE_MONSTRO_INEXISTENTE: return "Especie do encontro nao existe no bestiario.";
        case COMBATE_LIMITE_EXCEDIDO: return "Quantidade de combatentes acima da capacidade.";
        case COMBATE_DADOS_INVALIDOS: return "Dados de monstro invalidos para o combate.";
        case COMBATE_NAO_PREPARADO: return "Nenhum combate preparado.";
        case COMBATE_ENCERRADO: return "O combate ja foi encerrado.";
        case COMBATE_NAO_ENCERRADO: return "O combate ainda nao foi encerrado.";
        case COMBATE_TURNO_IGNORADO: return "Turno ignorado: combatente derrotado.";
        case COMBATE_SEM_ALVO: return "Nao ha alvo valido.";
        case COMBATE_ERRO_ARQUIVO: return "Nao foi possivel gravar o arquivo.";
        case COMBATE_FUGA_FALHOU: return "A fuga falhou: os monstros atacam.";
        case COMBATE_ITEM_RECUSADO: return "O item nao pode ser usado por este personagem agora.";
        default: return "Resultado desconhecido.";
    }
}

const char *estadoCombatenteParaTexto(EstadoCombatente estado) {
    switch (estado) {
        case COMBATENTE_ATIVO: return "ativo";
        case COMBATENTE_DERROTADO: return "derrotado";
        case COMBATENTE_FUGIU: return "fugiu";
        default: return "desconhecido";
    }
}

void nomeMonstroCombate(const MonstroCombate *monstro, char *destino, int tamanho) {
    if (destino == 0 || tamanho <= 0) {
        return;
    }
    if (monstro == 0 || monstro->especie == 0) {
        snprintf(destino, (size_t)tamanho, "?");
        return;
    }
    snprintf(destino, (size_t)tamanho, "%s %d", monstro->especie->nome, monstro->numeroExemplar);
}

void inicializaCombate(Combate *combate) {
    if (combate == 0) {
        return;
    }
    memset(combate, 0, sizeof(*combate));
    combate->quantidadePersonagens = 0;
    combate->quantidadeMonstros = 0;
    combate->quantidadeOrdem = 0;
    combate->rodadaAtual = 0;
    combate->encerrado = 0;
    combate->vencedor = LADO_NENHUM;
    combate->exibirHistorico = 1;
    combate->fichasAtualizadas = 0;
    combate->efeitosResolvidos = 0;
}

int contaPersonagensAtivos(const Combate *combate) {
    int total = 0;
    int i;

    if (combate == 0) {
        return 0;
    }
    for (i = 0; i < combate->quantidadePersonagens; i++) {
        if (combate->personagens[i].estado == COMBATENTE_ATIVO) {
            total++;
        }
    }
    return total;
}

int contaMonstrosAtivos(const Combate *combate) {
    int total = 0;
    int i;

    if (combate == 0) {
        return 0;
    }
    for (i = 0; i < combate->quantidadeMonstros; i++) {
        if (combate->monstros[i].estado == COMBATENTE_ATIVO) {
            total++;
        }
    }
    return total;
}

int combateTerminou(Combate *combate) {
    int personagens;
    int monstros;

    if (combate == 0) {
        return 1;
    }
    if (combate->encerrado) {
        return 1;
    }

    personagens = contaPersonagensAtivos(combate);
    monstros = contaMonstrosAtivos(combate);
    if (personagens == 0 && monstros == 0) {
        combate->encerrado = 1;
        combate->vencedor = LADO_NENHUM;
    } else if (monstros == 0) {
        combate->encerrado = 1;
        combate->vencedor = LADO_PERSONAGENS;
    } else if (personagens == 0) {
        combate->encerrado = 1;
        combate->vencedor = LADO_MONSTROS;
    }
    return combate->encerrado;
}

int combateEmAndamento(const Combate *combate) {
    return combate != 0 && combate->quantidadeOrdem > 0 && !combate->encerrado;
}

void rolaIniciativas(Combate *combate) {
    int i;

    if (combate == 0) {
        return;
    }
    for (i = 0; i < combate->quantidadePersonagens; i++) {
        combate->personagens[i].iniciativa = rolaD20()
            + personagem_calcular_atributos_totais(combate->personagens[i].ficha).iniciativa;
    }
    for (i = 0; i < combate->quantidadeMonstros; i++) {
        combate->monstros[i].iniciativa = rolaD20();
    }
}

static int identificadorDaEntrada(const Combate *combate, EntradaOrdem entrada) {
    if (entrada.lado == LADO_PERSONAGENS) {
        return combate->personagens[entrada.indice].ficha->id;
    }
    return combate->monstros[entrada.indice].especie->id;
}

static int exemplarDaEntrada(const Combate *combate, EntradaOrdem entrada) {
    if (entrada.lado == LADO_PERSONAGENS) {
        return 0;
    }
    return combate->monstros[entrada.indice].numeroExemplar;
}

static int agePrimeiro(const Combate *combate, EntradaOrdem a, EntradaOrdem b) {
    int idA;
    int idB;

    if (a.iniciativa != b.iniciativa) {
        return a.iniciativa > b.iniciativa;
    }
    if (a.lado != b.lado) {
        return a.lado == LADO_PERSONAGENS;
    }
    idA = identificadorDaEntrada(combate, a);
    idB = identificadorDaEntrada(combate, b);
    if (idA != idB) {
        return idA < idB;
    }
    return exemplarDaEntrada(combate, a) < exemplarDaEntrada(combate, b);
}

void montaOrdemCombate(Combate *combate) {
    EntradaOrdem atual;
    int i;
    int j;

    if (combate == 0) {
        return;
    }

    combate->quantidadeOrdem = 0;
    for (i = 0; i < combate->quantidadePersonagens; i++) {
        atual.lado = LADO_PERSONAGENS;
        atual.indice = i;
        atual.iniciativa = combate->personagens[i].iniciativa;
        combate->ordem[combate->quantidadeOrdem++] = atual;
    }
    for (i = 0; i < combate->quantidadeMonstros; i++) {
        atual.lado = LADO_MONSTROS;
        atual.indice = i;
        atual.iniciativa = combate->monstros[i].iniciativa;
        combate->ordem[combate->quantidadeOrdem++] = atual;
    }

    for (i = 1; i < combate->quantidadeOrdem; i++) {
        atual = combate->ordem[i];
        j = i - 1;
        while (j >= 0 && agePrimeiro(combate, atual, combate->ordem[j])) {
            combate->ordem[j + 1] = combate->ordem[j];
            j--;
        }
        combate->ordem[j + 1] = atual;
    }
}

static void nomeDaEntrada(const Combate *combate, EntradaOrdem entrada, char *destino, int tamanho) {
    if (entrada.lado == LADO_PERSONAGENS) {
        snprintf(destino, (size_t)tamanho, "%s", combate->personagens[entrada.indice].ficha->nome);
    } else {
        nomeMonstroCombate(&combate->monstros[entrada.indice], destino, tamanho);
    }
}

void exibeOrdem(const Combate *combate) {
    char nome[NOME_COMBATENTE_TAMANHO];
    int i;
    int vida;
    int maxima;

    if (combate == 0 || combate->quantidadeOrdem == 0) {
        printf("Nenhum combate preparado.\n");
        return;
    }
    printf("Ordem de acao:\n");
    for (i = 0; i < combate->quantidadeOrdem; i++) {
        nomeDaEntrada(combate, combate->ordem[i], nome, sizeof(nome));
        if (combate->ordem[i].lado == LADO_PERSONAGENS) {
            vida = combate->personagens[combate->ordem[i].indice].pontosVidaAtuais;
            maxima = combate->personagens[combate->ordem[i].indice].pontosVidaMaximos;
        } else {
            vida = combate->monstros[combate->ordem[i].indice].pontosVidaAtuais;
            maxima = combate->monstros[combate->ordem[i].indice].pontosVidaMaximos;
        }
        printf("%2d. ", i + 1);
        texto_imprime_alinhado(stdout, nome, 28);
        printf(" | %-10s | iniciativa %2d | PV %d/%d\n",
               combate->ordem[i].lado == LADO_PERSONAGENS ? "personagem" : "monstro",
               combate->ordem[i].iniciativa, vida, maxima);
    }
}

static void escreveEspeciais(FILE *f, const char *tipo, const char *dono, const HabilidadeEspecial *lista, int quantidade) {
    int i;

    for (i = 0; i < quantidade; i++) {
        fprintf(f, "  ALERTA: %s de %s: %s (%s). O efeito especial nao foi automatizado; consulte a descricao da ficha.\n",
               tipo, dono, tipoEspecialParaTexto(lista[i].tipo), lista[i].descricao);
    }
}

static void escreveEspeciaisDasEspecies(FILE *f, const Combate *combate) {
    const Monstro *vistas[MAX_MONSTROS_ENCONTRO];
    int quantidadeVistas = 0;
    int i;
    int j;
    int repetida;

    for (i = 0; i < combate->quantidadeMonstros; i++) {
        repetida = 0;
        for (j = 0; j < quantidadeVistas; j++) {
            if (vistas[j] == combate->monstros[i].especie) {
                repetida = 1;
            }
        }
        if (!repetida) {
            vistas[quantidadeVistas++] = combate->monstros[i].especie;
        }
    }
    for (i = 0; i < quantidadeVistas; i++) {
        escreveEspeciais(f, "ataque especial", vistas[i]->nome, vistas[i]->ataquesEspeciais,
                       vistas[i]->quantidadeAtaquesEspeciais);
        escreveEspeciais(f, "defesa especial", vistas[i]->nome, vistas[i]->defesasEspeciais,
                       vistas[i]->quantidadeDefesasEspeciais);
    }
}

static void escreveParticipantes(FILE *f, const Combate *combate) {
    char nome[NOME_COMBATENTE_TAMANHO];
    int i;

    if (combate == 0 || combate->quantidadeOrdem == 0) {
        fprintf(f, "Nenhum combate preparado.\n");
        return;
    }
    fprintf(f, "Personagens:\n");
    for (i = 0; i < combate->quantidadePersonagens; i++) {
        fprintf(f, "  ");
        texto_imprime_alinhado(f, combate->personagens[i].ficha->nome, 24);
        fprintf(f, " | PV %d/%d | iniciativa %d | %s\n",
               combate->personagens[i].pontosVidaAtuais, combate->personagens[i].pontosVidaMaximos,
               combate->personagens[i].iniciativa, estadoCombatenteParaTexto(combate->personagens[i].estado));
    }
    fprintf(f, "Monstros:\n");
    for (i = 0; i < combate->quantidadeMonstros; i++) {
        nomeMonstroCombate(&combate->monstros[i], nome, sizeof(nome));
        fprintf(f, "  ");
        texto_imprime_alinhado(f, nome, 24);
        fprintf(f, " | PV %d/%d | iniciativa %d | %s\n", combate->monstros[i].pontosVidaAtuais,
               combate->monstros[i].pontosVidaMaximos, combate->monstros[i].iniciativa,
               estadoCombatenteParaTexto(combate->monstros[i].estado));
    }
    escreveEspeciaisDasEspecies(f, combate);
}

static int grupoValido(const GrupoAventura *grupo, CadastroPersonagens *cadastro, int *algumAtivo) {
    int i;
    int j;

    *algumAtivo = 0;
    for (i = 0; i < grupo->quantidade; i++) {
        for (j = 0; j < i; j++) {
            if (grupo->idsPersonagens[j] == grupo->idsPersonagens[i]) {
                return COMBATE_GRUPO_INVALIDO;
            }
        }
        if (cadastro_buscar_por_id(cadastro, grupo->idsPersonagens[i]) == 0) {
            return COMBATE_PERSONAGEM_INEXISTENTE;
        }
    }
    for (i = 0; i < grupo->quantidade; i++) {
        if (cadastro_buscar_por_id(cadastro, grupo->idsPersonagens[i])->pontos_vida_atuais > 0) {
            *algumAtivo = 1;
        }
    }
    return COMBATE_OK;
}

static ResultadoCombate encontroValido(const Encontro *encontro, const Bestiario *bestiario) {
    const Monstro *especie;
    int total = 0;
    int i;
    int j;

    if (encontro->quantidadeEspecies < 1 || encontro->quantidadeEspecies > MAX_ESPECIES_ENCONTRO) {
        return COMBATE_ENCONTRO_INVALIDO;
    }
    for (i = 0; i < encontro->quantidadeEspecies; i++) {
        if (encontro->componentes[i].quantidade <= 0) {
            return COMBATE_ENCONTRO_INVALIDO;
        }
        for (j = 0; j < i; j++) {
            if (encontro->componentes[j].idMonstro == encontro->componentes[i].idMonstro) {
                return COMBATE_ENCONTRO_INVALIDO;
            }
        }
        total += encontro->componentes[i].quantidade;
        if (total > MAX_MONSTROS_ENCONTRO) {
            return COMBATE_LIMITE_EXCEDIDO;
        }
        especie = consultaMonstroPorId(bestiario, encontro->componentes[i].idMonstro);
        if (especie == 0) {
            return COMBATE_MONSTRO_INEXISTENTE;
        }
        if (!validaMonstro(especie)) {
            return COMBATE_DADOS_INVALIDOS;
        }
    }
    return COMBATE_OK;
}

ResultadoCombate preparaCombate(Combate *combate, CadastroPersonagens *cadastro, const GrupoAventura *grupo,
                                const Encontro *encontro, const Bestiario *bestiario) {
    Combate temporario;
    PersonagemCombate *personagem;
    MonstroCombate *monstro;
    const Monstro *especie;
    int algumAtivo;
    int vidaMaxima;
    int vida;
    int i;
    int exemplar;
    ResultadoCombate resultado;

    if (combate == 0 || cadastro == 0 || grupo == 0 || encontro == 0 || bestiario == 0) {
        return COMBATE_ARGUMENTO_NULO;
    }
    if (grupo->quantidade < 1 || grupo->quantidade > MAX_PERSONAGENS_GRUPO) {
        return COMBATE_GRUPO_INVALIDO;
    }
    resultado = (ResultadoCombate)grupoValido(grupo, cadastro, &algumAtivo);
    if (resultado != COMBATE_OK) {
        return resultado;
    }
    if (!algumAtivo) {
        return COMBATE_SEM_PERSONAGEM_ATIVO;
    }
    resultado = encontroValido(encontro, bestiario);
    if (resultado != COMBATE_OK) {
        return resultado;
    }

    inicializaCombate(&temporario);
    temporario.exibirHistorico = combate->exibirHistorico;

    for (i = 0; i < grupo->quantidade; i++) {
        personagem = &temporario.personagens[temporario.quantidadePersonagens++];
        personagem->ficha = cadastro_buscar_por_id(cadastro, grupo->idsPersonagens[i]);
        vidaMaxima = personagem_calcular_atributos_totais(personagem->ficha).pontos_vida_maximos;
        if (vidaMaxima < 1) {
            vidaMaxima = 1;
        }
        vida = personagem->ficha->pontos_vida_atuais;
        if (vida > vidaMaxima) {
            vida = vidaMaxima;
        }
        personagem->pontosVidaMaximos = vidaMaxima;
        personagem->pontosVidaAtuais = vida;
        personagem->iniciativa = 0;
        personagem->estado = vida > 0 ? COMBATENTE_ATIVO : COMBATENTE_DERROTADO;
        personagem->temCura = 0;
        personagem->temMagia = 0;
    }

    for (i = 0; i < encontro->quantidadeEspecies; i++) {
        especie = consultaMonstroPorId(bestiario, encontro->componentes[i].idMonstro);
        for (exemplar = 1; exemplar <= encontro->componentes[i].quantidade; exemplar++) {
            monstro = &temporario.monstros[temporario.quantidadeMonstros++];
            vida = rolaDado(especie->dadosVida);
            if (vida < 1) {
                vida = 1;
            }
            monstro->numeroExemplar = exemplar;
            monstro->especie = especie;
            monstro->pontosVidaMaximos = vida;
            monstro->pontosVidaAtuais = vida;
            monstro->iniciativa = 0;
            monstro->estado = COMBATENTE_ATIVO;
            monstro->testesMoral = 0;
        }
    }

    rolaIniciativas(&temporario);
    montaOrdemCombate(&temporario);
    *combate = temporario;
    return COMBATE_OK;
}

int selecionaPersonagemAlvo(const Combate *combate) {
    int validos[MAX_PERSONAGENS_GRUPO];
    int quantidade = 0;
    int i;

    if (combate == 0) {
        return -1;
    }
    for (i = 0; i < combate->quantidadePersonagens; i++) {
        if (combate->personagens[i].estado == COMBATENTE_ATIVO) {
            validos[quantidade++] = i;
        }
    }
    if (quantidade == 0) {
        return -1;
    }
    return validos[sorteiaInteiro(0, quantidade - 1)];
}

int selecionaMonstroAlvo(const Combate *combate) {
    int validos[MAX_MONSTROS_ENCONTRO];
    int quantidade = 0;
    int i;

    if (combate == 0) {
        return -1;
    }
    for (i = 0; i < combate->quantidadeMonstros; i++) {
        if (combate->monstros[i].estado == COMBATENTE_ATIVO) {
            validos[quantidade++] = i;
        }
    }
    if (quantidade == 0) {
        return -1;
    }
    return validos[sorteiaInteiro(0, quantidade - 1)];
}

int defesaEquivalenteMonstro(const Monstro *monstro) {
    return monstro == 0 ? 0 : 20 - monstro->classeArmadura;
}

int bonusAtaqueMonstro(const Monstro *monstro) {
    return monstro == 0 ? 0 : 20 - monstro->thac0;
}

int verificaAcerto(int resultadoD20, int bonusAtaque, int defesaAlvo) {
    if (resultadoD20 <= 1) {
        return 0;
    }
    if (resultadoD20 >= D20_FACES) {
        return 1;
    }
    return resultadoD20 + bonusAtaque >= defesaAlvo;
}

int aplicaDanoPersonagem(PersonagemCombate *alvo, int dano) {
    if (alvo == 0 || alvo->estado != COMBATENTE_ATIVO) {
        return 0;
    }
    if (dano < 0) {
        dano = 0;
    }
    alvo->pontosVidaAtuais -= dano;
    if (alvo->pontosVidaAtuais <= 0) {
        alvo->pontosVidaAtuais = 0;
        alvo->estado = COMBATENTE_DERROTADO;
        return 1;
    }
    return 0;
}

int aplicaDanoMonstro(MonstroCombate *alvo, int dano) {
    if (alvo == 0 || alvo->estado != COMBATENTE_ATIVO) {
        return 0;
    }
    if (dano < 0) {
        dano = 0;
    }
    alvo->pontosVidaAtuais -= dano;
    if (alvo->pontosVidaAtuais <= 0) {
        alvo->pontosVidaAtuais = 0;
        alvo->estado = COMBATENTE_DERROTADO;
        return 1;
    }
    return 0;
}

static int rolaDano(Dado dano) {
    int resultado = rolaDado(dano);

    return resultado < 0 ? 0 : resultado;
}

static void relataJogada(const Combate *combate, const char *atacante, const char *alvo, const char *golpe,
                         int d20, int bonus, int defesa, int acertou) {
    if (!combate->exibirHistorico) {
        return;
    }
    printf("\n%s ataca %s com %s.\n", atacante, alvo, golpe);
    printf("Dado: %d%s. Bonus: %d. Total: %d. Defesa: %d. %s.\n", d20,
           d20 == 1 ? " (natural)" : (d20 == D20_FACES ? " (natural)" : ""), bonus, d20 + bonus,
           defesa, acertou ? "ACERTO" : "ERROU");
}

static void relataDano(const Combate *combate, int dano, const char *alvo, int vida, int maxima, int derrotou) {
    if (!combate->exibirHistorico) {
        return;
    }
    printf("Dano: %d. %s ficou com %d/%d PV.\n", dano, alvo, vida, maxima);
    if (derrotou) {
        printf("%s foi derrotado.\n", alvo);
    }
}

static void golpePersonagem(Combate *combate, int atacante, int alvo, const char *nomeArma, Dado dano, int bonus) {
    PersonagemCombate *agressor = &combate->personagens[atacante];
    MonstroCombate *vitima = &combate->monstros[alvo];
    char nomeAlvo[NOME_COMBATENTE_TAMANHO];
    int defesa;
    int d20;
    int acertou;
    int pontos;

    nomeMonstroCombate(vitima, nomeAlvo, sizeof(nomeAlvo));
    defesa = defesaEquivalenteMonstro(vitima->especie);
    d20 = rolaD20();
    acertou = verificaAcerto(d20, bonus, defesa);
    relataJogada(combate, agressor->ficha->nome, nomeAlvo, nomeArma, d20, bonus, defesa, acertou);

    if (acertou) {
        pontos = rolaDano(dano);
        relataDano(combate, pontos, nomeAlvo, vitima->pontosVidaAtuais - pontos < 0 ? 0 : vitima->pontosVidaAtuais - pontos,
                   vitima->pontosVidaMaximos, vitima->pontosVidaAtuais - pontos <= 0);
        aplicaDanoMonstro(vitima, pontos);
    }
}

static int armaValidaNaMao(const ItemEquipado *mao) {
    return mao->ocupado && tipo_item_eh_arma(mao->item.tipo) && validaDado(mao->item.dano);
}

ResultadoCombate executaAtaquePersonagem(Combate *combate, int atacante, int alvo) {
    PersonagemCombate *agressor;
    const Equipamentos *equipamentos;
    Item arma;
    Dado dano;
    const char *nomeArma;
    int bonus;
    int segundoAlvo;

    if (combate == 0) {
        return COMBATE_ARGUMENTO_NULO;
    }
    if (atacante < 0 || atacante >= combate->quantidadePersonagens
        || alvo < 0 || alvo >= combate->quantidadeMonstros) {
        return COMBATE_DADOS_INVALIDOS;
    }
    agressor = &combate->personagens[atacante];
    if (agressor->estado != COMBATENTE_ATIVO || combate->monstros[alvo].estado != COMBATENTE_ATIVO) {
        return COMBATE_SEM_ALVO;
    }

    bonus = personagem_calcular_atributos_totais(agressor->ficha).ataque;
    equipamentos = &agressor->ficha->equipamentos;

    if (!equipamentos->arma_duas_maos_equipada && armaValidaNaMao(&equipamentos->mao_direita)
        && armaValidaNaMao(&equipamentos->mao_esquerda)) {
        golpePersonagem(combate, atacante, alvo, equipamentos->mao_direita.item.nome,
                        equipamentos->mao_direita.item.dano, bonus);
        segundoAlvo = combate->monstros[alvo].estado == COMBATENTE_ATIVO ? alvo : selecionaMonstroAlvo(combate);
        if (segundoAlvo >= 0) {
            golpePersonagem(combate, atacante, segundoAlvo, equipamentos->mao_esquerda.item.nome,
                            equipamentos->mao_esquerda.item.dano, bonus - PENALIDADE_SEGUNDA_ARMA);
        }
        return COMBATE_OK;
    }

    if (personagem_obter_arma_principal(agressor->ficha, &arma)) {
        dano = arma.dano;
        nomeArma = arma.nome;
    } else {
        dano.quantidade = DANO_DESARMADO_QUANTIDADE;
        dano.faces = DANO_DESARMADO_FACES;
        dano.modificador = 0;
        nomeArma = "ataque desarmado";
    }
    golpePersonagem(combate, atacante, alvo, nomeArma, dano, bonus);
    return COMBATE_OK;
}

static int escolheAliadoFerido(const Combate *combate) {
    int melhor = -1;
    int i;

    for (i = 0; i < combate->quantidadePersonagens; i++) {
        const PersonagemCombate *p = &combate->personagens[i];
        if (p->estado == COMBATENTE_ATIVO && p->pontosVidaAtuais * 2 <= p->pontosVidaMaximos
            && (melhor < 0
                || p->pontosVidaAtuais * combate->personagens[melhor].pontosVidaMaximos
                   < combate->personagens[melhor].pontosVidaAtuais * p->pontosVidaMaximos)) {
            melhor = i;
        }
    }
    return melhor;
}

static void executaCura(Combate *combate, int curandeiro, int alvo) {
    PersonagemCombate *agressor = &combate->personagens[curandeiro];
    PersonagemCombate *paciente = &combate->personagens[alvo];
    int pontos = rolaDano(agressor->dadoCura);

    if (paciente->pontosVidaAtuais + pontos > paciente->pontosVidaMaximos) {
        pontos = paciente->pontosVidaMaximos - paciente->pontosVidaAtuais;
    }
    paciente->pontosVidaAtuais += pontos;
    if (combate->exibirHistorico) {
        printf("\n%s cura %s: +%d PV (%d/%d).\n", agressor->ficha->nome, paciente->ficha->nome, pontos,
               paciente->pontosVidaAtuais, paciente->pontosVidaMaximos);
    }
}

static void executaMagia(Combate *combate, int atacante, int alvo) {
    PersonagemCombate *agressor = &combate->personagens[atacante];
    MonstroCombate *vitima = &combate->monstros[alvo];
    char nomeAlvo[NOME_COMBATENTE_TAMANHO];
    int pontos = rolaDano(agressor->dadoMagia);

    nomeMonstroCombate(vitima, nomeAlvo, sizeof(nomeAlvo));
    if (combate->exibirHistorico) {
        printf("\n%s lanca magia em %s (ignora a defesa).\n", agressor->ficha->nome, nomeAlvo);
    }
    relataDano(combate, pontos, nomeAlvo, vitima->pontosVidaAtuais - pontos < 0 ? 0 : vitima->pontosVidaAtuais - pontos,
               vitima->pontosVidaMaximos, vitima->pontosVidaAtuais - pontos <= 0);
    aplicaDanoMonstro(vitima, pontos);
}

ResultadoCombate configuraHabilidadesPersonagem(Combate *combate, int indice, int temCura, Dado dadoCura,
                                                int temMagia, Dado dadoMagia) {
    if (combate == 0) {
        return COMBATE_ARGUMENTO_NULO;
    }
    if (indice < 0 || indice >= combate->quantidadePersonagens) {
        return COMBATE_DADOS_INVALIDOS;
    }
    if ((temCura && !validaDado(dadoCura)) || (temMagia && !validaDado(dadoMagia))) {
        return COMBATE_DADOS_INVALIDOS;
    }
    combate->personagens[indice].temCura = temCura;
    combate->personagens[indice].dadoCura = dadoCura;
    combate->personagens[indice].temMagia = temMagia;
    combate->personagens[indice].dadoMagia = dadoMagia;
    return COMBATE_OK;
}

static int monstroFoge(Combate *combate, int indice) {
    MonstroCombate *m = &combate->monstros[indice];
    char nome[NOME_COMBATENTE_TAMANHO];
    int gatilho = 0;
    int i;
    int d20;

    if (!(m->testesMoral & TESTE_MORAL_VIDA) && m->pontosVidaAtuais * 2 <= m->pontosVidaMaximos) {
        m->testesMoral |= TESTE_MORAL_VIDA;
        gatilho = 1;
    }
    if (!(m->testesMoral & TESTE_MORAL_ALIADO)) {
        for (i = 0; i < combate->quantidadeMonstros; i++) {
            if (combate->monstros[i].estado == COMBATENTE_DERROTADO) {
                m->testesMoral |= TESTE_MORAL_ALIADO;
                gatilho = 1;
                break;
            }
        }
    }
    if (!gatilho) {
        return 0;
    }
    d20 = rolaD20();
    nomeMonstroCombate(m, nome, sizeof(nome));
    if (combate->exibirHistorico) {
        printf("\nTeste de moral de %s: d20 = %d contra moral %d. %s.\n", nome, d20, m->especie->moral,
               d20 > m->especie->moral ? "FUGIU" : "manteve a posicao");
    }
    if (d20 > m->especie->moral) {
        m->estado = COMBATENTE_FUGIU;
        return 1;
    }
    return 0;
}

ResultadoCombate executaAtaquesMonstro(Combate *combate, int atacante, int alvo) {
    MonstroCombate *agressor;
    PersonagemCombate *vitima;
    char nomeAgressor[NOME_COMBATENTE_TAMANHO];
    int bonus;
    int defesa;
    int d20;
    int acertou;
    int pontos;
    int i;

    if (combate == 0) {
        return COMBATE_ARGUMENTO_NULO;
    }
    if (atacante < 0 || atacante >= combate->quantidadeMonstros
        || alvo < 0 || alvo >= combate->quantidadePersonagens) {
        return COMBATE_DADOS_INVALIDOS;
    }
    agressor = &combate->monstros[atacante];
    vitima = &combate->personagens[alvo];
    if (agressor->estado != COMBATENTE_ATIVO || vitima->estado != COMBATENTE_ATIVO) {
        return COMBATE_SEM_ALVO;
    }

    nomeMonstroCombate(agressor, nomeAgressor, sizeof(nomeAgressor));
    bonus = bonusAtaqueMonstro(agressor->especie);
    for (i = 0; i < agressor->especie->quantidadeAtaques && vitima->estado == COMBATENTE_ATIVO; i++) {
        defesa = personagem_calcular_atributos_totais(vitima->ficha).defesa;
        d20 = rolaD20();
        acertou = verificaAcerto(d20, bonus, defesa);
        relataJogada(combate, nomeAgressor, vitima->ficha->nome, agressor->especie->ataques[i].nome,
                     d20, bonus, defesa, acertou);
        if (acertou) {
            pontos = rolaDano(agressor->especie->ataques[i].dano);
            relataDano(combate, pontos, vitima->ficha->nome,
                       vitima->pontosVidaAtuais - pontos < 0 ? 0 : vitima->pontosVidaAtuais - pontos,
                       vitima->pontosVidaMaximos, vitima->pontosVidaAtuais - pontos <= 0);
            aplicaDanoPersonagem(vitima, pontos);
        }
    }
    return COMBATE_OK;
}

ResultadoCombate executaTurno(Combate *combate, int posicaoOrdem) {
    EntradaOrdem entrada;
    int alvo;

    if (combate == 0) {
        return COMBATE_ARGUMENTO_NULO;
    }
    if (combate->quantidadeOrdem == 0) {
        return COMBATE_NAO_PREPARADO;
    }
    if (combate->encerrado) {
        return COMBATE_ENCERRADO;
    }
    if (posicaoOrdem < 0 || posicaoOrdem >= combate->quantidadeOrdem) {
        return COMBATE_DADOS_INVALIDOS;
    }

    entrada = combate->ordem[posicaoOrdem];
    if (entrada.lado == LADO_PERSONAGENS) {
        if (combate->personagens[entrada.indice].estado != COMBATENTE_ATIVO) {
            return COMBATE_TURNO_IGNORADO;
        }
        alvo = selecionaMonstroAlvo(combate);
        if (alvo < 0) {
            combateTerminou(combate);
            return COMBATE_SEM_ALVO;
        }
        if (combate->personagens[entrada.indice].temCura && escolheAliadoFerido(combate) >= 0) {
            executaCura(combate, entrada.indice, escolheAliadoFerido(combate));
        } else if (combate->personagens[entrada.indice].temMagia) {
            executaMagia(combate, entrada.indice, alvo);
        } else {
            executaAtaquePersonagem(combate, entrada.indice, alvo);
        }
    } else {
        if (combate->monstros[entrada.indice].estado != COMBATENTE_ATIVO) {
            return COMBATE_TURNO_IGNORADO;
        }
        if (monstroFoge(combate, entrada.indice)) {
            combateTerminou(combate);
            return COMBATE_TURNO_IGNORADO;
        }
        alvo = selecionaPersonagemAlvo(combate);
        if (alvo < 0) {
            combateTerminou(combate);
            return COMBATE_SEM_ALVO;
        }
        executaAtaquesMonstro(combate, entrada.indice, alvo);
    }

    combateTerminou(combate);
    return COMBATE_OK;
}

static int iniciativaMaisLenta(const Combate *combate) {
    int menor = 0;
    int achou = 0;
    int valor;
    int i;

    for (i = 0; i < combate->quantidadePersonagens; i++) {
        if (combate->personagens[i].estado == COMBATENTE_ATIVO) {
            valor = personagem_calcular_atributos_totais(combate->personagens[i].ficha).iniciativa;
            if (!achou || valor < menor) {
                menor = valor;
                achou = 1;
            }
        }
    }
    return menor;
}

ResultadoCombate tentaFugaGrupo(Combate *combate) {
    int bonus;
    int d20;
    int posicao;
    int alvo;
    EntradaOrdem entrada;

    if (combate == 0) {
        return COMBATE_ARGUMENTO_NULO;
    }
    if (combate->quantidadeOrdem == 0) {
        return COMBATE_NAO_PREPARADO;
    }
    if (combate->encerrado) {
        return COMBATE_ENCERRADO;
    }

    bonus = iniciativaMaisLenta(combate);
    d20 = rolaD20();
    if (combate->exibirHistorico) {
        printf("\nTentativa de fuga: d20 = %d + iniciativa do mais lento %d = %d contra %d. %s.\n", d20, bonus,
               d20 + bonus, FUGA_DIFICULDADE, d20 + bonus >= FUGA_DIFICULDADE ? "O GRUPO FUGIU" : "FALHOU");
    }
    if (d20 + bonus >= FUGA_DIFICULDADE) {
        combate->encerrado = 1;
        combate->vencedor = LADO_NENHUM;
        combate->grupoFugiu = 1;
        return COMBATE_OK;
    }

    combate->rodadaAtual++;
    if (combate->exibirHistorico) {
        printf("\nRODADA %d (apenas os monstros atacam)\n", combate->rodadaAtual);
    }
    for (posicao = 0; posicao < combate->quantidadeOrdem && !combate->encerrado; posicao++) {
        entrada = combate->ordem[posicao];
        if (entrada.lado != LADO_MONSTROS || combate->monstros[entrada.indice].estado != COMBATENTE_ATIVO) {
            continue;
        }
        alvo = selecionaPersonagemAlvo(combate);
        if (alvo >= 0) {
            executaAtaquesMonstro(combate, entrada.indice, alvo);
        }
        combateTerminou(combate);
    }
    if (!combate->encerrado && combate->rodadaAtual >= MAX_RODADAS) {
        combate->encerrado = 1;
        combate->vencedor = LADO_NENHUM;
    }
    return COMBATE_FUGA_FALHOU;
}

ResultadoCombate usaItemEmCombate(Combate *combate, int indice, int idItem, UsoItem *uso) {
    PersonagemCombate *personagem;

    if (combate == 0) {
        return COMBATE_ARGUMENTO_NULO;
    }
    if (combate->quantidadeOrdem == 0) {
        return COMBATE_NAO_PREPARADO;
    }
    if (combate->encerrado) {
        return COMBATE_ENCERRADO;
    }
    if (indice < 0 || indice >= combate->quantidadePersonagens) {
        return COMBATE_DADOS_INVALIDOS;
    }
    personagem = &combate->personagens[indice];
    if (personagem->estado != COMBATENTE_ATIVO) {
        return COMBATE_ITEM_RECUSADO;
    }
    if (personagem_consumir_item(personagem->ficha, idItem, &personagem->pontosVidaAtuais,
                                 personagem->pontosVidaMaximos, uso) != RESULTADO_OK) {
        return COMBATE_ITEM_RECUSADO;
    }
    return COMBATE_OK;
}

int calculaRecompensaMoedas(const Combate *combate) {
    if (combate == 0 || !combate->encerrado || combate->vencedor != LADO_PERSONAGENS) {
        return 0;
    }
    return calculaXPDerrotados(combate) / XP_POR_MOEDA;
}

int calculaRecompensaXP(const Combate *combate) {
    if (combate == 0 || !combate->encerrado || combate->vencedor != LADO_PERSONAGENS) {
        return 0;
    }
    return calculaXPDerrotados(combate);
}

void encerraBonusTemporarios(Combate *combate) {
    int i;

    if (combate == 0) {
        return;
    }
    for (i = 0; i < combate->quantidadePersonagens; i++) {
        personagem_limpar_bonus_temporarios(combate->personagens[i].ficha);
    }
}

ResultadoCombate executaRodada(Combate *combate) {
    int posicao;

    if (combate == 0) {
        return COMBATE_ARGUMENTO_NULO;
    }
    if (combate->quantidadeOrdem == 0) {
        return COMBATE_NAO_PREPARADO;
    }
    if (combate->encerrado) {
        return COMBATE_ENCERRADO;
    }
    if (combate->rodadaAtual >= MAX_RODADAS) {
        combate->encerrado = 1;
        combate->vencedor = LADO_NENHUM;
        return COMBATE_ENCERRADO;
    }

    combate->rodadaAtual++;
    if (combate->exibirHistorico) {
        printf("\nRODADA %d\n", combate->rodadaAtual);
    }
    for (posicao = 0; posicao < combate->quantidadeOrdem && !combate->encerrado; posicao++) {
        executaTurno(combate, posicao);
    }
    if (!combate->encerrado && combate->rodadaAtual >= MAX_RODADAS) {
        combate->encerrado = 1;
        combate->vencedor = LADO_NENHUM;
        if (combate->exibirHistorico) {
            printf("\nLimite de %d rodadas atingido. Combate encerrado sem vencedor.\n", MAX_RODADAS);
        }
    }
    return COMBATE_OK;
}

ResultadoCombate executaCombate(Combate *combate) {
    ResultadoCombate resultado = COMBATE_OK;

    if (combate == 0) {
        return COMBATE_ARGUMENTO_NULO;
    }
    if (combate->quantidadeOrdem == 0) {
        return COMBATE_NAO_PREPARADO;
    }
    if (combate->encerrado) {
        return COMBATE_ENCERRADO;
    }
    while (!combate->encerrado && resultado == COMBATE_OK) {
        resultado = executaRodada(combate);
    }
    return combate->encerrado ? COMBATE_OK : resultado;
}

int calculaXPDerrotados(const Combate *combate) {
    int total = 0;
    int i;

    if (combate == 0) {
        return 0;
    }
    for (i = 0; i < combate->quantidadeMonstros; i++) {
        if (combate->monstros[i].estado == COMBATENTE_DERROTADO) {
            total += combate->monstros[i].especie->valorXP;
        }
    }
    return total;
}

void exibeEstadoCombate(const Combate *combate) {
    if (combate == 0 || combate->quantidadeOrdem == 0) {
        printf("Nenhum combate preparado.\n");
        return;
    }
    printf("Rodada atual: %d | Personagens ativos: %d | Monstros ativos: %d | %s\n", combate->rodadaAtual,
           contaPersonagensAtivos(combate), contaMonstrosAtivos(combate),
           combate->encerrado ? "encerrado" : "em andamento");
    exibeParticipantes(combate);
}

static void escreveDerrotadosPorEspecie(FILE *f, const Combate *combate) {
    const Monstro *vistas[MAX_MONSTROS_ENCONTRO];
    int quantidadeVistas = 0;
    int fugitivos;
    int derrotados;
    int total;
    int i;
    int j;
    int repetida;

    for (i = 0; i < combate->quantidadeMonstros; i++) {
        repetida = 0;
        for (j = 0; j < quantidadeVistas; j++) {
            if (vistas[j] == combate->monstros[i].especie) {
                repetida = 1;
            }
        }
        if (!repetida) {
            vistas[quantidadeVistas++] = combate->monstros[i].especie;
        }
    }
    for (i = 0; i < quantidadeVistas; i++) {
        derrotados = 0;
        fugitivos = 0;
        total = 0;
        for (j = 0; j < combate->quantidadeMonstros; j++) {
            if (combate->monstros[j].especie == vistas[i]) {
                total++;
                if (combate->monstros[j].estado == COMBATENTE_DERROTADO) {
                    derrotados++;
                } else if (combate->monstros[j].estado == COMBATENTE_FUGIU) {
                    fugitivos++;
                }
            }
        }
        fprintf(f, "  %s: %d de %d derrotado(s), %d fugiu(ram)\n", vistas[i]->nome, derrotados, total, fugitivos);
    }
}

static void escreveResumoCombate(FILE *f, const Combate *combate) {
    if (combate == 0 || combate->quantidadeOrdem == 0) {
        fprintf(f, "Nenhum combate preparado.\n");
        return;
    }
    fprintf(f, "\n=== RESUMO DO COMBATE ===\n");
    if (!combate->encerrado) {
        fprintf(f, "Situacao: combate em andamento.\n");
    } else if (combate->vencedor == LADO_PERSONAGENS) {
        fprintf(f, "Resultado: vitoria dos personagens.\n");
    } else if (combate->vencedor == LADO_MONSTROS) {
        fprintf(f, "Resultado: vitoria dos monstros.\n");
    } else if (combate->grupoFugiu) {
        fprintf(f, "Resultado: o grupo fugiu do encontro. Sem vencedor e sem recompensa.\n");
    } else if (combate->rodadaAtual < MAX_RODADAS) {
        fprintf(f, "Resultado: encerrado sem vencedor.\n");
    } else {
        fprintf(f, "Resultado: encerrado sem vencedor (limite de %d rodadas).\n", MAX_RODADAS);
    }
    fprintf(f, "Rodadas: %d\n", combate->rodadaAtual);
    escreveParticipantes(f, combate);
    fprintf(f, "Monstros derrotados por especie:\n");
    escreveDerrotadosPorEspecie(f, combate);
    fprintf(f, "XP dos monstros derrotados: %d\n", calculaXPDerrotados(combate));
    fprintf(f, "Recompensa em moedas (XP / %d, dividida entre os personagens de pe): %d\n", XP_POR_MOEDA,
            calculaRecompensaMoedas(combate));
    fprintf(f, "Experiencia (dividida entre os personagens de pe): %d\n", calculaRecompensaXP(combate));
}

void exibeParticipantes(const Combate *combate) {
    escreveParticipantes(stdout, combate);
}

void exibeResumoCombate(const Combate *combate) {
    escreveResumoCombate(stdout, combate);
}

ResultadoCombate salvaResumoCombate(const Combate *combate, const char *nomeArquivo) {
    FILE *arquivo;

    if (combate == 0 || nomeArquivo == 0) {
        return COMBATE_ARGUMENTO_NULO;
    }
    if (combate->quantidadeOrdem == 0) {
        return COMBATE_NAO_PREPARADO;
    }
    arquivo = fopen(nomeArquivo, "w");
    if (arquivo == 0) {
        return COMBATE_ERRO_ARQUIVO;
    }
    escreveResumoCombate(arquivo, combate);
    if (ferror(arquivo)) {
        fclose(arquivo);
        return COMBATE_ERRO_ARQUIVO;
    }
    if (fclose(arquivo) != 0) {
        return COMBATE_ERRO_ARQUIVO;
    }
    return COMBATE_OK;
}

ResultadoCombate atualizaPersonagensAposCombate(Combate *combate) {
    int i;
    int vida;
    int ativos;
    int moedas;
    int experiencia;
    int parte;

    if (combate == 0) {
        return COMBATE_ARGUMENTO_NULO;
    }
    if (combate->quantidadeOrdem == 0) {
        return COMBATE_NAO_PREPARADO;
    }
    if (!combate->encerrado) {
        return COMBATE_NAO_ENCERRADO;
    }
    for (i = 0; i < combate->quantidadePersonagens; i++) {
        vida = combate->personagens[i].pontosVidaAtuais;
        if (vida < 0) {
            vida = 0;
        }
        if (vida > combate->personagens[i].pontosVidaMaximos) {
            vida = combate->personagens[i].pontosVidaMaximos;
        }
        if (vida > combate->personagens[i].ficha->pontos_vida_maximos) {
            vida = combate->personagens[i].ficha->pontos_vida_maximos;
        }
        combate->personagens[i].ficha->pontos_vida_atuais = vida;
    }
    if (!combate->fichasAtualizadas) {
        ativos = contaPersonagensAtivos(combate);
        moedas = calculaRecompensaMoedas(combate);
        experiencia = calculaRecompensaXP(combate);
        combate->moedasDistribuidas = 0;
        combate->xpDistribuido = 0;
        combate->niveisGanhos = 0;
        for (i = 0; i < combate->quantidadePersonagens && ativos > 0; i++) {
            if (combate->personagens[i].estado == COMBATENTE_ATIVO) {
                parte = moedas / ativos;
                if (parte > PERSONAGEM_MOEDAS_MAXIMO - combate->personagens[i].ficha->moedas) {
                    parte = PERSONAGEM_MOEDAS_MAXIMO - combate->personagens[i].ficha->moedas;
                }
                personagem_alterar_moedas(combate->personagens[i].ficha, parte);
                combate->moedasDistribuidas += parte;
                combate->niveisGanhos += personagem_ganhar_experiencia(combate->personagens[i].ficha,
                                                                       experiencia / ativos);
                combate->xpDistribuido += experiencia / ativos;
            }
        }
    }
    encerraBonusTemporarios(combate);
    combate->fichasAtualizadas = 1;
    return COMBATE_OK;
}
