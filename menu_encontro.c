#include "texto.h"

#include "menu_encontro.h"

#include "entrada.h"
#include "tela.h"

#include <stdio.h>

void inicializaEstadoEncontro(EstadoEncontro *estado) {
    inicializaGrupo(&estado->grupo);
    estado->contextoDefinido = 0;
    estado->faixaMinima = 0;
    estado->faixaMaxima = 0;
    inicializaPropostas(&estado->propostas);
}

static void selecionarGrupo(EstadoEncontro *estado, const CadastroPersonagens *cadastro) {
    char aviso[TELA_AVISO_TAMANHO] = "";
    int opcao;
    int id;

    do {
        tela_abrir_pagina("Encontros - Grupo");
        tela_exibir_aviso(aviso);
        cadastro_listar(cadastro);
        tela_linha();
        exibeGrupo(&estado->grupo, cadastro);
        printf("\nGrupo atual: %d/%d personagem(ns)\n", estado->grupo.quantidade, MAX_PERSONAGENS_GRUPO);
        printf("1 - Adicionar personagem por ID\n2 - Remover personagem por ID\n3 - Limpar grupo\n0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1) {
            id = ler_inteiro("ID do personagem: ");
            snprintf(aviso, sizeof(aviso), "%s",
                     resultadoGrupoParaTexto(adicionaPersonagemGrupo(&estado->grupo, cadastro, id)));
        } else if (opcao == 2) {
            id = ler_inteiro("ID do personagem: ");
            snprintf(aviso, sizeof(aviso), "%s", resultadoGrupoParaTexto(removePersonagemGrupo(&estado->grupo, id)));
        } else if (opcao == 3) {
            inicializaGrupo(&estado->grupo);
            snprintf(aviso, sizeof(aviso), "Grupo esvaziado.");
        } else if (opcao != 0) {
            snprintf(aviso, sizeof(aviso), "Opcao invalida.");
        }
        calculaPoderGrupo(&estado->grupo, cadastro);
    } while (opcao != 0);
}

static void exibirGrupo(EstadoEncontro *estado, const CadastroPersonagens *cadastro) {
    if (estado->grupo.quantidade > 0 && calculaPoderGrupo(&estado->grupo, cadastro) < 0) {
        printf("Algum personagem do grupo nao existe mais no cadastro. Refaca o grupo.\n");
    }
    exibeGrupo(&estado->grupo, cadastro);
}

static void informarContexto(EstadoEncontro *estado) {
    ContextoEncontro novo;

    printf("Terrenos: 1-Caverna 2-Ruinas 3-Floresta 4-Montanha 5-Planicie 6-Pantano 7-Urbano 8-Deserto\n");
    novo.terreno = (Terreno)ler_inteiro("Terreno: ");
    printf("Periodos: 1-Dia 2-Noite\n");
    novo.periodo = (Periodo)ler_inteiro("Periodo: ");
    printf("Dificuldades: 1-Facil 2-Media 3-Dificil\n");
    novo.dificuldade = (Dificuldade)ler_inteiro("Dificuldade: ");

    if (!contextoEhValido(&novo)) {
        printf("%s Contexto anterior mantido.\n", resultadoEncontroParaTexto(ENCONTRO_CONTEXTO_INVALIDO));
        return;
    }
    estado->contexto = novo;
    estado->contextoDefinido = 1;
    inicializaPropostas(&estado->propostas);
    printf("Contexto: %s, %s, %s.\n", terrenoParaTexto(novo.terreno), periodoParaTexto(novo.periodo),
           dificuldadeParaTexto(novo.dificuldade));
}

static int contextoPronto(const EstadoEncontro *estado) {
    if (!estado->contextoDefinido) {
        printf("Informe terreno, periodo e dificuldade primeiro (opcao 3).\n");
        return 0;
    }
    return 1;
}

