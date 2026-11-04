#ifndef GRUPO_H
#define GRUPO_H

#include "personagem.h"

#define MAX_PERSONAGENS_GRUPO 6

typedef enum {
    GRUPO_OK,
    GRUPO_CHEIO,
    GRUPO_PERSONAGEM_REPETIDO,
    GRUPO_PERSONAGEM_INEXISTENTE,
    GRUPO_PERSONAGEM_NAO_ESTA_NO_GRUPO,
    GRUPO_DADOS_INVALIDOS
} ResultadoGrupo;

typedef struct {
    int idsPersonagens[MAX_PERSONAGENS_GRUPO];
    int quantidade;
    int poderTotal;
} GrupoAventura;

void inicializaGrupo(GrupoAventura *grupo);
ResultadoGrupo adicionaPersonagemGrupo(GrupoAventura *grupo, const CadastroPersonagens *cadastro, int idPersonagem);
ResultadoGrupo removePersonagemGrupo(GrupoAventura *grupo, int idPersonagem);
int calculaPoderGrupo(GrupoAventura *grupo, const CadastroPersonagens *cadastro);
void exibeGrupo(const GrupoAventura *grupo, const CadastroPersonagens *cadastro);
const char *resultadoGrupoParaTexto(ResultadoGrupo resultado);

#endif
