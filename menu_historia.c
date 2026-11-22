#include "historia.h"

#include "cidade.h"
#include "entrada.h"
#include "menu_combate.h"
#include "tela.h"

#include <stdio.h>

#define SECAO_TAMANHO 40
#define ARQUIVO_TAMANHO 200

static void narrar(const char *secao) {
    int pulado = 0;

    if (tela_interativa()) {
        printf("(Pressione qualquer tecla para pular a animacao.)\n\n");
    }
    if (historia_exibe_secao(ARQUIVO_HISTORIA, secao, &pulado) <= 0) {
        printf("Texto \"%s\" nao encontrado em %s.\n", secao, ARQUIVO_HISTORIA);
    }
}

ModoJogo historiaAbertura(Som *som) {
    int pulado = 0;
    int escolha;

    tela_abrir_pagina("");
    som_toca(som, SOM_ABERTURA);
    if (tela_interativa()) {
        printf("(Pressione qualquer tecla para pular a animacao.)\n\n");
    }
    if (historia_exibe_secao(ARQUIVO_HISTORIA, "ABERTURA", &pulado) <= 0) {
        printf("Bem-vindo a Falmuth.\n");
        printf("[1] Sistema Auxiliar de Mestre de RPG\n");
        printf("[2] Modo Historia\n");
    }
    printf("\n");
    escolha = ler_inteiro_faixa("Sua escolha (1 ou 2): ", MODO_MESTRE, MODO_HISTORIA);
    if (escolha != MODO_HISTORIA) {
        escolha = MODO_MESTRE;
    }
    tela_abrir_pagina("");
    narrar(escolha == MODO_MESTRE ? "ESCOLHA_MESTRE" : "ESCOLHA_HISTORIA");
    tela_pausar();
    return (ModoJogo)escolha;
}

static void exibirHerois(const CadastroPersonagens *cadastro, const GrupoAventura *grupo) {
    const Personagem *personagem;
    int i;

    if (grupo->quantidade == 0) {
        printf("Nenhum heroi no grupo. Crie um heroi (opcao 2) para comecar.\n");
        return;
    }
    for (i = 0; i < grupo->quantidade; i++) {
        personagem = cadastro_buscar_por_id_const(cadastro, grupo->idsPersonagens[i]);
        if (personagem == 0) {
            printf("%d - ID %d removido do cadastro\n", i + 1, grupo->idsPersonagens[i]);
        } else {
            printf("%d - %s (%s %s) | Nivel %d (XP %d/%d) | PV %d/%d | Moedas %d\n", i + 1, personagem->nome,
                   raca_para_texto(personagem->raca), classe_para_texto(personagem->classe), personagem->nivel,
                   personagem->experiencia, personagem_xp_proximo_nivel(personagem),
                   personagem->pontos_vida_atuais, personagem->pontos_vida_maximos, personagem->moedas);
        }
    }
}

static Personagem *escolherHeroi(CadastroPersonagens *cadastro, const GrupoAventura *grupo, char aviso[]) {
    Personagem *personagem;
    int indice = 1;

    if (grupo->quantidade == 0) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "O grupo esta vazio. Crie um heroi primeiro.");
        return 0;
    }
    if (grupo->quantidade > 1) {
        exibirHerois(cadastro, grupo);
        indice = ler_inteiro_faixa("Qual heroi? (0 para cancelar): ", 0, grupo->quantidade);
        if (indice < 1) {
            return 0;
        }
    }
    personagem = cadastro_buscar_por_id(cadastro, grupo->idsPersonagens[indice - 1]);
    if (personagem == 0) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Esse heroi nao existe mais no cadastro. Refaca o grupo (opcao 3).");
    }
    return personagem;
}

