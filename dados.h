#ifndef DADOS_H
#define DADOS_H

#define DADO_QUANTIDADE_MAXIMA 100
#define DADO_FACES_MAXIMAS 1000
#define DADO_MODIFICADOR_MAXIMO 1000

typedef struct {
    int quantidade;
    int faces;
    int modificador;
} Dado;

int converteTextoDado(const char *texto, Dado *dado);
void dadoParaTexto(Dado dado, char *destino, int tamanho);
int validaDado(Dado dado);

void inicializaAleatorio(unsigned int semente);
unsigned int inicializaAleatorioPorTempo(void);
int sorteiaInteiro(int minimo, int maximo);
int rolaDado(Dado dado);

#endif
