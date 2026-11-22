#ifndef SOM_H
#define SOM_H

#define SOM_TAXA 11025
#define SOM_CABECALHO 44
#define SOM_DURACAO_MAXIMA_MS 2000
#define SOM_TAMANHO_MAXIMO (SOM_CABECALHO + SOM_TAXA * SOM_DURACAO_MAXIMA_MS / 1000)
#define SOM_NUM_EVENTOS 8

typedef enum {
    SOM_ABERTURA = 1,
    SOM_VITORIA,
    SOM_DERROTA,
    SOM_FUGA,
    SOM_NIVEL,
    SOM_MOEDAS,
    SOM_ERRO,
    SOM_MISSAO
} EventoSom;

typedef enum {
    SAIDA_SOM_CAMPAINHA,
    SAIDA_SOM_SISTEMA,
    SAIDA_SOM_APLAY,
    SAIDA_SOM_PAPLAY
} SaidaSom;

typedef struct {
    int ligado;
    SaidaSom saida;
    int gravado[SOM_NUM_EVENTOS + 1];
    int tamanho;
    unsigned char onda[SOM_TAMANHO_MAXIMO];
} Som;

int evento_som_eh_valido(EventoSom evento);
const char *evento_som_para_texto(EventoSom evento);
const char *saida_som_para_texto(SaidaSom saida);
void som_inicializa(Som *som);
int som_gera_onda(EventoSom evento, unsigned char destino[], int capacidade);
void som_toca(Som *som, EventoSom evento);
int som_alterna(Som *som);
void som_encerra(Som *som);

#endif