static void criarHeroi(CadastroPersonagens *cadastro, GrupoAventura *grupo, char aviso[]) {
    char nome[PERSONAGEM_NOME_TAMANHO];
    Personagem heroi;
    ResultadoOperacao resultado;
    int raca;
    int classe;

    if (grupo->quantidade >= MAX_PERSONAGENS_GRUPO) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "O grupo ja tem %d herois.", MAX_PERSONAGENS_GRUPO);
        return;
    }
    tela_abrir_pagina("Novo heroi");
    if (!ler_texto_obrigatorio("Nome do heroi: ", nome, sizeof(nome))) {
        return;
    }
    printf("Racas: 1-Humano 2-Elfo 3-Anao 4-Halfling 5-Orc\n");
    raca = ler_inteiro_faixa("Raca: ", RACA_HUMANO, RACA_ORC);
    printf("Classes: 1-Guerreiro 2-Ladino 3-Mago 4-Clerigo 5-Bardo\n");
    classe = ler_inteiro_faixa("Classe: ", CLASSE_GUERREIRO, CLASSE_BARDO);
    if (raca < RACA_HUMANO || classe < CLASSE_GUERREIRO) {
        return;
    }
    heroi = historia_cria_heroi(historia_proximo_id(cadastro), nome, (Raca)raca, (Classe)classe);
    resultado = cadastro_cadastrar(cadastro, heroi);
    if (resultado != RESULTADO_OK) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Nao foi possivel criar o heroi: %s", resultado_operacao_para_texto(resultado));
        return;
    }
    adicionaPersonagemGrupo(grupo, cadastro, heroi.id);
    calculaPoderGrupo(grupo, cadastro);
    snprintf(aviso, TELA_AVISO_TAMANHO, "%s (ID %d) entrou no grupo com %d moedas e uma arma inicial.", heroi.nome,
             heroi.id, heroi.moedas);
}

static void gerenciarGrupo(CadastroPersonagens *cadastro, GrupoAventura *grupo) {
    char aviso[TELA_AVISO_TAMANHO] = "";
    int opcao;
    int id;

    do {
        tela_abrir_pagina("Grupo de herois");
        tela_exibir_aviso(aviso);
        calculaPoderGrupo(grupo, cadastro);
        exibeGrupo(grupo, cadastro);
        tela_linha();
        printf("Personagens cadastrados:\n");
        cadastro_listar(cadastro);
        tela_linha();
        printf("1 - Adicionar personagem cadastrado ao grupo\n");
        printf("2 - Remover personagem do grupo\n");
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1) {
            id = ler_inteiro("ID do personagem: ");
            snprintf(aviso, sizeof(aviso), "%s", resultadoGrupoParaTexto(adicionaPersonagemGrupo(grupo, cadastro, id)));
        } else if (opcao == 2) {
            id = ler_inteiro("ID do personagem: ");
            snprintf(aviso, sizeof(aviso), "%s", resultadoGrupoParaTexto(removePersonagemGrupo(grupo, id)));
        } else if (opcao != 0) {
            snprintf(aviso, sizeof(aviso), "Opcao invalida.");
        }
    } while (opcao != 0);
}

void historiaLerCapitulos(int liberados) {
    static const char *const NUMERAIS[HISTORIA_NUM_CAPITULOS] = {"I", "II", "III", "IV"};
    char secao[SECAO_TAMANHO];
    int opcao;
    int i;

    do {
        tela_abrir_pagina("A historia de Falmuth");
        printf("1 - Prologo\n");
        for (i = 1; i <= HISTORIA_NUM_CAPITULOS; i++) {
            printf("%d - Capitulo %s%s\n", i + 1, NUMERAIS[i - 1],
                   i <= liberados ? "" : " (bloqueado: conclua a missao principal correspondente)");
        }
        printf("0 - Voltar\n");
        opcao = ler_inteiro_faixa("Opcao: ", 0, HISTORIA_NUM_CAPITULOS + 1);
        if (opcao >= 2 && opcao - 1 > liberados) {
            printf("Capitulo bloqueado. Conclua a missao principal %d no Modo Historia.\n", opcao - 1);
            tela_pausar();
        } else if (opcao >= 1) {
            if (opcao == 1) {
                snprintf(secao, sizeof(secao), "PROLOGO");
            } else {
                snprintf(secao, sizeof(secao), "CAPITULO_%d", opcao - 1);
            }
            tela_abrir_pagina("");
            narrar(secao);
            tela_pausar();
        }
    } while (opcao >= 1);
}

