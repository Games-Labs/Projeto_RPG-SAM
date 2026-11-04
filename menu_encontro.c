#include "menu_encontro.h"

#include "entrada.h"

#include <stdio.h>

void inicializaEstadoEncontro(EstadoEncontro *estado) {
    inicializaGrupo(&estado->grupo);
    estado->contextoDefinido = 0;
    estado->faixaMinima = 0;
    estado->faixaMaxima = 0;
    inicializaPropostas(&estado->propostas);
}

static void selecionarGrupo(EstadoEncontro *estado, const CadastroPersonagens *cadastro) {
    int opcao;
    int id;

    do {
        printf("\nGrupo atual: %d/%d personagem(ns)\n", estado->grupo.quantidade, MAX_PERSONAGENS_GRUPO);
        printf("1 - Adicionar personagem por ID\n2 - Remover personagem por ID\n3 - Limpar grupo\n0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1) {
            id = ler_inteiro("ID do personagem: ");
            printf("%s\n", resultadoGrupoParaTexto(adicionaPersonagemGrupo(&estado->grupo, cadastro, id)));
        } else if (opcao == 2) {
            id = ler_inteiro("ID do personagem: ");
            printf("%s\n", resultadoGrupoParaTexto(removePersonagemGrupo(&estado->grupo, id)));
        } else if (opcao == 3) {
            inicializaGrupo(&estado->grupo);
            printf("Grupo esvaziado.\n");
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }
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
    for (i = 0; i < candidatos.quantidade; i++) {
        printf("%3d | %-24s | %-11s | peso %2d | poder individual %d\n", candidatos.itens[i]->id,
               candidatos.itens[i]->nome, frequenciaParaTexto(candidatos.itens[i]->frequencia),
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

static void exibirMenu(void) {
    printf("\n=== Encontros ===\n");
    printf("1 - Selecionar personagens do grupo\n");
    printf("2 - Exibir grupo e poder total\n");
    printf("3 - Informar terreno, periodo e dificuldade\n");
    printf("4 - Exibir monstros elegiveis\n");
    printf("5 - Gerar tres propostas\n");
    printf("6 - Escolher proposta do Mestre\n");
    printf("7 - Exibir encontro escolhido\n");
    printf("0 - Voltar\n");
}

void menuEncontro(EstadoEncontro *estado, const CadastroPersonagens *cadastro, const Bestiario *b) {
    int opcao;

    do {
        exibirMenu();
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
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}
