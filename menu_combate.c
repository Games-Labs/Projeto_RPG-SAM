#include "menu_combate.h"

#include "entrada.h"
#include "tela.h"

#include <stdio.h>
#include <string.h>

void tocarSomFimCombate(Som *som, const Combate *combate) {
    if (combate == 0 || !combate->encerrado) {
        return;
    }
    if (combate->vencedor == LADO_PERSONAGENS) {
        som_toca(som, combate->fichasAtualizadas && combate->niveisGanhos > 0 ? SOM_NIVEL : SOM_VITORIA);
    } else if (combate->vencedor == LADO_MONSTROS) {
        som_toca(som, SOM_DERROTA);
    } else if (combate->grupoFugiu) {
        som_toca(som, SOM_FUGA);
    }
}

static void finalizarSeEncerrado(Combate *combate, Som *som) {
    int confirma;

    if (!combate->encerrado || combate->efeitosResolvidos) {
        return;
    }
    exibeResumoCombate(combate);
    confirma = ler_inteiro_faixa("Aplicar os PV restantes as fichas dos personagens? (1-Sim 0-Nao): ", 0, 1);
    if (confirma == 1 && atualizaPersonagensAposCombate(combate) == COMBATE_OK) {
        printf("Fichas dos personagens atualizadas com os PV restantes.\n");
        if (combate->moedasDistribuidas > 0) {
            printf("Recompensa: %d moeda(s) divididas entre os personagens de pe.\n", combate->moedasDistribuidas);
        }
        if (combate->xpDistribuido > 0) {
            printf("Experiencia: %d XP distribuidos; %d nivel(is) ganho(s).\n", combate->xpDistribuido,
                   combate->niveisGanhos);
        }
    } else {
        encerraBonusTemporarios(combate);
        printf("Efeitos descartados. As fichas permanecem como estavam antes do combate.\n");
    }
    tocarSomFimCombate(som, combate);
    combate->efeitosResolvidos = 1;
}

static void gravarRelatorio(const Combate *combate) {
    char nome[200];
    ResultadoCombate resultado;

    if (combate->quantidadeOrdem == 0) {
        printf("%s\n", resultadoCombateParaTexto(COMBATE_NAO_PREPARADO));
        return;
    }
    ler_texto("Arquivo do relatorio (vazio para relatorio-combate.txt): ", nome, sizeof(nome));
    if (nome[0] == '\0') {
        strcpy(nome, "relatorio-combate.txt");
    }
    resultado = salvaResumoCombate(combate, nome);
    printf("%s\n", resultadoCombateParaTexto(resultado));
    if (resultado == COMBATE_OK) {
        printf("Relatorio gravado em %s.\n", nome);
    }
}

static void prepararCombate(Combate *combate, CadastroPersonagens *cadastro, const EstadoEncontro *encontro,
                            const Bestiario *b) {
    ResultadoCombate resultado;
    int indice = encontro->propostas.indiceEscolhido;

    if (indice < 0 || indice >= encontro->propostas.quantidade) {
        printf("Nenhum encontro escolhido. Gere e escolha uma proposta no menu de encontros.\n");
        return;
    }
    resultado = preparaCombate(combate, cadastro, &encontro->grupo, &encontro->propostas.propostas[indice], b);
    printf("%s\n", resultadoCombateParaTexto(resultado));
    if (resultado == COMBATE_OK) {
        exibeParticipantes(combate);
        exibeOrdem(combate);
    }
}

static int combatePronto(const Combate *combate) {
    if (combate->quantidadeOrdem == 0) {
        printf("%s\n", resultadoCombateParaTexto(COMBATE_NAO_PREPARADO));
        return 0;
    }
    if (combate->encerrado) {
        printf("%s Prepare um novo combate.\n", resultadoCombateParaTexto(COMBATE_ENCERRADO));
        return 0;
    }
    return 1;
}

static void configurarHabilidades(Combate *combate) {
    int i;
    int opcao;
    int temCura = 0;
    int temMagia = 0;
    Dado cura = {0, 0, 0};
    Dado magia = {0, 0, 0};

    if (!combatePronto(combate)) {
        return;
    }
    for (i = 0; i < combate->quantidadePersonagens; i++) {
        printf("%d - %s (cura: %s, magia: %s)\n", i + 1, combate->personagens[i].ficha->nome,
               combate->personagens[i].temCura ? "sim" : "nao", combate->personagens[i].temMagia ? "sim" : "nao");
    }
    i = ler_inteiro_faixa("Personagem: ", 1, combate->quantidadePersonagens) - 1;
    opcao = ler_inteiro_faixa("Cura em aliado com PV <= 50% ao inves de atacar (1-Sim 0-Nao): ", 0, 1);
    if (opcao == 1) {
        temCura = 1;
        ler_dado("Dado de cura (exemplo 1d8): ", &cura);
    }
    opcao = ler_inteiro_faixa("Magia de dano que ignora a defesa ao inves de atacar (1-Sim 0-Nao): ", 0, 1);
    if (opcao == 1) {
        temMagia = 1;
        ler_dado("Dado de dano da magia (exemplo 2d6): ", &magia);
    }
    printf("%s\n", resultadoCombateParaTexto(configuraHabilidadesPersonagem(combate, i, temCura, cura, temMagia, magia)));
}

