#include "entrada.h"
#include "catalogo.h"
#include "cidade.h"
#include "combate.h"
#include "historia.h"
#include "menu_bestiario.h"
#include "menu_combate.h"
#include "menu_encontro.h"
#include "persistencia.h"
#include "personagem.h"
#include "tela.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void exibir_opcoes_raca(void) {
    printf("Racas: 1-Humano 2-Elfo 3-Anao 4-Halfling 5-Orc\n");
}

static void exibir_opcoes_classe(void) {
    printf("Classes: 1-Guerreiro 2-Ladino 3-Mago 4-Clerigo 5-Bardo\n");
}

static void exibir_opcoes_tipo_item(void) {
    printf("Tipos de item:\n");
    printf("1-Elmo 2-Peitoral 3-Manoplas 4-Calca 5-Botas 6-Anel 7-Colar 8-Cinto 9-Arma uma mao 10-Arma duas maos\n");
    printf("11-Runa (equipavel, da bonus) 12-Consumivel (pocao)\n");
}

static void exibir_opcoes_slot(void) {
    printf("Slots: 1-Elmo 2-Peitoral 3-Manoplas 4-Calca 5-Botas 6-Anel 7-Colar 8-Cinto 9-Mao direita 10-Mao esquerda 11-Runa\n");
}

static Personagem ler_personagem(void) {
    int id;
    char nome[PERSONAGEM_NOME_TAMANHO];
    Raca raca;
    Classe classe;
    int nivel;
    int pv_maximos;
    int pv_atuais;
    int ataque;
    int defesa;
    int iniciativa;
    int poder;
    char mensagem[80];

    id = ler_inteiro_faixa("ID do personagem (inteiro positivo): ", 1, INT_MAX);
    ler_texto_obrigatorio("Nome: ", nome, PERSONAGEM_NOME_TAMANHO);
    exibir_opcoes_raca();
    raca = (Raca)ler_inteiro_faixa("Raca: ", RACA_HUMANO, RACA_ORC);
    exibir_opcoes_classe();
    classe = (Classe)ler_inteiro_faixa("Classe: ", CLASSE_GUERREIRO, CLASSE_BARDO);
    nivel = ler_inteiro_faixa("Nivel (1 a 20): ", PERSONAGEM_NIVEL_MINIMO, PERSONAGEM_NIVEL_MAXIMO);
    pv_maximos = ler_inteiro_faixa("PV maximos (1 a 999): ", PERSONAGEM_PV_MINIMO, PERSONAGEM_PV_MAXIMO);
    snprintf(mensagem, sizeof(mensagem), "PV atuais (0 a %d): ", pv_maximos);
    pv_atuais = ler_inteiro_faixa(mensagem, 0, pv_maximos);
    ataque = ler_inteiro_faixa("Ataque (0 a 30): ", PERSONAGEM_ATAQUE_MINIMO, PERSONAGEM_ATAQUE_MAXIMO);
    defesa = ler_inteiro_faixa("Defesa (1 a 30): ", PERSONAGEM_DEFESA_MINIMA, PERSONAGEM_DEFESA_MAXIMA);
    iniciativa = ler_inteiro_faixa("Iniciativa (-5 a 20): ", PERSONAGEM_INICIATIVA_MINIMA, PERSONAGEM_INICIATIVA_MAXIMA);
    poder = ler_inteiro_faixa("Poder (1 a 100): ", PERSONAGEM_PODER_MINIMO, PERSONAGEM_PODER_MAXIMO);

    return personagem_criar_ficha_basica(id, nome, raca, classe, nivel, pv_maximos,
                                         pv_atuais, ataque, defesa, iniciativa, poder);
}

