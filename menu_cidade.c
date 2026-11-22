#include "cidade.h"

#include "entrada.h"
#include "historia.h"
#include "tela.h"

#include <stdio.h>

static void cabecalho(const Personagem *personagem, const char *titulo, char aviso[]) {
    tela_abrir_pagina(titulo);
    printf("%s | PV %d/%d | Moedas %d | Bolsa %d/%d\n", personagem->nome, personagem->pontos_vida_atuais,
           personagem->pontos_vida_maximos, personagem->moedas,
           inventario_obter_ocupacao(&personagem->inventario), INVENTARIO_MAX_ESPACOS);
    tela_linha();
    tela_exibir_aviso(aviso);
}

static void vender(Personagem *personagem, const CatalogoItens *catalogo, LocalVenda local, char aviso[], Som *som) {
    const ItemCatalogo *lista[CATALOGO_MAX_ITENS];
    ResultadoOperacao resultado;
    int quantidade;
    int id;
    int i;

    do {
        cabecalho(personagem, local_venda_para_texto(local), aviso);
        quantidade = catalogo_listar_local(catalogo, local, lista, CATALOGO_MAX_ITENS);
        if (quantidade == 0) {
            printf("Nada a venda aqui. Confira o arquivo %s.\n", ARQUIVO_CATALOGO);
        }
        for (i = 0; i < quantidade; i++) {
            catalogo_exibir_item(lista[i]);
        }
        id = ler_inteiro("ID do item para comprar (0 para voltar): ");
        if (id != 0) {
            resultado = catalogo_comprar(catalogo, id, local, personagem);
            if (resultado == RESULTADO_OK) {
                snprintf(aviso, TELA_AVISO_TAMANHO, "Compra realizada: %s.", catalogo_buscar_por_id(catalogo, id)->item.nome);
                som_toca(som, SOM_MOEDAS);
            } else {
                snprintf(aviso, TELA_AVISO_TAMANHO, "Compra recusada: %s", resultado_operacao_para_texto(resultado));
                som_toca(som, SOM_ERRO);
            }
        }
    } while (id != 0);
}

static void comprar_item_do_personagem(Personagem *personagem, const char *titulo, char aviso[], Som *som) {
    const Item *item;
    ResultadoCidade resultado;
    char nome[ITEM_NOME_TAMANHO];
    int valor = 0;
    int id;

    do {
        cabecalho(personagem, titulo, aviso);
        printf("O comerciante paga %d%% do preco de cada item.\n", CIDADE_PERCENTUAL_VENDA);
        personagem_listar_inventario(personagem);
        id = ler_inteiro("ID do item para vender (0 para voltar): ");
        if (id == 0) {
            break;
        }
        item = inventario_buscar_item_por_id(&personagem->inventario, id);
        if (item == 0) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "%s", resultado_cidade_para_texto(CIDADE_ITEM_NAO_ENCONTRADO));
            continue;
        }
        snprintf(nome, sizeof(nome), "%s", item->nome);
        if (cidade_item_importante(item) && cidade_valor_venda(item) > 0) {
            printf("%s (%s) sera vendido por %d moeda(s).\n", item->nome, tipo_item_para_texto(item->tipo),
                   cidade_valor_venda(item));
            if (!ler_confirmacao("Confirma a venda? (1 ou sim para confirmar): ")) {
                snprintf(aviso, TELA_AVISO_TAMANHO, "Venda cancelada.");
                continue;
            }
        }
        resultado = cidade_vender_item(personagem, id, &valor);
        if (resultado == CIDADE_SUCESSO) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "%s vendido por %d moeda(s).", nome, valor);
            som_toca(som, SOM_MOEDAS);
        } else {
            snprintf(aviso, TELA_AVISO_TAMANHO, "Venda recusada: %s", resultado_cidade_para_texto(resultado));
            som_toca(som, SOM_ERRO);
        }
    } while (id != 0);
}

static void comercio(Personagem *personagem, const CatalogoItens *catalogo, LocalVenda local, char aviso[],
                     Som *som) {
    int opcao;

    do {
        cabecalho(personagem, local_venda_para_texto(local), aviso);
        printf("1 - Comprar\n");
        printf("2 - Vender itens da bolsa (%d%% do preco)\n", CIDADE_PERCENTUAL_VENDA);
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1) {
            vender(personagem, catalogo, local, aviso, som);
        } else if (opcao == 2) {
            comprar_item_do_personagem(personagem, "Vender itens", aviso, som);
        } else if (opcao != 0) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "Opcao invalida.");
        }
    } while (opcao != 0);
}