static void exibirCandidatos(const EstadoEncontro *estado, const Bestiario *b) {
    ListaCandidatos candidatos;
    ResultadoEncontro resultado;
    int i;

    if (!contextoPronto(estado)) {
        return;
    }
    resultado = filtraCandidatos(b, &estado->contexto, &candidatos);
    if (resultado != ENCONTRO_OK) {
        printf("%s\n", resultadoEncontroParaTexto(resultado));
        return;
    }
    printf("=== Monstros elegiveis ===\n");
    printf(" ID | Nome                     | Frequencia  | Peso   | Poder\n");
    for (i = 0; i < candidatos.quantidade; i++) {
        printf("%3d | ",candidatos.itens[i]->id);
        texto_imprime_alinhado(stdout, candidatos.itens[i]->nome, 24);
        printf(" | %-11s | peso %2d | poder individual %d\n",
               frequenciaParaTexto(candidatos.itens[i]->frequencia),
               pesoFrequencia(candidatos.itens[i]->frequencia),
               poderIndividualContextual(candidatos.itens[i], &estado->contexto));
    }
    printf("Total: %d candidato(s).\n", candidatos.quantidade);
}

static void exibirPropostas(const EstadoEncontro *estado, const Bestiario *b) {
    int i;

    printf("Faixa de poder solicitada: %d a %d\n", estado->faixaMinima, estado->faixaMaxima);
    for (i = 0; i < estado->propostas.quantidade; i++) {
        printf("Proposta %d:\n", i + 1);
        exibeEncontro(&estado->propostas.propostas[i], b);
    }
}

static void gerarPropostas(EstadoEncontro *estado, const CadastroPersonagens *cadastro, const Bestiario *b) {
    ResultadoEncontro resultado;

    if (!contextoPronto(estado)) {
        return;
    }
    if (estado->grupo.quantidade == 0 || calculaPoderGrupo(&estado->grupo, cadastro) <= 0) {
        printf("%s\n", resultadoEncontroParaTexto(ENCONTRO_GRUPO_INVALIDO));
        return;
    }

    calculaFaixaDificuldade(estado->grupo.poderTotal, estado->contexto.dificuldade,
                            &estado->faixaMinima, &estado->faixaMaxima);
    resultado = geraPropostas(b, &estado->grupo, &estado->contexto, &estado->propostas);
    printf("Poder do grupo: %d\n", estado->grupo.poderTotal);
    if (resultado == ENCONTRO_OK || resultado == ENCONTRO_PARCIAL) {
        exibirPropostas(estado, b);
    }
    if (resultado != ENCONTRO_OK) {
        printf("%s\n", resultadoEncontroParaTexto(resultado));
    }
}

static void escolherProposta(EstadoEncontro *estado) {
    int opcao;

    if (estado->propostas.quantidade == 0) {
        printf("Nao ha propostas. Gere-as primeiro (opcao 5).\n");
        return;
    }
    opcao = ler_inteiro("Numero da proposta: ");
    if (escolheProposta(&estado->propostas, opcao) == ENCONTRO_OK) {
        printf("Proposta %d escolhida.\n", opcao);
    } else {
        printf("%s\n", resultadoEncontroParaTexto(ENCONTRO_ESCOLHA_INVALIDA));
    }
}

