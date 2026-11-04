#include "menu_bestiario.h"

#include "entrada.h"

#include <stdio.h>
#include <string.h>

#define LINHA_EDICAO 1200
#define MAX_FILTRO MAX_MONSTROS

void carregarBestiarioInicial(Bestiario *b) {
    ResultadoBestiario resultado = carregaBestiario(b, ARQUIVO_BESTIARIO);

    if (resultado == BESTIARIO_OK) {
        printf("Bestiario carregado: %d monstro(s) de %s.\n", b->quantidade, ARQUIVO_BESTIARIO);
        return;
    }
    printf("Aviso ao carregar %s: %s\n", ARQUIVO_BESTIARIO, b->primeiroErro);
    printf("Registros validos carregados: %d | rejeitados: %d\n", b->quantidade, b->rejeitados);
}

static void listarFiltrados(const Monstro *lista[], int quantidade) {
    int i;

    if (quantidade == 0) {
        printf("Nenhum monstro encontrado.\n");
        return;
    }
    for (i = 0; i < quantidade; i++) {
        printf("%3d | %-24s | %-11s | %s\n", lista[i]->id, lista[i]->nome,
               frequenciaParaTexto(lista[i]->frequencia), atividadeParaTexto(lista[i]->atividade));
    }
    printf("Total: %d monstro(s).\n", quantidade);
}

static void editarCampos(Monstro *m, unsigned char vistos[NUM_CAMPOS_MONSTRO]) {
    char linha[LINHA_EDICAO];
    char erro[200];
    char *igual;
    int indice;
    int i;

    printf("Digite chave=valor (uma por linha). 'chaves' lista as chaves; 'fim' conclui.\n");
    for (;;) {
        ler_texto("> ", linha, sizeof(linha));
        if (linha[0] == '\0' && feof(stdin)) {
            return;
        }
        if (strcmp(linha, "fim") == 0) {
            return;
        }
        if (strcmp(linha, "chaves") == 0) {
            for (i = 0; i < NUM_CAMPOS_MONSTRO; i++) {
                printf("%s%s ", monstroNomeCampo(i), vistos[i] ? "*" : "");
            }
            printf("\n(* = ja definida)\n");
            continue;
        }
        if (linha[0] == '\0') {
            continue;
        }
        igual = strchr(linha, '=');
        if (igual == 0) {
            printf("Formato esperado: chave=valor.\n");
            continue;
        }
        *igual = '\0';
        indice = monstroDefineCampo(m, linha, igual + 1, erro, sizeof(erro));
        if (indice < 0) {
            printf("Recusado: %s\n", erro);
        } else {
            vistos[indice] = 1;
        }
    }
}

static int camposCompletos(const unsigned char vistos[NUM_CAMPOS_MONSTRO]) {
    int i;
    int completos = 1;

    for (i = 0; i < NUM_CAMPOS_MONSTRO; i++) {
        if (!vistos[i]) {
            printf("Chave obrigatoria ausente: %s\n", monstroNomeCampo(i));
            completos = 0;
        }
    }
    return completos;
}

static void cadastrarMonstro(Bestiario *b) {
    Monstro novo;
    const Monstro *modelo;
    unsigned char vistos[NUM_CAMPOS_MONSTRO];
    char motivo[200];
    int idModelo;

    memset(&novo, 0, sizeof(novo));
    memset(vistos, 0, sizeof(vistos));
    idModelo = ler_inteiro("ID de um monstro modelo para copiar (0 para ficha vazia): ");
    if (idModelo != 0) {
        modelo = consultaMonstroPorId(b, idModelo);
        if (modelo == 0) {
            printf("%s\n", resultadoBestiarioParaTexto(BESTIARIO_NAO_ENCONTRADO));
            return;
        }
        novo = *modelo;
        memset(vistos, 1, sizeof(vistos));
        printf("Informe ao menos as chaves id e nome novos.\n");
    }

    editarCampos(&novo, vistos);
    if (!camposCompletos(vistos)) {
        printf("Cadastro cancelado.\n");
        return;
    }
    if (!validaMonstroMotivo(&novo, motivo, sizeof(motivo))) {
        printf("Cadastro recusado: %s\n", motivo);
        return;
    }
    printf("%s\n", resultadoBestiarioParaTexto(insereMonstro(b, novo)));
}