static void lutar(Combate *combate, Som *som) {
    int opcao;

    while (combateEmAndamento(combate)) {
        tela_abrir_pagina("Combate");
        exibeEstadoCombate(combate);
        tela_linha();
        printf("1 - Lutar uma rodada\n");
        printf("2 - Lutar ate o fim\n");
        printf("3 - Usar item\n");
        printf("4 - Tentar fugir (d20 + iniciativa do mais lento contra %d)\n", FUGA_DIFICULDADE);
        opcao = ler_inteiro_faixa("Acao: ", 1, 4);
        if (opcao == 1) {
            executaRodada(combate);
        } else if (opcao == 3) {
            usarItemEmCombate(combate);
        } else if (opcao == 4) {
            tentaFugaGrupo(combate);
        } else {
            executaCombate(combate);
        }
        tela_pausar();
    }
    if (combate->quantidadeOrdem == 0 || combate->efeitosResolvidos) {
        return;
    }
    tela_abrir_pagina("Fim do combate");
    exibeResumoCombate(combate);
    if (atualizaPersonagensAposCombate(combate) == COMBATE_OK) {
        printf("\nOs PV restantes foram aplicados as fichas.\n");
        if (combate->moedasDistribuidas > 0) {
            printf("Recompensa: %d moeda(s) divididas entre os herois de pe.\n", combate->moedasDistribuidas);
        }
        if (combate->xpDistribuido > 0) {
            printf("Experiencia: %d XP distribuidos; %d nivel(is) ganho(s).\n", combate->xpDistribuido,
                   combate->niveisGanhos);
        }
    }
    tocarSomFimCombate(som, combate);
    combate->efeitosResolvidos = 1;
    tela_pausar();
}

static int sorteiaEncontro(EstadoEncontro *estado, const Bestiario *bestiario, ContextoEncontro contexto) {
    ResultadoEncontro gerado;
    int sorteada;

    estado->contexto = contexto;
    estado->contextoDefinido = 1;
    calculaFaixaDificuldade(estado->grupo.poderTotal, contexto.dificuldade, &estado->faixaMinima, &estado->faixaMaxima);
    inicializaPropostas(&estado->propostas);
    gerado = geraPropostas(bestiario, &estado->grupo, &contexto, &estado->propostas);
    if ((gerado != ENCONTRO_OK && gerado != ENCONTRO_PARCIAL) || estado->propostas.quantidade == 0) {
        inicializaPropostas(&estado->propostas);
        return 0;
    }
    sorteada = sorteiaInteiro(1, estado->propostas.quantidade);
    escolheProposta(&estado->propostas, sorteada);
    return sorteada;
}

static int iniciaCombateSorteado(CadastroPersonagens *cadastro, EstadoEncontro *estado, Combate *combate,
                                 const Bestiario *bestiario, int sorteada, char aviso[], Som *som) {
    ResultadoCombate preparado;

    printf("\n%s, %s. O grupo encontra:\n", terrenoParaTexto(estado->contexto.terreno),
           periodoParaTexto(estado->contexto.periodo));
    exibeEncontro(&estado->propostas.propostas[sorteada - 1], bestiario);
    preparado = preparaCombate(combate, cadastro, &estado->grupo, &estado->propostas.propostas[sorteada - 1], bestiario);
    if (preparado != COMBATE_OK) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "O grupo recua: %s", resultadoCombateParaTexto(preparado));
        return 0;
    }
    tela_pausar();
    lutar(combate, som);
    return 1;
}

