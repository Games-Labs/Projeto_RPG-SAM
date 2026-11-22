#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "grupo.h"
#include "personagem.h"

#define PERSISTENCIA_ERRO_ARQUIVO (-1)
#define PERSISTENCIA_PROGRESSO_MAXIMO 1000

typedef struct {
    int idsGrupo[MAX_PERSONAGENS_GRUPO];
    int quantidadeGrupo;
    int missoesPrincipais;
    int missoesSecundarias;
} EstadoSalvo;

void inicializaEstadoSalvo(EstadoSalvo *estado);
int salvaJogo(const CadastroPersonagens *cadastro, const EstadoSalvo *estado, const char *nomeArquivo);
int carregaJogo(CadastroPersonagens *cadastro, EstadoSalvo *estado, const char *nomeArquivo);

int salvaPersonagens(const CadastroPersonagens *cadastro, const char *nomeArquivo);
int carregaPersonagens(CadastroPersonagens *cadastro, const char *nomeArquivo);

#endif