static void alterarMonstro(Bestiario *b) {
    const Monstro *atual;
    Monstro copia;
    unsigned char vistos[NUM_CAMPOS_MONSTRO];
    char motivo[200];
    int id = ler_inteiro("ID do monstro a alterar: ");

    atual = consultaMonstroPorId(b, id);
    if (atual == 0) {
        printf("%s\n", resultadoBestiarioParaTexto(BESTIARIO_NAO_ENCONTRADO));
        return;
    }
    copia = *atual;
    memset(vistos, 1, sizeof(vistos));
    editarCampos(&copia, vistos);
    if (!validaMonstroMotivo(&copia, motivo, sizeof(motivo))) {
        printf("Alteracao recusada: %s\n", motivo);
        return;
    }
    printf("%s\n", resultadoBestiarioParaTexto(alteraMonstro(b, id, copia)));
}

static void consultarPorNome(Bestiario *b) {
    char nome[MONSTRO_NOME_TAMANHO];
    const Monstro *m;

    ler_texto("Nome do monstro: ", nome, sizeof(nome));
    m = buscaMonstroPorNome(b, nome);
    if (m == 0) {
        printf("%s\n", resultadoBestiarioParaTexto(BESTIARIO_NAO_ENCONTRADO));
    } else {
        exibeMonstro(m);
    }
}

static void filtrarPorTerreno(const Bestiario *b) {
    const Monstro *lista[MAX_FILTRO];
    int opcao;

    printf("Terrenos: 1-Caverna 2-Ruinas 3-Floresta 4-Montanha 5-Planicie 6-Pantano 7-Urbano 8-Deserto\n");
    opcao = ler_inteiro("Terreno: ");
    if (!terrenoEhValido((Terreno)opcao)) {
        printf("Terreno invalido.\n");
        return;
    }
    listarFiltrados(lista, listaMonstrosPorTerreno(b, (Terreno)opcao, lista, MAX_FILTRO));
}

static void filtrarPorPeriodo(const Bestiario *b) {
    const Monstro *lista[MAX_FILTRO];
    int opcao;

    printf("Periodos: 1-Diurno 2-Noturno 3-Qualquer horario\n");
    opcao = ler_inteiro("Periodo: ");
    if (!atividadeEhValido((Atividade)opcao)) {
        printf("Periodo invalido.\n");
        return;
    }
    listarFiltrados(lista, listaMonstrosPorPeriodo(b, (Atividade)opcao, lista, MAX_FILTRO));
}

static void salvarBestiario(const Bestiario *b) {
    char nome[200];
    ResultadoBestiario resultado;

    ler_texto("Arquivo de saida (vazio para " ARQUIVO_BESTIARIO_SAIDA "): ", nome, sizeof(nome));
    if (nome[0] == '\0') {
        strcpy(nome, ARQUIVO_BESTIARIO_SAIDA);
    }
    resultado = salvaBestiario(b, nome);
    printf("%s\n", resultadoBestiarioParaTexto(resultado));
    if (resultado == BESTIARIO_OK) {
        printf("%d monstro(s) gravados em %s.\n", b->quantidade, nome);
    }
}

static void exibirMenu(void) {
    printf("\n=== Bestiario ===\n");
    printf("1 - Consultar monstro por ID\n");
    printf("2 - Consultar monstro por nome\n");
    printf("3 - Cadastrar monstro\n");
    printf("4 - Alterar monstro\n");
    printf("5 - Remover monstro\n");
    printf("6 - Listar monstros\n");
    printf("7 - Filtrar por terreno\n");
    printf("8 - Filtrar por periodo de atividade\n");
    printf("9 - Salvar bestiario\n");
    printf("0 - Voltar\n");
}

void menuBestiario(Bestiario *b) {
    const Monstro *m;
    int opcao;

    do {
        exibirMenu();
        opcao = ler_inteiro("Opcao: ");

        if (opcao == 1) {
            m = consultaMonstroPorId(b, ler_inteiro("ID do monstro: "));
            if (m == 0) {
                printf("%s\n", resultadoBestiarioParaTexto(BESTIARIO_NAO_ENCONTRADO));
            } else {
                exibeMonstro(m);
            }
        } else if (opcao == 2) {
            consultarPorNome(b);
        } else if (opcao == 3) {
            cadastrarMonstro(b);
        } else if (opcao == 4) {
            alterarMonstro(b);
        } else if (opcao == 5) {
            printf("%s\n", resultadoBestiarioParaTexto(removeMonstro(b, ler_inteiro("ID do monstro a remover: "))));
        } else if (opcao == 6) {
            listaMonstros(b);
        } else if (opcao == 7) {
            filtrarPorTerreno(b);
        } else if (opcao == 8) {
            filtrarPorPeriodo(b);
        } else if (opcao == 9) {
            salvarBestiario(b);
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}