static int procuraEncontroDaMissao(EstadoEncontro *estado, const Bestiario *bestiario, const Missao *missao) {
    ContextoEncontro contexto;
    int terreno;
    int dificuldade;
    int sorteada;

    contexto.periodo = (Periodo)sorteiaInteiro(PERIODO_DIA, PERIODO_NOITE);
    for (terreno = TERRENO_CAVERNA - 1; terreno <= TERRENO_DESERTO; terreno++) {
        contexto.terreno = terreno < TERRENO_CAVERNA ? missao->terreno : (Terreno)terreno;
        for (dificuldade = FACIL - 1; dificuldade <= DIFICIL; dificuldade++) {
            contexto.dificuldade = dificuldade < FACIL ? missao->dificuldade : (Dificuldade)dificuldade;
            sorteada = sorteiaEncontro(estado, bestiario, contexto);
            if (sorteada > 0) {
                return sorteada;
            }
        }
    }
    return 0;
}

static void fazerMissao(CadastroPersonagens *cadastro, EstadoEncontro *estado, Combate *combate,
                        const Bestiario *bestiario, ProgressoHistoria *progresso, int principal, int numero,
                        char aviso[], Som *som) {
    const Missao *missao = principal ? historia_missao_principal(numero) : historia_missao_secundaria(numero);
    char secao[SECAO_TAMANHO];
    int sorteada;
    int premiados;
    int niveis = 0;

    if (missao == 0) {
        return;
    }
    if (estado->grupo.quantidade == 0 || calculaPoderGrupo(&estado->grupo, cadastro) <= 0) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Grupo vazio ou com personagem removido. Crie um heroi ou refaca o grupo.");
        return;
    }
    tela_abrir_pagina(missao->titulo);
    snprintf(secao, sizeof(secao), "MISSAO_%c%d_INICIO", principal ? 'P' : 'S', numero);
    narrar(secao);
    printf("\nLocal: %s | Risco: %s | Recompensa por heroi: %d moedas e %d XP\n", terrenoParaTexto(missao->terreno),
           dificuldadeParaTexto(missao->dificuldade), missao->moedas, missao->experiencia);
    if (ler_inteiro_faixa("Partir agora? (1-Sim 0-Nao): ", 0, 1) != 1) {
        return;
    }
    sorteada = procuraEncontroDaMissao(estado, bestiario, missao);
    if (sorteada == 0) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Nenhum inimigo a altura do grupo foi encontrado. Tente com outro grupo.");
        return;
    }
    if (!iniciaCombateSorteado(cadastro, estado, combate, bestiario, sorteada, aviso, som)) {
        return;
    }
    if (!combate->encerrado || combate->vencedor != LADO_PERSONAGENS) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Missao nao concluida: o grupo nao venceu. E possivel tentar de novo.");
        return;
    }
    if (principal) {
        historia_conclui_principal(progresso, numero);
    } else {
        historia_conclui_secundaria(progresso, numero);
    }
    premiados = historia_recompensa_grupo(cadastro, &estado->grupo, missao, &niveis);
    tela_abrir_pagina("Missao concluida");
    som_toca(som, SOM_MISSAO);
    snprintf(secao, sizeof(secao), "MISSAO_%c%d_FIM", principal ? 'P' : 'S', numero);
    narrar(secao);
    printf("\n%d heroi(s) receberam %d moedas e %d XP cada. Niveis ganhos: %d.\n", premiados, missao->moedas,
           missao->experiencia, niveis);
    if (principal) {
        printf("Capitulo %d da historia de Falmuth liberado na livraria da cidade.\n", numero);
    }
    tela_pausar();
}