static void livraria(Personagem *personagem, int capitulosLiberados, char aviso[]) {
    ResultadoCidade resultado;
    int opcao;

    do {
        cabecalho(personagem, "Livraria", aviso);
        printf("1 - Ler a historia de Falmuth\n");
        printf("2 - Estudar %s\n", estudo_para_texto(ESTUDO_MAGIA));
        printf("3 - Estudar %s\n", estudo_para_texto(ESTUDO_MEDICINA));
        printf("Cada estudo custa %d moedas e tem %d%% de chance de dar resultado.\n", CIDADE_PRECO_ESTUDO,
               CIDADE_CHANCE_ESTUDO);
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1) {
            historiaLerCapitulos(capitulosLiberados);
        } else if (opcao == 2 || opcao == 3) {
            resultado = cidade_estudar(personagem, (Estudo)(opcao - 1));
            if (resultado == CIDADE_SUCESSO) {
                snprintf(aviso, TELA_AVISO_TAMANHO, "Estudo concluido: %s.", estudo_para_texto((Estudo)(opcao - 1)));
            } else if (resultado == CIDADE_FRACASSO) {
                snprintf(aviso, TELA_AVISO_TAMANHO, "As paginas nao fizeram sentido desta vez. As moedas foram gastas.");
            } else {
                snprintf(aviso, TELA_AVISO_TAMANHO, "%s", resultado_cidade_para_texto(resultado));
            }
        } else if (opcao != 0) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "Opcao invalida.");
        }
    } while (opcao != 0);
}

static void hospital(Personagem *personagem, const CatalogoItens *catalogo, char aviso[], Som *som) {
    ResultadoCidade resultado;
    int opcao;

    do {
        cabecalho(personagem, "Hospital", aviso);
        printf("1 - Tratamento completo (%d moedas)\n", CIDADE_PRECO_TRATAMENTO);
        printf("2 - Comprar pocoes de cura\n");
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1) {
            resultado = cidade_tratar(personagem);
            if (resultado == CIDADE_SUCESSO) {
                snprintf(aviso, TELA_AVISO_TAMANHO, "%s foi tratado e esta com todos os PV.", personagem->nome);
            } else {
                snprintf(aviso, TELA_AVISO_TAMANHO, "%s", resultado_cidade_para_texto(resultado));
            }
        } else if (opcao == 2) {
            vender(personagem, catalogo, LOCAL_HOSPITAL, aviso, som);
        } else if (opcao != 0) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "Opcao invalida.");
        }
    } while (opcao != 0);
}

static void academia(Personagem *personagem, char aviso[]) {
    ResultadoCidade resultado;
    int opcao;

    cabecalho(personagem, "Academia", aviso);
    printf("Cada treino custa %d moedas e tem %d%% de chance de dar +1 no atributo.\n", CIDADE_PRECO_ACADEMIA,
           CIDADE_CHANCE_ACADEMIA);
    printf("1 - Ataque (%d)\n2 - Defesa (%d)\n3 - Iniciativa (%d)\n0 - Voltar\n", personagem->ataque,
           personagem->defesa, personagem->iniciativa);
    opcao = ler_inteiro_faixa("Treino: ", 0, TREINO_INICIATIVA);
    if (opcao < TREINO_ATAQUE) {
        return;
    }
    resultado = cidade_treinar(personagem, (Treino)opcao);
    if (resultado == CIDADE_SUCESSO) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Treino concluido: +1 em %s.", treino_para_texto((Treino)opcao));
    } else if (resultado == CIDADE_FRACASSO) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "O treino de %s nao rendeu desta vez. As moedas foram gastas.",
                 treino_para_texto((Treino)opcao));
    } else {
        snprintf(aviso, TELA_AVISO_TAMANHO, "%s", resultado_cidade_para_texto(resultado));
    }
}

