#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include "som.h"

#include "tela.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#else
#include <unistd.h>
#endif

#define NOTAS_MAXIMAS 8
#define AMPLITUDE_BASE 8
#define AMPLITUDE_ATAQUE 48
#define NIVEL_SILENCIO 128
#define CAMINHO_TAMANHO 96
#define COMANDO_TAMANHO 200
#define CAMPAINHA "\a"

typedef struct {
    int frequencia;
    int milissegundos;
} Nota;

typedef struct {
    int quantidade;
    Nota notas[NOTAS_MAXIMAS];
} Melodia;

static const Melodia MELODIAS[SOM_NUM_EVENTOS] = {
    {6, {{262, 180}, {330, 180}, {392, 180}, {523, 360}, {392, 160}, {523, 500}}},
    {4, {{523, 110}, {659, 110}, {784, 110}, {1047, 330}}},
    {4, {{392, 220}, {330, 220}, {262, 220}, {196, 500}}},
    {4, {{784, 70}, {659, 70}, {523, 70}, {392, 140}}},
    {5, {{523, 90}, {659, 90}, {784, 90}, {1047, 90}, {1319, 260}}},
    {2, {{1319, 60}, {1568, 140}}},
    {2, {{196, 140}, {147, 220}}},
    {7, {{392, 140}, {523, 140}, {659, 140}, {784, 140}, {0, 60}, {784, 120}, {1047, 420}}}
};

int evento_som_eh_valido(EventoSom evento) {
    return evento >= SOM_ABERTURA && evento <= SOM_MISSAO;
}

const char *evento_som_para_texto(EventoSom evento) {
    switch (evento) {
        case SOM_ABERTURA: return "Abertura";
        case SOM_VITORIA: return "Vitoria";
        case SOM_DERROTA: return "Derrota";
        case SOM_FUGA: return "Fuga";
        case SOM_NIVEL: return "Subida de nivel";
        case SOM_MOEDAS: return "Moedas";
        case SOM_ERRO: return "Recusa";
        case SOM_MISSAO: return "Missao concluida";
        default: return "Som desconhecido";
    }
}

const char *saida_som_para_texto(SaidaSom saida) {
    switch (saida) {
        case SAIDA_SOM_CAMPAINHA: return "campainha do terminal";
        case SAIDA_SOM_SISTEMA: return "audio do Windows";
        case SAIDA_SOM_APLAY: return "aplay";
        case SAIDA_SOM_PAPLAY: return "paplay";
        default: return "saida desconhecida";
    }
}

static void escreve_inteiro(unsigned char destino[], int posicao, int bytes, long valor) {
    int i;

    for (i = 0; i < bytes; i++) {
        destino[posicao + i] = (unsigned char)((valor >> (8 * i)) & 0xFF);
    }
}

static void escreve_cabecalho(unsigned char destino[], int amostras) {
    memcpy(destino, "RIFF", 4);
    escreve_inteiro(destino, 4, 4, SOM_CABECALHO - 8 + amostras);
    memcpy(destino + 8, "WAVEfmt ", 8);
    escreve_inteiro(destino, 16, 4, 16);
    escreve_inteiro(destino, 20, 2, 1);
    escreve_inteiro(destino, 22, 2, 1);
    escreve_inteiro(destino, 24, 4, SOM_TAXA);
    escreve_inteiro(destino, 28, 4, SOM_TAXA);
    escreve_inteiro(destino, 32, 2, 1);
    escreve_inteiro(destino, 34, 2, 8);
    memcpy(destino + 36, "data", 4);
    escreve_inteiro(destino, 40, 4, amostras);
}

int som_gera_onda(EventoSom evento, unsigned char destino[], int capacidade) {
    const Melodia *melodia;
    int amostras = 0;
    int duracao;
    int amplitude;
    int nota;
    int i;

    if (!evento_som_eh_valido(evento) || destino == 0 || capacidade <= SOM_CABECALHO) {
        return 0;
    }
    melodia = &MELODIAS[evento - 1];
    for (nota = 0; nota < melodia->quantidade; nota++) {
        duracao = melodia->notas[nota].milissegundos * SOM_TAXA / 1000;
        for (i = 0; i < duracao && SOM_CABECALHO + amostras < capacidade; i++) {
            amplitude = 0;
            if (melodia->notas[nota].frequencia > 0) {
                amplitude = AMPLITUDE_BASE + AMPLITUDE_ATAQUE * (duracao - i) / duracao;
                if ((i * melodia->notas[nota].frequencia * 2 / SOM_TAXA) % 2 != 0) {
                    amplitude = -amplitude;
                }
            }
            destino[SOM_CABECALHO + amostras] = (unsigned char)(NIVEL_SILENCIO + amplitude);
            amostras++;
        }
    }
    escreve_cabecalho(destino, amostras);
    return SOM_CABECALHO + amostras;
}