static void montarManual(EstadoEncontro *estado, const CadastroPersonagens *cadastro, const Bestiario *b) {
    Encontro manual;
    ListaCandidatos candidatos;
    ComponenteEncontro *componente;
    const Monstro *m;
    int id;
    int quantidade;
    int i;
    int elegivel;
    int continuar = 1;

    if (!contextoPronto(estado)) {
        return;
    }
    if (estado->grupo.quantidade == 0 || calculaPoderGrupo(&estado->grupo, cadastro) <= 0) {
        printf("%s\n", resultadoEncontroParaTexto(ENCONTRO_GRUPO_INVALIDO));
        return;
    }
    if (filtraCandidatos(b, &estado->contexto, &candidatos) != ENCONTRO_OK) {
        printf("%s\n", resultadoEncontroParaTexto(ENCONTRO_SEM_CANDIDATOS));
        return;
    }
    exibirCandidatos(estado, b);
    inicializaEncontro(&manual);
    while (continuar && manual.quantidadeEspecies < MAX_ESPECIES_ENCONTRO) {
        id = ler_inteiro("ID do monstro (0 para concluir): ");
        if (id == 0) {
            continuar = 0;
            break;
        }
        elegivel = 0;
        for (i = 0; i < candidatos.quantidade; i++) {
            if (candidatos.itens[i]->id == id) {
                elegivel = 1;
            }
        }
        for (i = 0; i < manual.quantidadeEspecies; i++) {
            if (manual.componentes[i].idMonstro == id) {
                elegivel = 0;
            }
        }
        if (!elegivel) {
            printf("ID nao elegivel ou ja incluido.\n");
            continue;
        }
        quantidade = ler_inteiro_faixa("Quantidade: ", 1, MAX_MONSTROS_ENCONTRO);
        if (manual.quantidadeMonstros + quantidade > MAX_MONSTROS_ENCONTRO) {
            printf("Limite de %d monstros excedido.\n", MAX_MONSTROS_ENCONTRO);
            continue;
        }
        m = consultaMonstroPorId(b, id);
        componente = &manual.componentes[manual.quantidadeEspecies++];
        componente->idMonstro = id;
        componente->quantidade = quantidade;
        componente->poderCalculado = poderComponente(m, quantidade, &estado->contexto);
        manual.quantidadeMonstros += quantidade;
        manual.poderTotal += componente->poderCalculado;
    }
    if (manual.quantidadeEspecies == 0) {
        printf("Nenhum monstro informado. Encontro manual cancelado.\n");
        return;
    }
    calculaFaixaDificuldade(estado->grupo.poderTotal, estado->contexto.dificuldade,
                            &estado->faixaMinima, &estado->faixaMaxima);
    inicializaPropostas(&estado->propostas);
    estado->propostas.propostas[0] = manual;
    estado->propostas.quantidade = 1;
    estado->propostas.indiceEscolhido = 0;
    printf("Encontro manual definido como proposta escolhida.\n");
    exibeEncontro(&manual, b);
    if (manual.poderTotal < estado->faixaMinima || manual.poderTotal > estado->faixaMaxima) {
        printf("Aviso: poder %d fora da faixa da dificuldade (%d a %d). O Mestre pode seguir mesmo assim.\n",
               manual.poderTotal, estado->faixaMinima, estado->faixaMaxima);
    }
}

static void exibirEscolhido(const EstadoEncontro *estado, const Bestiario *b) {
    if (estado->propostas.indiceEscolhido < 0) {
        printf("Nenhuma proposta foi escolhida.\n");
        return;
    }
    printf("Encontro escolhido (proposta %d) | %s, %s, %s:\n", estado->propostas.indiceEscolhido + 1,
           terrenoParaTexto(estado->contexto.terreno), periodoParaTexto(estado->contexto.periodo),
           dificuldadeParaTexto(estado->contexto.dificuldade));
    exibeEncontro(&estado->propostas.propostas[estado->propostas.indiceEscolhido], b);
}

static void exibirMenu(const EstadoEncontro *estado) {
    tela_abrir_pagina("Encontros");
    printf("Grupo: %d personagem(ns) | ", estado->grupo.quantidade);
    if (estado->contextoDefinido) {
        printf("Contexto: %s, %s, %s | ", terrenoParaTexto(estado->contexto.terreno),
               periodoParaTexto(estado->contexto.periodo), dificuldadeParaTexto(estado->contexto.dificuldade));
    } else {
        printf("Contexto: nao informado | ");
    }
    printf("Propostas: %d\n", estado->propostas.quantidade);
    tela_linha();
    printf("1 - Selecionar personagens do grupo\n");
    printf("2 - Exibir grupo e poder total\n");
    printf("3 - Informar terreno, periodo e dificuldade\n");
    printf("4 - Exibir monstros elegiveis\n");
    printf("5 - Gerar tres propostas\n");
    printf("6 - Escolher proposta do Mestre\n");
    printf("7 - Exibir encontro escolhido\n");
    printf("8 - Montar encontro manualmente\n");
    printf("0 - Voltar\n");
}

void menuEncontro(EstadoEncontro *estado, const CadastroPersonagens *cadastro, const Bestiario *b) {
    int opcao;

    do {
        exibirMenu(estado);
        opcao = ler_inteiro("Opcao: ");

        if (opcao == 1) {
            selecionarGrupo(estado, cadastro);
        } else if (opcao == 2) {
            exibirGrupo(estado, cadastro);
        } else if (opcao == 3) {
            informarContexto(estado);
        } else if (opcao == 4) {
            exibirCandidatos(estado, b);
        } else if (opcao == 5) {
            gerarPropostas(estado, cadastro, b);
        } else if (opcao == 6) {
            escolherProposta(estado);
        } else if (opcao == 7) {
            exibirEscolhido(estado, b);
        } else if (opcao == 8) {
            montarManual(estado, cadastro, b);
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }
        if (opcao != 0 && opcao != 1) {
            tela_pausar();
        }
    } while (opcao != 0);
}