static Item ler_item(void) {
    Item item;

    memset(&item, 0, sizeof(item));
    item.id = ler_inteiro_faixa("ID do item (inteiro positivo): ", 1, INT_MAX);
    ler_texto_obrigatorio("Nome do item: ", item.nome, ITEM_NOME_TAMANHO);
    exibir_opcoes_tipo_item();
    item.tipo = (TipoItem)ler_inteiro_faixa("Tipo: ", TIPO_ITEM_ELMO, TIPO_ITEM_CONSUMIVEL);
    item.espacos = ler_inteiro_faixa("Espacos consumidos (1 a 50): ", ITEM_ESPACOS_MINIMO, ITEM_ESPACOS_MAXIMO);
    item.bonus_ataque = ler_inteiro("Bonus de ataque: ");
    item.bonus_defesa = ler_inteiro("Bonus de defesa: ");
    item.bonus_vida = ler_inteiro("Bonus de vida: ");
    item.bonus_iniciativa = ler_inteiro("Bonus de iniciativa: ");
    item.poder = ler_inteiro_faixa("Poder do item (0 ou maior): ", 0, INT_MAX);
    item.preco = ler_inteiro_faixa("Preco em moedas (0 se nao tiver): ", 0, ITEM_PRECO_MAXIMO);
    if (item.tipo == TIPO_ITEM_CONSUMIVEL) {
        printf("Efeitos: 1-Cura 2-Bonus temporario 3-Veneno 4-Suspeita (25%% cura, 75%% veneno)\n");
        item.efeito = (EfeitoItem)ler_inteiro_faixa("Efeito: ", EFEITO_CURA, EFEITO_SUSPEITA);
        if (item.efeito == EFEITO_BONUS) {
            printf("O bonus temporario usa os bonus de ataque, defesa e iniciativa informados acima.\n");
        } else {
            ler_dado("Dado do efeito (exemplo 2d8+2): ", &item.dano);
        }
    }
    if (tipo_item_eh_arma(item.tipo)) {
        printf("O dano e o dado rolado a cada ataque no formato NdL ou NdL+B:\n");
        printf("N = quantidade de dados, L = lados de cada dado, B = bonus fixo opcional.\n");
        printf("Exemplo: 1d6 rola um dado de 6 lados; 1d8+1 rola um d8 e soma 1.\n");
        ler_dado("Dano da arma (exemplo 1d6, 1d8+1): ", &item.dano);
    }
    if (item.tipo == TIPO_ITEM_ARMA_UMA_MAO) {
        printf("Mao preferida: 0-Qualquer 1-Direita 2-Esquerda (na mao errada: -%d%% de ataque e defesa)\n",
               ITEM_PENALIDADE_MAO_ERRADA);
        item.mao = (MaoPreferida)ler_inteiro_faixa("Mao preferida: ", MAO_QUALQUER, MAO_PREFERIDA_ESQUERDA);
    }

    return item;
}

static Personagem *selecionar_personagem(CadastroPersonagens *cadastro) {
    int id = ler_inteiro("ID do personagem: ");
    Personagem *personagem = cadastro_buscar_por_id(cadastro, id);

    if (personagem == 0) {
        printf("%s\n", resultado_operacao_para_texto(RESULTADO_NAO_ENCONTRADO));
    }

    return personagem;
}