#ifndef _WIN32
static int comando_existe(const char *nome) {
    char comando[COMANDO_TAMANHO];

    snprintf(comando, sizeof(comando), "command -v %s >/dev/null 2>&1", nome);
    return system(comando) == 0;
}

static void caminho_do_evento(EventoSom evento, char destino[], int tamanho) {
    snprintf(destino, (size_t)tamanho, "/tmp/falmuth_som_%ld_%d.wav", (long)getpid(), (int)evento);
}

static int grava_evento(Som *som, EventoSom evento) {
    char caminho[CAMINHO_TAMANHO];
    FILE *arquivo;
    size_t escritos;

    if (som->gravado[evento]) {
        return 1;
    }
    som->tamanho = som_gera_onda(evento, som->onda, SOM_TAMANHO_MAXIMO);
    caminho_do_evento(evento, caminho, sizeof(caminho));
    arquivo = fopen(caminho, "wb");
    if (arquivo == 0) {
        return 0;
    }
    escritos = fwrite(som->onda, 1, (size_t)som->tamanho, arquivo);
    if (fclose(arquivo) != 0 || escritos != (size_t)som->tamanho) {
        remove(caminho);
        return 0;
    }
    som->gravado[evento] = 1;
    return 1;
}
#endif

void som_inicializa(Som *som) {
    if (som == 0) {
        return;
    }
    memset(som, 0, sizeof(*som));
    som->ligado = tela_interativa();
    som->saida = SAIDA_SOM_CAMPAINHA;
    if (!som->ligado) {
        return;
    }
#ifdef _WIN32
    som->saida = SAIDA_SOM_SISTEMA;
#else
    if (comando_existe("aplay")) {
        som->saida = SAIDA_SOM_APLAY;
    } else if (comando_existe("paplay")) {
        som->saida = SAIDA_SOM_PAPLAY;
    }
#endif
}

static void toca_campainha(void) {
    fputs(CAMPAINHA, stdout);
    fflush(stdout);
}

void som_toca(Som *som, EventoSom evento) {
#ifndef _WIN32
    char caminho[CAMINHO_TAMANHO];
    char comando[COMANDO_TAMANHO];
#endif

    if (som == 0 || !som->ligado || !evento_som_eh_valido(evento) || !tela_interativa()) {
        return;
    }
    if (som->saida == SAIDA_SOM_CAMPAINHA) {
        toca_campainha();
        return;
    }
#ifdef _WIN32
    PlaySoundA(0, 0, 0);
    som->tamanho = som_gera_onda(evento, som->onda, SOM_TAMANHO_MAXIMO);
    if (som->tamanho == 0
        || !PlaySoundA((LPCSTR)som->onda, 0, SND_MEMORY | SND_ASYNC | SND_NODEFAULT)) {
        som->saida = SAIDA_SOM_CAMPAINHA;
        toca_campainha();
    }
#else
    if (!grava_evento(som, evento)) {
        som->saida = SAIDA_SOM_CAMPAINHA;
        toca_campainha();
        return;
    }
    caminho_do_evento(evento, caminho, sizeof(caminho));
    snprintf(comando, sizeof(comando), "%s %s >/dev/null 2>&1 &",
             som->saida == SAIDA_SOM_APLAY ? "aplay -q" : "paplay", caminho);
    if (system(comando) != 0) {
        som->saida = SAIDA_SOM_CAMPAINHA;
        toca_campainha();
    }
#endif
}

int som_alterna(Som *som) {
    if (som == 0) {
        return 0;
    }
    som->ligado = !som->ligado;
#ifdef _WIN32
    if (!som->ligado) {
        PlaySoundA(0, 0, 0);
    }
#endif
    return som->ligado;
}

void som_encerra(Som *som) {
#ifndef _WIN32
    char caminho[CAMINHO_TAMANHO];
    int evento;
#endif

    if (som == 0) {
        return;
    }
#ifdef _WIN32
    PlaySoundA(0, 0, SND_NODEFAULT);
#else
    for (evento = SOM_ABERTURA; evento <= SOM_MISSAO; evento++) {
        if (som->gravado[evento]) {
            caminho_do_evento((EventoSom)evento, caminho, sizeof(caminho));
            remove(caminho);
            som->gravado[evento] = 0;
        }
    }
#endif
    som->ligado = 0;
}
