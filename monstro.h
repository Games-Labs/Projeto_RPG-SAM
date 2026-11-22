#ifndef MONSTRO_H
#define MONSTRO_H

#include "dados.h"

#define MAX_CLIMAS 5
#define MAX_TERRENOS 8
#define MAX_MOVIMENTOS 4
#define MAX_ATAQUES 6
#define MAX_ESPECIAIS 6

#define MONSTRO_NOME_TAMANHO 50
#define MONSTRO_DESCRICAO_TAMANHO 500
#define MONSTRO_ORGANIZACAO_TAMANHO 40
#define MONSTRO_TESOURO_TAMANHO 80
#define MONSTRO_CLASSIFICACAO_TAMANHO 30
#define ATAQUE_NOME_TAMANHO 40
#define ESPECIAL_DESCRICAO_TAMANHO 120

#define NUM_CAMPOS_MONSTRO 38

#define DADOS_VIDA_FACES 8

#define CA_MINIMA 0
#define CA_MAXIMA 9
#define THAC0_MINIMO 9
#define THAC0_MAXIMO 20
#define INTELIGENCIA_MINIMA 0
#define INTELIGENCIA_MAXIMA 15
#define MORAL_MINIMA 6
#define MORAL_MAXIMA 20
#define XP_MINIMO 1
#define XP_MAXIMO 9999
#define RESISTENCIA_MAGIA_MINIMA 0
#define RESISTENCIA_MAGIA_MAXIMA 100
#define COVIL_MAXIMO 100000
#define DESLOCAMENTO_MAXIMO 200
#define ALTURA_MAXIMA 1000.0f
#define PODER_BASE_MINIMO 1
#define PODER_BASE_MAXIMO 1000
#define MODIFICADOR_PODER_MAXIMO 50
#define BONUS_PODER_MAXIMO 100
#define LIMITE_GRUPO_GRANDE_MAXIMO 50

typedef enum {
    FREQUENCIA_MUITO_RARO = 1,
    FREQUENCIA_RARO,
    FREQUENCIA_INCOMUM,
    FREQUENCIA_COMUM,
    FREQUENCIA_MUITO_COMUM
} Frequencia;

typedef enum {
    CLIMA_QUALQUER = 1,
    CLIMA_TEMPERADO,
    CLIMA_TROPICAL,
    CLIMA_FRIO,
    CLIMA_ARIDO
} Clima;

typedef enum {
    TERRENO_CAVERNA = 1,
    TERRENO_RUINAS,
    TERRENO_FLORESTA,
    TERRENO_MONTANHA,
    TERRENO_PLANICIE,
    TERRENO_PANTANO,
    TERRENO_URBANO,
    TERRENO_DESERTO
} Terreno;

typedef enum {
    ATIVIDADE_DIURNO = 1,
    ATIVIDADE_NOTURNO,
    ATIVIDADE_QUALQUER_HORARIO
} Atividade;

typedef enum {
    DIETA_CARNIVORO = 1,
    DIETA_HERBIVORO,
    DIETA_ONIVORO,
    DIETA_PARASITA,
    DIETA_NAO_SE_ALIMENTA
} Dieta;

typedef enum {
    MOVIMENTO_TERRESTRE = 1,
    MOVIMENTO_VOO,
    MOVIMENTO_NATACAO,
    MOVIMENTO_ESCAVACAO
} TipoMovimento;

typedef enum {
    MANOBRABILIDADE_NENHUMA = 1,
    MANOBRABILIDADE_A,
    MANOBRABILIDADE_B,
    MANOBRABILIDADE_C,
    MANOBRABILIDADE_D,
    MANOBRABILIDADE_E
} Manobrabilidade;

typedef enum {
    TAMANHO_PEQUENO = 1,
    TAMANHO_MEDIO,
    TAMANHO_GRANDE,
    TAMANHO_ENORME,
    TAMANHO_GIGANTESCO
} Tamanho;

typedef enum {
    ESPECIAL_ABRACO = 1,
    ESPECIAL_AGARRAR,
    ESPECIAL_ARMADILHA,
    ESPECIAL_ARQUEARIA,
    ESPECIAL_ARREMESSAR_PEDRA,
    ESPECIAL_CAMUFLAGEM,
    ESPECIAL_CANTO,
    ESPECIAL_CANTO_ABISSAL,
    ESPECIAL_CARAPACA,
    ESPECIAL_CORROSAO,
    ESPECIAL_DERRUBAR,
    ESPECIAL_DISSOLVER,
    ESPECIAL_DOENCA,
    ESPECIAL_DRENAR_FORCA,
    ESPECIAL_DRENAR_SANGUE,
    ESPECIAL_DRENO_NIVEL,
    ESPECIAL_ECOLOCALIZACAO,
    ESPECIAL_EMBOSCADA,
    ESPECIAL_ENCANTO,
    ESPECIAL_ENVOLVER,
    ESPECIAL_ESCALADA,
    ESPECIAL_FORMACAO,
    ESPECIAL_FURIA,
    ESPECIAL_ILUSAO,
    ESPECIAL_IMATERIAL,
    ESPECIAL_IMUNIDADE,
    ESPECIAL_INCENDIAR,
    ESPECIAL_INVESTIDA,
    ESPECIAL_INVISIBILIDADE,
    ESPECIAL_LINGUA,
    ESPECIAL_MAGIA,
    ESPECIAL_MEDO,
    ESPECIAL_MERGULHO,
    ESPECIAL_MUSICA,
    ESPECIAL_ODOR,
    ESPECIAL_ORIENTACAO,
    ESPECIAL_PARALISIA,
    ESPECIAL_PASSO_ARBOREO,
    ESPECIAL_PELE_PEDRA,
    ESPECIAL_PETRIFICACAO,
    ESPECIAL_RASTREAR,
    ESPECIAL_REDEMOINHO,
    ESPECIAL_REGENERACAO,
    ESPECIAL_RESISTENCIA_MAGICA,
    ESPECIAL_RESPIRACAO_AQUATICA,
    ESPECIAL_SOPRO,
    ESPECIAL_TEIA,
    ESPECIAL_TENACIDADE,
    ESPECIAL_TRANSPARENTE,
    ESPECIAL_TREMOR,
    ESPECIAL_VENENO,
    ESPECIAL_VISAO_AGUCADA
} TipoEspecial;