static void administrar_inventario(CadastroPersonagens *cadastro, int emCombate) {
    Personagem *personagem = selecionar_personagem(cadastro);
    char aviso[TELA_AVISO_TAMANHO] = "";
    char titulo[PERSONAGEM_NOME_TAMANHO + 20];
    int opcao;

    if (personagem == 0) {
        tela_pausar();
        return;
    }

    do {
        snprintf(titulo, sizeof(titulo), "Inventario de %s", personagem->nome);
        tela_abrir_pagina(titulo);
        tela_exibir_aviso(aviso);
        printf("1 - Adicionar item\n");
        printf("2 - Buscar item por ID\n");
        printf("3 - Remover item\n");
        printf("4 - Listar inventario\n");
        printf("5 - Informar ocupacao\n");
        printf("6 - Usar item (consumivel)\n");
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");

        if (opcao == 1) {
            Item item = ler_item();
            ResultadoOperacao resultado = personagem_adicionar_item(personagem, item);
            snprintf(aviso, sizeof(aviso), "%s", resultado_operacao_para_texto(resultado));
        } else if (opcao == 2) {
            int id_item = ler_inteiro("ID do item: ");
            const Item *item = inventario_buscar_item_por_id(&personagem->inventario, id_item);
            if (item == 0) {
                printf("%s\n", resultado_operacao_para_texto(RESULTADO_ITEM_NAO_ENCONTRADO));
            } else {
                printf("ID %d | %s | %s | espacos %d\n",
                       item->id, item->nome, tipo_item_para_texto(item->tipo), item->espacos);
            }
            tela_pausar();
        } else if (opcao == 3) {
            int id_item = ler_inteiro("ID do item: ");
            ResultadoOperacao resultado = personagem_remover_item(personagem, id_item, 0);
            snprintf(aviso, sizeof(aviso), "%s", resultado_operacao_para_texto(resultado));
        } else if (opcao == 4) {
            personagem_listar_inventario(personagem);
            tela_pausar();
        } else if (opcao == 5) {
            snprintf(aviso, sizeof(aviso), "Ocupacao atual: %d/%d",
                     inventario_obter_ocupacao(&personagem->inventario),
                     INVENTARIO_MAX_ESPACOS);
        } else if (opcao == 6 && emCombate) {
            snprintf(aviso, sizeof(aviso), "Ha um combate em andamento: use o item pelo menu de combate (opcao 10).");
        } else if (opcao == 6) {
            usarItemForaDeCombate(personagem, aviso);
        } else if (opcao != 0) {
            snprintf(aviso, sizeof(aviso), "Opcao invalida.");
        }
    } while (opcao != 0);
}

static void conceder_experiencia(CadastroPersonagens *cadastro, char aviso[], Som *som) {
    Personagem *personagem = selecionar_personagem(cadastro);
    int experiencia;
    int niveis;

    if (personagem == 0) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "%s", resultado_operacao_para_texto(RESULTADO_NAO_ENCONTRADO));
        return;
    }
    printf("%s esta no nivel %d com %d/%d XP.\n", personagem->nome, personagem->nivel, personagem->experiencia,
           personagem_xp_proximo_nivel(personagem));
    experiencia = ler_inteiro_faixa("XP a conceder (0 para cancelar): ", 0, PERSONAGEM_XP_MAXIMO);
    if (experiencia < 1) {
        return;
    }
    niveis = personagem_ganhar_experiencia(personagem, experiencia);
    if (niveis > 0) {
        som_toca(som, SOM_NIVEL);
    }
    snprintf(aviso, TELA_AVISO_TAMANHO, "%s recebeu %d XP: nivel %d, %d/%d XP (%d nivel(is) ganho(s)).",
             personagem->nome, experiencia, personagem->nivel, personagem->experiencia,
             personagem_xp_proximo_nivel(personagem), niveis);
}

static void equipar_item(CadastroPersonagens *cadastro, char aviso[]) {
    Personagem *personagem = selecionar_personagem(cadastro);
    const Item *item;
    SlotEquipamento slot;
    int id_item;

    if (personagem == 0) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "%s", resultado_operacao_para_texto(RESULTADO_NAO_ENCONTRADO));
        return;
    }
    personagem_listar_inventario(personagem);
    id_item = ler_inteiro("ID do item no inventario: ");
    exibir_opcoes_slot();
    slot = (SlotEquipamento)ler_inteiro_faixa("Slot de destino: ", SLOT_ELMO, SLOT_RUNA);
    item = inventario_buscar_item_por_id(&personagem->inventario, id_item);
    if (personagem_item_em_mao_errada(item, slot)) {
        printf("ATENCAO: %s foi feito para a %s.\n", item->nome, mao_preferida_para_texto(item->mao));
        printf("Na mao errada, %s fica com -%d%% de ataque e de defesa enquanto o item estiver equipado.\n",
               personagem->nome, ITEM_PENALIDADE_MAO_ERRADA);
        if (!ler_confirmacao("Quer mesmo equipar assim? Digite 1 ou sim para confirmar: ")) {
            snprintf(aviso, TELA_AVISO_TAMANHO, "Equipar cancelado. Nada mudou.");
            return;
        }
    }
    snprintf(aviso, TELA_AVISO_TAMANHO, "%s",
             resultado_operacao_para_texto(personagem_equipar_item(personagem, id_item, slot)));
    if (personagem_usa_mao_errada(personagem)) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "Aviso: arma na mao errada, -%d%% de ataque e defesa ate trocar de mao.",
                 ITEM_PENALIDADE_MAO_ERRADA);
    }
}