static void menuMissoes(CadastroPersonagens *cadastro, EstadoEncontro *estado, Combate *combate,
                        const Bestiario *bestiario, ProgressoHistoria *progresso, Som *som) {
    char aviso[TELA_AVISO_TAMANHO] = "";
    int opcao;
    int i;

    do {
        tela_abrir_pagina("Missoes");
        tela_exibir_aviso(aviso);
        printf("Linha principal (%d de %d concluidas):\n", progresso->principais, HISTORIA_NUM_PRINCIPAIS);
        for (i = 1; i <= HISTORIA_NUM_PRINCIPAIS; i++) {
            printf("%d - %s [%s]\n", i, historia_missao_principal(i)->titulo,
                   i <= progresso->principais ? "concluida"
                   : i == progresso->principais + 1 ? "disponivel" : "bloqueada");
        }
        printf("Historias secundarias (%d de %d concluidas):\n", historia_conta_secundarias(progresso),
               HISTORIA_NUM_SECUNDARIAS);
        for (i = 1; i <= HISTORIA_NUM_SECUNDARIAS; i++) {
            printf("%d - %s [%s]\n", HISTORIA_NUM_PRINCIPAIS + i, historia_missao_secundaria(i)->titulo,
                   historia_secundaria_concluida(progresso, i) ? "concluida" : "disponivel");
        }
        printf("0 - Voltar\n");
        opcao = ler_inteiro_faixa("Missao: ", 0, HISTORIA_NUM_PRINCIPAIS + HISTORIA_NUM_SECUNDARIAS);
        if (opcao >= 1 && opcao <= progresso->principais) {
            snprintf(aviso, sizeof(aviso), "Essa missao ja foi concluida.");
        } else if (opcao > progresso->principais + 1 && opcao <= HISTORIA_NUM_PRINCIPAIS) {
            snprintf(aviso, sizeof(aviso), "Missao bloqueada: conclua antes a missao principal %d.",
                     progresso->principais + 1);
        } else if (opcao >= 1 && opcao <= HISTORIA_NUM_PRINCIPAIS) {
            fazerMissao(cadastro, estado, combate, bestiario, progresso, 1, opcao, aviso, som);
        } else if (opcao > HISTORIA_NUM_PRINCIPAIS
                   && historia_secundaria_concluida(progresso, opcao - HISTORIA_NUM_PRINCIPAIS)) {
            snprintf(aviso, sizeof(aviso), "Essa historia secundaria ja foi concluida.");
        } else if (opcao > HISTORIA_NUM_PRINCIPAIS) {
            fazerMissao(cadastro, estado, combate, bestiario, progresso, 0, opcao - HISTORIA_NUM_PRINCIPAIS, aviso, som);
        }
    } while (opcao >= 1);
}

static void lerNomeArquivo(const char *padrao, char nome[], int tamanho) {
    char mensagem[ARQUIVO_TAMANHO];

    snprintf(mensagem, sizeof(mensagem), "Arquivo (vazio para %.100s): ", padrao);
    ler_texto(mensagem, nome, tamanho);
    if (nome[0] == '\0') {
        snprintf(nome, (size_t)tamanho, "%.100s", padrao);
    }
}

int jogoSalvar(const CadastroPersonagens *cadastro, const GrupoAventura *grupo, const ProgressoHistoria *progresso,
               const char *padrao, char aviso[]) {
    char nome[ARQUIVO_TAMANHO];
    EstadoSalvo estado;
    int i;

    if (cadastro == 0 || grupo == 0 || progresso == 0 || padrao == 0 || aviso == 0) {
        return 0;
    }
    inicializaEstadoSalvo(&estado);
    for (i = 0; i < grupo->quantidade && i < MAX_PERSONAGENS_GRUPO; i++) {
        if (cadastro_buscar_indice_por_id(cadastro, grupo->idsPersonagens[i]) >= 0) {
            estado.idsGrupo[estado.quantidadeGrupo++] = grupo->idsPersonagens[i];
        }
    }
    estado.missoesPrincipais = progresso->principais;
    estado.missoesSecundarias = progresso->secundarias;
    lerNomeArquivo(padrao, nome, sizeof(nome));
    if (salvaJogo(cadastro, &estado, nome) != 0) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Nao foi possivel gravar o arquivo %.150s.", nome);
        return 0;
    }
    snprintf(aviso, TELA_AVISO_TAMANHO, "%d personagem(ns) gravado(s) em %.150s (grupo e progresso incluidos).",
             cadastro->quantidade, nome);
    return 1;
}

