#ifndef HISTORIA_H
#define HISTORIA_H

#include "catalogo.h"
#include "combate.h"
#include "menu_encontro.h"
#include "persistencia.h"
#include "som.h"

#define ARQUIVO_HISTORIA "DadosHistoria.txt"
#define ARQUIVO_JOGO_MESTRE "DadosPersonagens.txt"
#define ARQUIVO_JOGO_HISTORIA "DadosHistoriaSalvo.txt"
#define HISTORIA_NUM_PRINCIPAIS 4
#define HISTORIA_NUM_SECUNDARIAS 3
#define HISTORIA_CODIGO_PAINEL 99
#define HISTORIA_ERRO_ARQUIVO -1
#define HISTORIA_MOEDAS_INICIAIS 50
#define HISTORIA_NUM_CAPITULOS 4

typedef enum {
    MODO_MESTRE = 1,
    MODO_HISTORIA
} ModoJogo;

typedef enum {
    HISTORIA_SAIR,
    HISTORIA_ABRIR_PAINEL
} SaidaHistoria;

typedef struct {
    int principais;
    int secundarias;
} ProgressoHistoria;

typedef struct {
    const char *titulo;
    Terreno terreno;
    Dificuldade dificuldade;
    int moedas;
    int experiencia;
} Missao;

void historia_inicializa_progresso(ProgressoHistoria *progresso);
void historia_define_progresso(ProgressoHistoria *progresso, int principais, int secundarias);
const Missao *historia_missao_principal(int numero);
const Missao *historia_missao_secundaria(int numero);
int historia_capitulos_liberados(const ProgressoHistoria *progresso);
int historia_conta_secundarias(const ProgressoHistoria *progresso);
int historia_secundaria_concluida(const ProgressoHistoria *progresso, int numero);
int historia_conclui_principal(ProgressoHistoria *progresso, int numero);
int historia_conclui_secundaria(ProgressoHistoria *progresso, int numero);
int historia_recompensa_grupo(CadastroPersonagens *cadastro, const GrupoAventura *grupo, const Missao *missao,
                              int *niveis);
void historiaLerCapitulos(int liberados);
int jogoSalvar(const CadastroPersonagens *cadastro, const GrupoAventura *grupo, const ProgressoHistoria *progresso,
               const char *padrao, char aviso[]);
int jogoCarregar(CadastroPersonagens *cadastro, GrupoAventura *grupo, ProgressoHistoria *progresso,
                 const char *padrao, char aviso[]);
int historia_exibe_secao(const char *nomeArquivo, const char *secao, int *pulado);
Personagem historia_cria_heroi(int id, const char *nome, Raca raca, Classe classe);
int historia_proximo_id(const CadastroPersonagens *cadastro);
int historia_descansa_grupo(CadastroPersonagens *cadastro, const GrupoAventura *grupo);
ModoJogo historiaAbertura(Som *som);
SaidaHistoria menuHistoria(CadastroPersonagens *cadastro, EstadoEncontro *encontro, Combate *combate,
                           const Bestiario *bestiario, const CatalogoItens *catalogo, ProgressoHistoria *progresso,
                           Som *som);

#endif