static void alterar_moedas(CadastroPersonagens *cadastro, char aviso[]) {
    Personagem *personagem = selecionar_personagem(cadastro);
    ResultadoOperacao resultado;
    int variacao;

    if (personagem == 0) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "%s", resultado_operacao_para_texto(RESULTADO_NAO_ENCONTRADO));
        return;
    }
    printf("%s tem %d moeda(s). Moedas nao ocupam espaco na bolsa.\n", personagem->nome, personagem->moedas);
    variacao = ler_inteiro_faixa("Quantidade a adicionar (negativo para retirar): ", -PERSONAGEM_MOEDAS_MAXIMO,
                                 PERSONAGEM_MOEDAS_MAXIMO);
    if (variacao < -PERSONAGEM_MOEDAS_MAXIMO) {
        return;
    }
    resultado = personagem_alterar_moedas(personagem, variacao);
    if (resultado == RESULTADO_OK) {
        snprintf(aviso, TELA_AVISO_TAMANHO, "%s agora tem %d moeda(s).", personagem->nome, personagem->moedas);
    } else {
        snprintf(aviso, TELA_AVISO_TAMANHO, "%s", resultado_operacao_para_texto(resultado));
    }
}

static void exibir_tela_inicial(unsigned int semente) {
    tela_abrir_pagina("");
    printf("  ||                                                        ||\n");
    printf("  ||        SISTEMA AUXILIAR DE MESTRE DE RPG               ||\n");
    printf("  ||                                                        ||\n");
    printf("  ||   Personagens  *  Bestiario  *  Encontros  *  Combate  ||\n");
    printf("  ||      Cidade de Falmuth  *  Itens  *  Modo Historia     ||\n");
    printf("  ||                                                        ||\n");
    tela_linha();
    printf("  Semente aleatoria desta sessao: %u\n", semente);
}

static void carregar_catalogo_inicial(CatalogoItens *catalogo) {
    if (catalogo_carregar(catalogo, ARQUIVO_CATALOGO) == CATALOGO_ERRO_ARQUIVO) {
        printf("Aviso: %s nao foi encontrado. As lojas da cidade ficarao vazias.\n", ARQUIVO_CATALOGO);
        return;
    }
    printf("Catalogo de itens carregado: %d item(ns) de %s.\n", catalogo->quantidade, ARQUIVO_CATALOGO);
    if (catalogo->rejeitados > 0) {
        printf("Aviso: %d item(ns) rejeitado(s); primeiro erro no bloco da linha %d.\n", catalogo->rejeitados,
               catalogo->linhaPrimeiroErro);
    }
}

static void exibir_menu_principal(int modoHistoria, int somLigado, char aviso[]) {
    tela_abrir_pagina("MENU PRINCIPAL - Sistema Auxiliar de Mestre");
    tela_exibir_aviso(aviso);
    printf("--- Personagens ---\n");
    printf(" 1 - Cadastrar personagem\n");
    printf(" 2 - Consultar personagem por ID\n");
    printf(" 3 - Alterar personagem\n");
    printf(" 4 - Remover personagem\n");
    printf(" 5 - Listar personagens\n");
    printf("--- Inventario e equipamentos ---\n");
    printf(" 6 - Administrar inventario (inclui usar item)\n");
    printf(" 7 - Consultar equipamentos\n");
    printf(" 8 - Equipar item\n");
    printf(" 9 - Desequipar item\n");
    printf("10 - Exibir atributos totais\n");
    printf("--- Mestre ---\n");
    printf("11 - Bestiario\n");
    printf("12 - Encontros\n");
    printf("13 - Combate\n");
    printf("--- Arquivo ---\n");
    printf("14 - Salvar personagens, itens, grupo e progresso em arquivo\n");
    printf("15 - Carregar personagens, itens, grupo e progresso de arquivo\n");
    printf("--- Cidade e moedas ---\n");
    printf("16 - Adicionar ou retirar moedas de um personagem\n");
    printf("17 - Cidade de Falmuth (lojas, hospital, academia, livraria...)\n");
    printf("18 - Conceder experiencia (XP) a um personagem\n");
    printf("19 - Som: %s (escolha para alternar)\n", somLigado ? "ligado" : "desligado");
    tela_linha();
    printf(" 0 - %s\n", modoHistoria ? "Voltar ao Modo Historia" : "Encerrar");
}