int jogoCarregar(CadastroPersonagens *cadastro, GrupoAventura *grupo, ProgressoHistoria *progresso,
                 const char *padrao, char aviso[]) {
    char nome[ARQUIVO_TAMANHO];
    EstadoSalvo estado;
    int resultado;
    int i;

    if (cadastro == 0 || grupo == 0 || progresso == 0 || padrao == 0 || aviso == 0) {
        return 0;
    }
    inicializaEstadoSalvo(&estado);
    lerNomeArquivo(padrao, nome, sizeof(nome));
    resultado = carregaJogo(cadastro, &estado, nome);
    if (resultado == PERSISTENCIA_ERRO_ARQUIVO) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Nao foi possivel abrir o arquivo %.150s.", nome);
        return 0;
    }
    if (resultado != 0) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Erro na linha %d de %.150s. Nada foi alterado.", resultado, nome);
        return 0;
    }
    inicializaGrupo(grupo);
    for (i = 0; i < estado.quantidadeGrupo; i++) {
        adicionaPersonagemGrupo(grupo, cadastro, estado.idsGrupo[i]);
    }
    calculaPoderGrupo(grupo, cadastro);
    historia_define_progresso(progresso, estado.missoesPrincipais, estado.missoesSecundarias);
    snprintf(aviso, TELA_AVISO_TAMANHO, "%d personagem(ns) carregado(s) de %.150s. Grupo com %d.",
             cadastro->quantidade, nome, grupo->quantidade);
    return 1;
}

static void explorar(CadastroPersonagens *cadastro, EstadoEncontro *estado, Combate *combate,
                     const Bestiario *bestiario, char aviso[], Som *som) {
    ContextoEncontro contexto;
    int terreno;
    int dificuldade;
    int sorteada;

    if (combateEmAndamento(combate)) {
        lutar(combate, som);
        return;
    }
    if (estado->grupo.quantidade == 0 || calculaPoderGrupo(&estado->grupo, cadastro) <= 0) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Grupo vazio ou com personagem removido. Use as opcoes 2 ou 3.");
        return;
    }
    tela_abrir_pagina("Explorar os arredores de Falmuth");
    printf("Terrenos: 1-Caverna 2-Ruinas 3-Floresta 4-Montanha 5-Planicie 6-Pantano 7-Urbano 8-Deserto\n");
    terreno = ler_inteiro("Para onde ir (0 para cancelar): ");
    if (terreno == 0) {
        return;
    }
    printf("Dificuldades: 1-Facil 2-Media 3-Dificil\n");
    dificuldade = ler_inteiro("Quanto risco aceitar: ");
    contexto.terreno = (Terreno)terreno;
    contexto.dificuldade = (Dificuldade)dificuldade;
    contexto.periodo = (Periodo)sorteiaInteiro(PERIODO_DIA, PERIODO_NOITE);
    if (!contextoEhValido(&contexto)) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "%s", resultadoEncontroParaTexto(ENCONTRO_CONTEXTO_INVALIDO));
        return;
    }

    sorteada = sorteiaEncontro(estado, bestiario, contexto);
    if (sorteada == 0) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "A regiao esta calma (%s, %s). Nenhum encontro a altura do grupo.",
                 terrenoParaTexto(contexto.terreno), periodoParaTexto(contexto.periodo));
        return;
    }
    iniciaCombateSorteado(cadastro, estado, combate, bestiario, sorteada, aviso, som);
}

static void descansar(CadastroPersonagens *cadastro, const EstadoEncontro *estado, const Combate *combate, char aviso[]) {
    if (combateEmAndamento(combate)) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Nao da para descansar no meio de um combate.");
    } else if (estado->grupo.quantidade == 0) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "O grupo esta vazio.");
    } else {
        snprintf(aviso, TELA_AVISO_TAMANHO, "O grupo acampou. PV recuperados no total: %d (1d8 por heroi).",
                 historia_descansa_grupo(cadastro, &estado->grupo));
    }
}

