#ifndef COMBATE_H
#define COMBATE_H

#include "bestiario.h"
#include "dados.h"
#include "encontro.h"
#include "grupo.h"
#include "personagem.h"

#define MAX_COMBATENTES (MAX_PERSONAGENS_GRUPO + MAX_MONSTROS_ENCONTRO)
#define MAX_RODADAS 100
#define D20_FACES 20
#define DANO_DESARMADO_QUANTIDADE 1
#define DANO_DESARMADO_FACES 2
#define NOME_COMBATENTE_TAMANHO 70
#define PENALIDADE_SEGUNDA_ARMA 2
#define TESTE_MORAL_VIDA 1
#define TESTE_MORAL_ALIADO 2
#define FUGA_DIFICULDADE 10
#define XP_POR_MOEDA 5

typedef enum {
    COMBATENTE_ATIVO = 1,
    COMBATENTE_DERROTADO,
    COMBATENTE_FUGIU
} EstadoCombatente;

typedef enum {
    LADO_PERSONAGENS = 1,
    LADO_MONSTROS,
    LADO_NENHUM
} LadoCombate;

typedef enum {
    COMBATE_OK,
    COMBATE_ARGUMENTO_NULO,
    COMBATE_GRUPO_INVALIDO,
    COMBATE_PERSONAGEM_INEXISTENTE,
    COMBATE_SEM_PERSONAGEM_ATIVO,
    COMBATE_ENCONTRO_INVALIDO,
    COMBATE_MONSTRO_INEXISTENTE,
    COMBATE_LIMITE_EXCEDIDO,
    COMBATE_DADOS_INVALIDOS,
    COMBATE_NAO_PREPARADO,
    COMBATE_ENCERRADO,
    COMBATE_NAO_ENCERRADO,
    COMBATE_TURNO_IGNORADO,
    COMBATE_SEM_ALVO,
    COMBATE_ERRO_ARQUIVO,
    COMBATE_FUGA_FALHOU,
    COMBATE_ITEM_RECUSADO
} ResultadoCombate;

typedef struct {
    Personagem *ficha;
    int pontosVidaMaximos;
    int pontosVidaAtuais;
    int iniciativa;
    EstadoCombatente estado;
    int temCura;
    Dado dadoCura;
    int temMagia;
    Dado dadoMagia;
} PersonagemCombate;

typedef struct {
    int numeroExemplar;
    const Monstro *especie;
    int pontosVidaMaximos;
    int pontosVidaAtuais;
    int iniciativa;
    EstadoCombatente estado;
    int testesMoral;
} MonstroCombate;

typedef struct {
    LadoCombate lado;
    int indice;
    int iniciativa;
} EntradaOrdem;

typedef struct {
    PersonagemCombate personagens[MAX_PERSONAGENS_GRUPO];
    int quantidadePersonagens;

    MonstroCombate monstros[MAX_MONSTROS_ENCONTRO];
    int quantidadeMonstros;

    EntradaOrdem ordem[MAX_COMBATENTES];
    int quantidadeOrdem;
    int rodadaAtual;
    int encerrado;
    LadoCombate vencedor;
    int exibirHistorico;
    int fichasAtualizadas;
    int efeitosResolvidos;
    int grupoFugiu;
    int moedasDistribuidas;
    int xpDistribuido;
    int niveisGanhos;
} Combate;

void inicializaCombate(Combate *combate);
ResultadoCombate preparaCombate(Combate *combate, CadastroPersonagens *cadastro, const GrupoAventura *grupo,
                                const Encontro *encontro, const Bestiario *bestiario);
int contaPersonagensAtivos(const Combate *combate);
int contaMonstrosAtivos(const Combate *combate);
int combateTerminou(Combate *combate);
int combateEmAndamento(const Combate *combate);

void rolaIniciativas(Combate *combate);
void montaOrdemCombate(Combate *combate);
void exibeOrdem(const Combate *combate);
void exibeParticipantes(const Combate *combate);

int selecionaPersonagemAlvo(const Combate *combate);
int selecionaMonstroAlvo(const Combate *combate);

int defesaEquivalenteMonstro(const Monstro *monstro);
int bonusAtaqueMonstro(const Monstro *monstro);
int verificaAcerto(int resultadoD20, int bonusAtaque, int defesaAlvo);
int aplicaDanoPersonagem(PersonagemCombate *alvo, int dano);
int aplicaDanoMonstro(MonstroCombate *alvo, int dano);
ResultadoCombate executaAtaquePersonagem(Combate *combate, int atacante, int alvo);
ResultadoCombate executaAtaquesMonstro(Combate *combate, int atacante, int alvo);

ResultadoCombate configuraHabilidadesPersonagem(Combate *combate, int indice, int temCura, Dado dadoCura,
                                                int temMagia, Dado dadoMagia);
ResultadoCombate tentaFugaGrupo(Combate *combate);
ResultadoCombate usaItemEmCombate(Combate *combate, int indice, int idItem, UsoItem *uso);
int calculaRecompensaMoedas(const Combate *combate);
int calculaRecompensaXP(const Combate *combate);
void encerraBonusTemporarios(Combate *combate);
ResultadoCombate executaTurno(Combate *combate, int posicaoOrdem);
ResultadoCombate executaRodada(Combate *combate);
ResultadoCombate executaCombate(Combate *combate);

void exibeEstadoCombate(const Combate *combate);
void exibeResumoCombate(const Combate *combate);
ResultadoCombate salvaResumoCombate(const Combate *combate, const char *nomeArquivo);
int calculaXPDerrotados(const Combate *combate);
ResultadoCombate atualizaPersonagensAposCombate(Combate *combate);
void nomeMonstroCombate(const MonstroCombate *monstro, char *destino, int tamanho);
const char *resultadoCombateParaTexto(ResultadoCombate resultado);
const char *estadoCombatenteParaTexto(EstadoCombatente estado);

#endif