static void bar(Personagem *personagem, char aviso[]) {
    ResultadoCidade resultado;
    int pontos = 0;

    resultado = cidade_bar(personagem, &pontos);
    if (resultado == CIDADE_SUCESSO) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Bar: uma boa refeicao e descanso. %s recuperou %d PV.", personagem->nome,
                 pontos);
    } else if (resultado == CIDADE_FRACASSO) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Bar: %s se meteu em uma briga e perdeu %d PV.", personagem->nome, pontos);
    } else {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Bar (entrada %d moedas): %s", CIDADE_PRECO_BAR,
                 resultado_cidade_para_texto(resultado));
    }
}

static void praca(Personagem *personagem, char aviso[]) {
    ResultadoCidade resultado;
    int valor = 0;

    if (ler_inteiro_faixa("Tentar roubar alguem na praca? 75% de ser pego (1-Sim 0-Nao): ", 0, 1) != 1) {
        return;
    }
    resultado = cidade_roubar(personagem, &valor);
    if (resultado == CIDADE_SUCESSO) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Praca: %s fugiu com %d moedas.", personagem->nome, valor);
    } else if (resultado == CIDADE_FRACASSO) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Praca: %s foi pego, apanhou e perdeu %d PV.", personagem->nome, valor);
    } else {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Praca: %s", resultado_cidade_para_texto(resultado));
    }
}

static void ferro_velho(Personagem *personagem, char aviso[]) {
    const Item *item;
    Item removido;
    int id;

    do {
        cabecalho(personagem, "Ferro velho", aviso);
        personagem_listar_inventario(personagem);
        id = ler_inteiro("ID do item para jogar fora (0 para voltar): ");
        if (id == 0) {
            break;
        }
        item = inventario_buscar_item_por_id(&personagem->inventario, id);
        if (item == 0) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "%s", resultado_operacao_para_texto(RESULTADO_ITEM_NAO_ENCONTRADO));
            continue;
        }
        if (cidade_item_importante(item)) {
            printf("ATENCAO: %s (%s) e um item importante e sera perdido para sempre.\n", item->nome,
                   tipo_item_para_texto(item->tipo));
            if (ler_inteiro_faixa("Confirma o descarte? (1-Sim 0-Nao): ", 0, 1) != 1) {
                snprintf(aviso, TELA_AVISO_TAMANHO, "Descarte cancelado.");
                continue;
            }
        }
        if (personagem_remover_item(personagem, id, &removido) == RESULTADO_OK) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "%s foi jogado fora.", removido.nome);
        }
    } while (id != 0);
}

void menuCidade(Personagem *personagem, const CatalogoItens *catalogo, int capitulosLiberados, Som *som) {
    char aviso[TELA_AVISO_TAMANHO] = "";
    int opcao;

    if (personagem == 0 || catalogo == 0) {
        return;
    }
    do {
        cabecalho(personagem, "Cidade de Falmuth", aviso);
        printf("1 - Loja de itens (comprar e vender)\n");
        printf("2 - Hospital\n");
        printf("3 - Academia\n");
        printf("4 - Bar (%d moedas)\n", CIDADE_PRECO_BAR);
        printf("5 - Praca\n");
        printf("6 - Cabana escura\n");
        printf("7 - Ferreiro (comprar e vender)\n");
        printf("8 - Ferro velho\n");
        printf("9 - Livraria (historia de Falmuth e estudos)\n");
        printf("0 - Sair da cidade\n");
        opcao = ler_inteiro("Opcao: ");

        if (opcao == 1) {
            comercio(personagem, catalogo, LOCAL_LOJA, aviso, som);
        } else if (opcao == 2) {
            hospital(personagem, catalogo, aviso, som);
        } else if (opcao == 3) {
            academia(personagem, aviso);
        } else if (opcao == 4) {
            bar(personagem, aviso);
        } else if (opcao == 5) {
            praca(personagem, aviso);
        } else if (opcao == 6) {
            vender(personagem, catalogo, LOCAL_CABANA, aviso, som);
        } else if (opcao == 7) {
            comercio(personagem, catalogo, LOCAL_FERREIRO, aviso, som);
        } else if (opcao == 8) {
            ferro_velho(personagem, aviso);
        } else if (opcao == 9) {
            livraria(personagem, capitulosLiberados, aviso);
        } else if (opcao != 0) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "Opcao invalida.");
        }
    } while (opcao != 0);
}