typedef enum {
    ORDEM_LEAL = 1,
    ORDEM_NEUTRO,
    ORDEM_CAOTICO
} EixoOrdem;

typedef enum {
    MORAL_BOM = 1,
    MORAL_NEUTRO,
    MORAL_MAU
} EixoMoral;

#define DECLARA_ENUM(Tipo, prefixo) \
    int prefixo##DeTexto(const char *texto, Tipo *valor); \
    const char *prefixo##ParaTexto(Tipo valor); \
    int prefixo##EhValido(Tipo valor);

DECLARA_ENUM(Frequencia, frequencia)
DECLARA_ENUM(Clima, clima)
DECLARA_ENUM(Terreno, terreno)
DECLARA_ENUM(Atividade, atividade)
DECLARA_ENUM(Dieta, dieta)
DECLARA_ENUM(TipoMovimento, tipoMovimento)
DECLARA_ENUM(Manobrabilidade, manobrabilidade)
DECLARA_ENUM(Tamanho, tamanho)
DECLARA_ENUM(TipoEspecial, tipoEspecial)
DECLARA_ENUM(EixoOrdem, eixoOrdem)
DECLARA_ENUM(EixoMoral, eixoMoral)

typedef struct {
    TipoMovimento tipo;
    int deslocamento;
    Manobrabilidade manobrabilidade;
} Movimento;

typedef struct {
    char nome[ATAQUE_NOME_TAMANHO];
    Dado dano;
} Ataque;

typedef struct {
    TipoEspecial tipo;
    int valor;
    char descricao[ESPECIAL_DESCRICAO_TAMANHO];
} HabilidadeEspecial;

typedef struct {
    EixoOrdem ordem;
    EixoMoral moral;
} Tendencia;

typedef struct {
    Terreno terreno;
    int modificador;
} ModificadorTerreno;

typedef struct {
    int poderBase;
    int modDia;
    int modNoite;
    ModificadorTerreno modTerreno[MAX_TERRENOS];
    int quantidadeModTerreno;
    int bonusPorAdicional;
    int limiteGrupoGrande;
    int bonusGrupoGrande;
} PoderEncontro;

typedef struct {
    int id;
    char nome[MONSTRO_NOME_TAMANHO];
    char descricao[MONSTRO_DESCRICAO_TAMANHO];
    char descricaoCombate[MONSTRO_DESCRICAO_TAMANHO];
    char habitatSociedade[MONSTRO_DESCRICAO_TAMANHO];
    char descricaoEcologia[MONSTRO_DESCRICAO_TAMANHO];

    Frequencia frequencia;
    Dado numeroEncontrado;
    int minimoCovil, maximoCovil;

    Clima climas[MAX_CLIMAS];
    int quantidadeClimas;
    Terreno terrenos[MAX_TERRENOS];
    int quantidadeTerrenos;

    char organizacao[MONSTRO_ORGANIZACAO_TAMANHO];
    Atividade atividade;
    Dieta dieta;
    int inteligencia;
    char tesouro[MONSTRO_TESOURO_TAMANHO];
    Tendencia tendencia;

    int classeArmadura;
    Movimento movimentos[MAX_MOVIMENTOS];
    int quantidadeMovimentos;
    Dado dadosVida;
    int thac0;

    Ataque ataques[MAX_ATAQUES];
    int quantidadeAtaques;
    HabilidadeEspecial ataquesEspeciais[MAX_ESPECIAIS];
    int quantidadeAtaquesEspeciais;
    HabilidadeEspecial defesasEspeciais[MAX_ESPECIAIS];
    int quantidadeDefesasEspeciais;

    int resistenciaMagia;
    Tamanho tamanho;
    float alturaMinima, alturaMaxima;
    int moral;
    char classificacaoMoral[MONSTRO_CLASSIFICACAO_TAMANHO];

    int valorXP;

    PoderEncontro poder;
} Monstro;

int validaMonstro(const Monstro *m);
int validaMonstroMotivo(const Monstro *m, char *motivo, int tamanho);
void exibeMonstro(const Monstro *m);

int viveNoTerreno(const Monstro *m, Terreno terreno);
int ativoNoPeriodo(const Monstro *m, Atividade periodo);

int monstroIndiceCampo(const char *chave);
const char *monstroNomeCampo(int indice);
int monstroDefineCampo(Monstro *m, const char *chave, const char *valor, char *erro, int tamanhoErro);
int monstroCampoParaTexto(const Monstro *m, int indice, char *destino, int tamanho);

#endif