SaidaHistoria menuHistoria(CadastroPersonagens *cadastro, EstadoEncontro *encontro, Combate *combate,
                           const Bestiario *bestiario, const CatalogoItens *catalogo, ProgressoHistoria *progresso,
                           Som *som) {
    char aviso[TELA_AVISO_TAMANHO] = "";
    Personagem *heroi;
    int opcao;

    do {
        tela_abrir_pagina("Falmuth - Modo Historia");
        tela_exibir_aviso(aviso);
        exibirHerois(cadastro, &encontro->grupo);
        printf("Historia: %d de %d capitulos liberados | Secundarias: %d de %d\n",
               historia_capitulos_liberados(progresso), HISTORIA_NUM_CAPITULOS,
               historia_conta_secundarias(progresso), HISTORIA_NUM_SECUNDARIAS);
        tela_linha();
        printf(" 1 - Missoes (linha principal e historias secundarias)\n");
        printf(" 2 - Criar heroi\n");
        printf(" 3 - Grupo (adicionar ou remover personagens)\n");
        printf(" 4 - Visitar a cidade (lojas, livraria com a historia de Falmuth...)\n");
        printf(" 5 - %s\n", combateEmAndamento(combate) ? "Voltar ao combate em andamento" : "Explorar e procurar um encontro");
        printf(" 6 - Descansar no acampamento\n");
        printf(" 7 - Usar item da bolsa\n");
        printf(" 8 - Salvar jogo (herois, grupo e progresso)\n");
        printf(" 9 - Carregar jogo\n");
        printf("10 - Som: %s (escolha para alternar)\n", som != 0 && som->ligado ? "ligado" : "desligado");
        printf(" 0 - Sair do jogo\n");
        printf("%d - Painel SAM (Sistema Auxiliar de Mestre)\n", HISTORIA_CODIGO_PAINEL);
        opcao = ler_inteiro("Opcao: ");

        if ((opcao == 1 || opcao == 9) && combateEmAndamento(combate)) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "Ha um combate em andamento. Use a opcao 5 para continuar.");
        } else if (opcao == 1) {
            menuMissoes(cadastro, encontro, combate, bestiario, progresso, som);
        } else if (opcao == 8) {
            jogoSalvar(cadastro, &encontro->grupo, progresso, ARQUIVO_JOGO_HISTORIA, aviso);
        } else if (opcao == 9) {
            if (jogoCarregar(cadastro, &encontro->grupo, progresso, ARQUIVO_JOGO_HISTORIA, aviso)) {
                inicializaCombate(combate);
            }
        } else if (opcao == 2) {
            criarHeroi(cadastro, &encontro->grupo, aviso);
        } else if (opcao == 3) {
            gerenciarGrupo(cadastro, &encontro->grupo);
        } else if ((opcao == 4 || opcao == 7) && combateEmAndamento(combate)) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "Ha um combate em andamento. Use a opcao 5 para continuar.");
        } else if (opcao == 4) {
            heroi = escolherHeroi(cadastro, &encontro->grupo, aviso);
            if (heroi != 0) {
                menuCidade(heroi, catalogo, historia_capitulos_liberados(progresso), som);
            }
        } else if (opcao == 5) {
            explorar(cadastro, encontro, combate, bestiario, aviso, som);
        } else if (opcao == 6) {
            descansar(cadastro, encontro, combate, aviso);
        } else if (opcao == 7) {
            heroi = escolherHeroi(cadastro, &encontro->grupo, aviso);
            if (heroi != 0) {
                usarItemForaDeCombate(heroi, aviso);
            }
        } else if (opcao == 10) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "Som %s.", som_alterna(som) ? "ligado" : "desligado");
        } else if (opcao == HISTORIA_CODIGO_PAINEL) {
            return HISTORIA_ABRIR_PAINEL;
        } else if (opcao != 0) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "Opcao invalida.");
        }
    } while (opcao != 0);

    return HISTORIA_SAIR;
}