static void menu_mestre(CadastroPersonagens *cadastro, Bestiario *bestiario, EstadoEncontro *encontro,
                        Combate *combate, const CatalogoItens *catalogo, ProgressoHistoria *progresso,
                        Som *som, int modoHistoria) {
    char aviso[TELA_AVISO_TAMANHO] = "";
    int opcao;

    do {
        exibir_menu_principal(modoHistoria, som != 0 && som->ligado, aviso);
        opcao = ler_inteiro("Opcao: ");

        if (opcao == 1) {
            Personagem personagem = ler_personagem();
            ResultadoOperacao resultado = cadastro_cadastrar(cadastro, personagem);
            snprintf(aviso, sizeof(aviso), "%s", resultado_operacao_para_texto(resultado));
        } else if (opcao == 2) {
            Personagem *personagem = selecionar_personagem(cadastro);
            if (personagem != 0) {
                personagem_exibir(personagem);
            }
            tela_pausar();
        } else if (opcao == 3 && combateEmAndamento(combate)) {
            snprintf(aviso, sizeof(aviso), "Alteracao bloqueada: ha um combate em andamento.");
        } else if (opcao == 4 && combateEmAndamento(combate)) {
            snprintf(aviso, sizeof(aviso), "Remocao bloqueada: ha um combate em andamento.");
        } else if (opcao == 3) {
            int id_atual = ler_inteiro("ID atual do personagem: ");
            Personagem novos_dados = ler_personagem();
            ResultadoOperacao resultado = cadastro_alterar(cadastro, id_atual, novos_dados);
            snprintf(aviso, sizeof(aviso), "%s", resultado_operacao_para_texto(resultado));
        } else if (opcao == 4) {
            int id = ler_inteiro("ID do personagem a remover: ");
            ResultadoOperacao resultado = cadastro_remover(cadastro, id);
            if (resultado == RESULTADO_OK) {
                inicializaCombate(combate);
            }
            snprintf(aviso, sizeof(aviso), "%s", resultado_operacao_para_texto(resultado));
        } else if (opcao == 5) {
            cadastro_listar(cadastro);
            printf("Total: %d personagem(ns).\n", cadastro_obter_quantidade(cadastro));
            tela_pausar();
        } else if (opcao == 6) {
            administrar_inventario(cadastro, combateEmAndamento(combate));
        } else if (opcao == 7) {
            Personagem *personagem = selecionar_personagem(cadastro);
            if (personagem != 0) {
                personagem_exibir_equipamentos(personagem);
            }
            tela_pausar();
        } else if (opcao == 8) {
            equipar_item(cadastro, aviso);
        } else if (opcao == 9) {
            Personagem *personagem = selecionar_personagem(cadastro);
            if (personagem != 0) {
                SlotEquipamento slot;
                personagem_exibir_equipamentos(personagem);
                exibir_opcoes_slot();
                slot = (SlotEquipamento)ler_inteiro_faixa("Slot a desequipar: ", SLOT_ELMO, SLOT_RUNA);
                snprintf(aviso, sizeof(aviso), "%s",
                         resultado_operacao_para_texto(personagem_desequipar_item(personagem, slot)));
            } else {
                snprintf(aviso, sizeof(aviso), "%s", resultado_operacao_para_texto(RESULTADO_NAO_ENCONTRADO));
            }
        } else if (opcao == 10) {
            Personagem *personagem = selecionar_personagem(cadastro);
            if (personagem != 0) {
                personagem_exibir_atributos_totais(personagem);
            }
            tela_pausar();
        } else if (opcao == 11) {
            int monstrosAntes = bestiario->quantidade;
            menuBestiario(bestiario, combateEmAndamento(combate));
            if (bestiario->quantidade < monstrosAntes) {
                inicializaCombate(combate);
            }
        } else if (opcao == 12) {
            menuEncontro(encontro, cadastro, bestiario);
        } else if (opcao == 13) {
            menuCombate(combate, cadastro, encontro, bestiario, som);
        } else if (opcao == 14) {
            jogoSalvar(cadastro, &encontro->grupo, progresso, ARQUIVO_JOGO_MESTRE, aviso);
        } else if (opcao == 15 && combateEmAndamento(combate)) {
            snprintf(aviso, sizeof(aviso), "Carga bloqueada: ha um combate em andamento.");
        } else if (opcao == 15) {
            if (jogoCarregar(cadastro, &encontro->grupo, progresso, ARQUIVO_JOGO_MESTRE, aviso)) {
                inicializaCombate(combate);
            }
        } else if (opcao == 16) {
            alterar_moedas(cadastro, aviso);
        } else if (opcao == 17 && combateEmAndamento(combate)) {
            snprintf(aviso, sizeof(aviso), "Cidade bloqueada: ha um combate em andamento.");
        } else if (opcao == 17) {
            Personagem *personagem = selecionar_personagem(cadastro);
            if (personagem != 0) {
                menuCidade(personagem, catalogo, HISTORIA_NUM_CAPITULOS, som);
            } else {
                snprintf(aviso, sizeof(aviso), "%s", resultado_operacao_para_texto(RESULTADO_NAO_ENCONTRADO));
            }
        } else if (opcao == 18) {
            conceder_experiencia(cadastro, aviso, som);
        } else if (opcao == 19) {
            snprintf(aviso, sizeof(aviso), "Som %s.", som_alterna(som) ? "ligado" : "desligado");
        } else if (opcao != 0) {
            snprintf(aviso, sizeof(aviso), "Opcao invalida.");
        }
    } while (opcao != 0);
}

