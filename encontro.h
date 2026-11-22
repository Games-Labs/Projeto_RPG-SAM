#ifndef ENCONTRO_H
#define ENCONTRO_H

#include "bestiario.h"
#include "dados.h"
#include "grupo.h"

#define MAX_CANDIDATOS MAX_MONSTROS
#define MAX_ESPECIES_ENCONTRO 4
#define MAX_MONSTROS_ENCONTRO 50
#define NUM_PROPOSTAS 3
#define MAX_TENTATIVAS_PROPOSTA 5000
#define MAX_SORTEIOS_ESPECIE 20

typedef enum {
    FACIL = 1,
    MEDIA,
    DIFICIL
} Dificuldade;

typedef enum {
    PERIODO_DIA = 1,
    PERIODO_NOITE
} Periodo;

typedef enum {
    ENCONTRO_OK,
    ENCONTRO_PARCIAL,
    ENCONTRO_NENHUMA_PROPOSTA,
    ENCONTRO_SEM_CANDIDATOS,
    ENCONTRO_GRUPO_INVALIDO,
    ENCONTRO_CONTEXTO_INVALIDO,
    ENCONTRO_FAIXA_INVALIDA,
    ENCONTRO_TENTATIVA_REJEITADA,
    ENCONTRO_ESCOLHA_INVALIDA,
    ENCONTRO_DADOS_INVALIDOS
} ResultadoEncontro;

typedef struct {
    Terreno terreno;
    Periodo periodo;
    Dificuldade dificuldade;
} ContextoEncontro;

typedef struct {
    const Monstro *itens[MAX_CANDIDATOS];
    int quantidade;
} ListaCandidatos;

typedef struct {
    int idMonstro;
    int quantidade;
    int poderCalculado;
} ComponenteEncontro;

typedef struct {
    ComponenteEncontro componentes[MAX_ESPECIES_ENCONTRO];
    int quantidadeEspecies;
    int quantidadeMonstros;
    int poderTotal;
} Encontro;

typedef struct {
    Encontro propostas[NUM_PROPOSTAS];
    int quantidade;
    int indiceEscolhido;
} ConjuntoPropostas;

int dificuldadeEhValida(Dificuldade dificuldade);
int periodoEhValido(Periodo periodo);
int contextoEhValido(const ContextoEncontro *contexto);
const char *dificuldadeParaTexto(Dificuldade dificuldade);
const char *periodoParaTexto(Periodo periodo);
const char *resultadoEncontroParaTexto(ResultadoEncontro resultado);

void inicializaEncontro(Encontro *e);
void inicializaPropostas(ConjuntoPropostas *conjunto);

void calculaFaixaDificuldade(int poderGrupo, Dificuldade dificuldade, int *minimo, int *maximo);
int monstroCompativel(const Monstro *m, const ContextoEncontro *contexto);
ResultadoEncontro filtraCandidatos(const Bestiario *b, const ContextoEncontro *contexto, ListaCandidatos *saida);

int poderIndividualContextual(const Monstro *m, const ContextoEncontro *contexto);
int poderComponente(const Monstro *m, int quantidade, const ContextoEncontro *contexto);
int calculaPoderEncontro(Encontro *e, const Bestiario *b, const ContextoEncontro *contexto);

int pesoFrequencia(Frequencia frequencia);
const Monstro *sorteiaMonstroPonderado(const ListaCandidatos *candidatos);
ResultadoEncontro tentaGerarEncontro(const Bestiario *b, const ListaCandidatos *candidatos,
                                     const ContextoEncontro *contexto, int poderMinimo, int poderMaximo,
                                     Encontro *saida);
int encontrosIguais(const Encontro *a, const Encontro *b);
ResultadoEncontro geraPropostas(const Bestiario *b, const GrupoAventura *grupo,
                                const ContextoEncontro *contexto, ConjuntoPropostas *saida);
ResultadoEncontro escolheProposta(ConjuntoPropostas *conjunto, int opcao);
void exibeEncontro(const Encontro *e, const Bestiario *b);

#endif