void usarItemForaDeCombate(Personagem *personagem, char aviso[]) {
    UsoItem uso;
    ResultadoOperacao resultado;
    int id;

    if (personagem == 0) {
        return;
    }
    personagem_listar_inventario(personagem);
    id = ler_inteiro("ID do item a usar (0 para cancelar): ");
    if (id == 0) {
        return;
    }
    resultado = personagem_usar_item(personagem, id, &uso);
    if (resultado == RESULTADO_OK) {
        personagem_descrever_uso(personagem, &uso, aviso, TELA_AVISO_TAMANHO);
    } else {
        snprintf(aviso, TELA_AVISO_TAMANHO, "%s", resultado_operacao_para_texto(resultado));
    }
}

void usarItemEmCombate(Combate *combate) {
    char texto[TELA_AVISO_TAMANHO];
    UsoItem uso;
    ResultadoCombate resultado;
    int indice;
    int id;
    int i;

    if (!combatePronto(combate)) {
        return;
    }
    for (i = 0; i < combate->quantidadePersonagens; i++) {
        printf("%d - %s | PV %d/%d | %s\n", i + 1, combate->personagens[i].ficha->nome,
               combate->personagens[i].pontosVidaAtuais, combate->personagens[i].pontosVidaMaximos,
               estadoCombatenteParaTexto(combate->personagens[i].estado));
    }
    indice = ler_inteiro_faixa("Personagem que usa o item (0 para cancelar): ", 0, combate->quantidadePersonagens) - 1;
    if (indice < 0) {
        return;
    }
    personagem_listar_inventario(combate->personagens[indice].ficha);
    id = ler_inteiro("ID do item a usar (0 para cancelar): ");
    if (id == 0) {
        return;
    }
    resultado = usaItemEmCombate(combate, indice, id, &uso);
    if (resultado == COMBATE_OK) {
        personagem_descrever_uso(combate->personagens[indice].ficha, &uso, texto, sizeof(texto));
        printf("%s\n", texto);
        printf("%s esta com %d/%d PV no combate.\n", combate->personagens[indice].ficha->nome,
               combate->personagens[indice].pontosVidaAtuais, combate->personagens[indice].pontosVidaMaximos);
    } else {
        printf("%s\n", resultadoCombateParaTexto(resultado));
    }
}

static void exibirMenu(const Combate *combate) {
    tela_abrir_pagina("Combate");
    if (combateEmAndamento(combate)) {
        printf("Em andamento: rodada %d | personagens ativos %d | monstros ativos %d\n", combate->rodadaAtual,
               contaPersonagensAtivos(combate), contaMonstrosAtivos(combate));
        tela_linha();
    }
    printf("1 - Preparar combate com o encontro escolhido\n");
    printf("2 - Exibir participantes e ordem\n");
    printf("3 - Executar uma rodada\n");
    printf("4 - Executar ate o encerramento\n");
    printf("5 - Exibir estado atual\n");
    printf("6 - Exibir resumo do ultimo combate\n");
    printf("7 - Gravar relatorio do combate em arquivo\n");
    printf("8 - Configurar cura/magia de um personagem\n");
    printf("9 - Tentar fugir do encontro (d20 + iniciativa do mais lento contra %d)\n", FUGA_DIFICULDADE);
    printf("10 - Usar item de um personagem\n");
    printf("0 - Voltar\n");
}

void menuCombate(Combate *combate, CadastroPersonagens *cadastro, const EstadoEncontro *encontro, const Bestiario *b,
                 Som *som) {
    int opcao;

    do {
        exibirMenu(combate);
        opcao = ler_inteiro("Opcao: ");

        if (opcao == 1) {
            prepararCombate(combate, cadastro, encontro, b);
        } else if (opcao == 2) {
            exibeParticipantes(combate);
            exibeOrdem(combate);
        } else if (opcao == 3) {
            if (combatePronto(combate)) {
                executaRodada(combate);
                finalizarSeEncerrado(combate, som);
            }
        } else if (opcao == 4) {
            if (combatePronto(combate)) {
                executaCombate(combate);
                finalizarSeEncerrado(combate, som);
            }
        } else if (opcao == 5) {
            exibeEstadoCombate(combate);
        } else if (opcao == 6) {
            exibeResumoCombate(combate);
        } else if (opcao == 7) {
            gravarRelatorio(combate);
        } else if (opcao == 8) {
            configurarHabilidades(combate);
        } else if (opcao == 9) {
            if (combatePronto(combate)) {
                tentaFugaGrupo(combate);
                finalizarSeEncerrado(combate, som);
            }
        } else if (opcao == 10) {
            usarItemEmCombate(combate);
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }
        if (opcao != 0) {
            tela_pausar();
        }
    } while (opcao != 0);
}