int main(int argc, char *argv[]) {
    CadastroPersonagens cadastro;
    Bestiario bestiario;
    EstadoEncontro encontro;
    Combate combate;
    CatalogoItens catalogo;
    ProgressoHistoria progresso;
    Som som;
    ConfigConsole console;
    ModoJogo modo;
    unsigned int semente;

    console = configura_console_utf8();
    som_inicializa(&som);

    if (argc > 1) {
        semente = (unsigned int)strtoul(argv[1], 0, 10);
        inicializaAleatorio(semente);
    } else {
        semente = inicializaAleatorioPorTempo();
    }
    exibir_tela_inicial(semente);

    cadastro_inicializar(&cadastro);
    inicializaBestiario(&bestiario);
    inicializaEstadoEncontro(&encontro);
    inicializaCombate(&combate);
    catalogo_inicializar(&catalogo);
    historia_inicializa_progresso(&progresso);
    carregarBestiarioInicial(&bestiario);
    carregar_catalogo_inicial(&catalogo);
    tela_pausar();

    modo = historiaAbertura(&som);
    if (modo == MODO_HISTORIA) {
        while (menuHistoria(&cadastro, &encontro, &combate, &bestiario, &catalogo, &progresso, &som)
               == HISTORIA_ABRIR_PAINEL) {
            menu_mestre(&cadastro, &bestiario, &encontro, &combate, &catalogo, &progresso, &som, 1);
        }
    } else {
        menu_mestre(&cadastro, &bestiario, &encontro, &combate, &catalogo, &progresso, &som, 0);
    }

    tela_abrir_pagina("");
    printf("Programa encerrado.\n");
    som_encerra(&som);
    restaura_console(&console);
    return 0;
}
