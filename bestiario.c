#include "bestiario.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define LINHA_MAXIMA 2048
#define VALOR_MAXIMO 4096

typedef struct {
    ResultadoBestiario primeiro;
    int linha;
    int dentro;
    int invalido;
    int linhaInicio;
    Monstro atual;
    unsigned char vistos[NUM_CAMPOS_MONSTRO];
} EstadoLeitura;

const char *resultadoBestiarioParaTexto(ResultadoBestiario resultado) {
    switch (resultado) {
        case BESTIARIO_OK: return "Operacao realizada com sucesso.";
        case BESTIARIO_ARQUIVO_INEXISTENTE: return "Arquivo inexistente ou ilegivel.";
        case BESTIARIO_ERRO_FORMATO: return "Erro de formato no arquivo.";
        case BESTIARIO_REGISTRO_INVALIDO: return "Registro invalido.";
        case BESTIARIO_REGISTRO_INCOMPLETO: return "Registro incompleto.";
        case BESTIARIO_CHEIO: return "Bestiario cheio.";
        case BESTIARIO_ID_DUPLICADO: return "ID duplicado.";
        case BESTIARIO_NAO_ENCONTRADO: return "Monstro nao encontrado.";
        case BESTIARIO_DADOS_INVALIDOS: return "Dados invalidos.";
        case BESTIARIO_ERRO_ESCRITA: return "Erro ao gravar o arquivo.";
        default: return "Resultado desconhecido.";
    }
}

void inicializaBestiario(Bestiario *b) {
    if (b == 0) {
        return;
    }
    b->quantidade = 0;
    b->rejeitados = 0;
    b->primeiroErro[0] = '\0';
}

static int indiceDoId(const Bestiario *b, int id) {
    int i;

    for (i = 0; i < b->quantidade; i++) {
        if (b->monstros[i].id == id) {
            return i;
        }
    }
    return -1;
}

ResultadoBestiario insereMonstro(Bestiario *b, Monstro monstro) {
    if (b == 0 || !validaMonstro(&monstro)) {
        return BESTIARIO_DADOS_INVALIDOS;
    }
    if (b->quantidade >= MAX_MONSTROS) {
        return BESTIARIO_CHEIO;
    }
    if (indiceDoId(b, monstro.id) >= 0) {
        return BESTIARIO_ID_DUPLICADO;
    }
    b->monstros[b->quantidade] = monstro;
    b->quantidade++;
    return BESTIARIO_OK;
}

Monstro *buscaMonstroPorId(Bestiario *b, int id) {
    int indice;

    if (b == 0) {
        return 0;
    }
    indice = indiceDoId(b, id);
    return indice < 0 ? 0 : &b->monstros[indice];
}

const Monstro *consultaMonstroPorId(const Bestiario *b, int id) {
    int indice;

    if (b == 0) {
        return 0;
    }
    indice = indiceDoId(b, id);
    return indice < 0 ? 0 : &b->monstros[indice];
}

static int nomesIguais(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0') {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == *b;
}

Monstro *buscaMonstroPorNome(Bestiario *b, const char *nome) {
    int i;

    if (b == 0 || nome == 0) {
        return 0;
    }
    for (i = 0; i < b->quantidade; i++) {
        if (nomesIguais(b->monstros[i].nome, nome)) {
            return &b->monstros[i];
        }
    }
    return 0;
}

ResultadoBestiario alteraMonstro(Bestiario *b, int id, Monstro novosDados) {
    int indice;

    if (b == 0 || !validaMonstro(&novosDados)) {
        return BESTIARIO_DADOS_INVALIDOS;
    }
    indice = indiceDoId(b, id);
    if (indice < 0) {
        return BESTIARIO_NAO_ENCONTRADO;
    }
    if (novosDados.id != id && indiceDoId(b, novosDados.id) >= 0) {
        return BESTIARIO_ID_DUPLICADO;
    }
    b->monstros[indice] = novosDados;
    return BESTIARIO_OK;
}

ResultadoBestiario removeMonstro(Bestiario *b, int id) {
    int indice;
    int i;

    if (b == 0) {
        return BESTIARIO_DADOS_INVALIDOS;
    }
    indice = indiceDoId(b, id);
    if (indice < 0) {
        return BESTIARIO_NAO_ENCONTRADO;
    }
    for (i = indice; i < b->quantidade - 1; i++) {
        b->monstros[i] = b->monstros[i + 1];
    }
    b->quantidade--;
    return BESTIARIO_OK;
}

void listaMonstros(const Bestiario *b) {
    int i;

    if (b == 0 || b->quantidade == 0) {
        printf("Nenhum monstro cadastrado.\n");
        return;
    }
    printf("Quantidade cadastrada: %d/%d\n", b->quantidade, MAX_MONSTROS);
    for (i = 0; i < b->quantidade; i++) {
        printf("%3d | %-24s | %-11s | %-16s | poder base %d\n", b->monstros[i].id, b->monstros[i].nome,
               frequenciaParaTexto(b->monstros[i].frequencia), atividadeParaTexto(b->monstros[i].atividade),
               b->monstros[i].poder.poderBase);
    }
}

int listaMonstrosPorTerreno(const Bestiario *b, Terreno terreno, const Monstro *resultado[], int capacidadeResultado) {
    int i;
    int total = 0;

    if (b == 0 || resultado == 0 || capacidadeResultado <= 0 || !terrenoEhValido(terreno)) {
        return 0;
    }
    for (i = 0; i < b->quantidade && total < capacidadeResultado; i++) {
        if (viveNoTerreno(&b->monstros[i], terreno)) {
            resultado[total++] = &b->monstros[i];
        }
    }
    return total;
}

int listaMonstrosPorPeriodo(const Bestiario *b, Atividade periodo, const Monstro *resultado[], int capacidadeResultado) {
    int i;
    int total = 0;

    if (b == 0 || resultado == 0 || capacidadeResultado <= 0 || !atividadeEhValido(periodo)) {
        return 0;
    }
    for (i = 0; i < b->quantidade && total < capacidadeResultado; i++) {
        if (ativoNoPeriodo(&b->monstros[i], periodo)) {
            resultado[total++] = &b->monstros[i];
        }
    }
    return total;
}

static void registraFalha(Bestiario *b, EstadoLeitura *estado, ResultadoBestiario codigo, int linha, const char *mensagem) {
    if (estado->primeiro == BESTIARIO_OK) {
        estado->primeiro = codigo;
        snprintf(b->primeiroErro, sizeof(b->primeiroErro), "linha %d: %s", linha, mensagem);
    }
    if (estado->dentro && !estado->invalido) {
        estado->invalido = 1;
        b->rejeitados++;
    } else if (!estado->dentro) {
        b->rejeitados++;
    }
}

static char *removeQuebra(char *linha) {
    size_t tamanho = strlen(linha);

    while (tamanho > 0 && (linha[tamanho - 1] == '\n' || linha[tamanho - 1] == '\r')) {
        linha[--tamanho] = '\0';
    }
    while (*linha == ' ' || *linha == '\t') {
        linha++;
    }
    return linha;
}

static void iniciaRegistro(EstadoLeitura *estado) {
    memset(&estado->atual, 0, sizeof(estado->atual));
    memset(estado->vistos, 0, sizeof(estado->vistos));
    estado->dentro = 1;
    estado->invalido = 0;
    estado->linhaInicio = estado->linha;
}

static void fechaRegistro(Bestiario *b, EstadoLeitura *estado) {
    char motivo[BESTIARIO_ERRO_TAMANHO - 20];
    char mensagem[BESTIARIO_ERRO_TAMANHO - 20];
    ResultadoBestiario resultado;
    int i;

    if (!estado->invalido) {
        for (i = 0; i < NUM_CAMPOS_MONSTRO; i++) {
            if (!estado->vistos[i]) {
                snprintf(mensagem, sizeof(mensagem), "registro da linha %d sem a chave obrigatoria '%s'",
                         estado->linhaInicio, monstroNomeCampo(i));
                registraFalha(b, estado, BESTIARIO_REGISTRO_INVALIDO, estado->linha, mensagem);
                break;
            }
        }
    }

    if (!estado->invalido && !validaMonstroMotivo(&estado->atual, motivo, sizeof(motivo))) {
        snprintf(mensagem, sizeof(mensagem), "registro da linha %d invalido: %s", estado->linhaInicio, motivo);
        registraFalha(b, estado, BESTIARIO_REGISTRO_INVALIDO, estado->linha, mensagem);
    }

    if (!estado->invalido) {
        resultado = insereMonstro(b, estado->atual);
        if (resultado != BESTIARIO_OK) {
            snprintf(mensagem, sizeof(mensagem), "registro da linha %d recusado: %s id %d",
                     estado->linhaInicio, resultadoBestiarioParaTexto(resultado), estado->atual.id);
            registraFalha(b, estado, resultado, estado->linha, mensagem);
        }
    }

    estado->dentro = 0;
    estado->invalido = 0;
}

static void interpretaCampo(Bestiario *b, EstadoLeitura *estado, char *texto) {
    char erro[BESTIARIO_ERRO_TAMANHO - 20];
    char *igual = strchr(texto, '=');
    int indice;

    if (igual == 0) {
        registraFalha(b, estado, BESTIARIO_ERRO_FORMATO, estado->linha, "linha sem '=' dentro do registro");
        return;
    }
    *igual = '\0';
    indice = monstroDefineCampo(&estado->atual, texto, igual + 1, erro, sizeof(erro));
    if (indice < 0) {
        registraFalha(b, estado, BESTIARIO_ERRO_FORMATO, estado->linha, erro);
        return;
    }
    if (estado->vistos[indice]) {
        snprintf(erro, sizeof(erro), "chave repetida '%s'", monstroNomeCampo(indice));
        registraFalha(b, estado, BESTIARIO_ERRO_FORMATO, estado->linha, erro);
        return;
    }
    estado->vistos[indice] = 1;
}

static void descartaRestoDaLinha(FILE *arquivo) {
    int caractere;

    while ((caractere = fgetc(arquivo)) != '\n' && caractere != EOF) {
    }
}

ResultadoBestiario carregaBestiario(Bestiario *b, const char *nomeArquivo) {
    FILE *arquivo;
    char linha[LINHA_MAXIMA];
    char *texto;
    EstadoLeitura estado;
    size_t tamanho;

    if (b == 0 || nomeArquivo == 0) {
        return BESTIARIO_DADOS_INVALIDOS;
    }

    inicializaBestiario(b);
    arquivo = fopen(nomeArquivo, "r");
    if (arquivo == 0) {
        snprintf(b->primeiroErro, sizeof(b->primeiroErro), "nao foi possivel abrir '%s'", nomeArquivo);
        return BESTIARIO_ARQUIVO_INEXISTENTE;
    }

    estado.primeiro = BESTIARIO_OK;
    estado.linha = 0;
    estado.dentro = 0;
    estado.invalido = 0;
    estado.linhaInicio = 0;

    while (fgets(linha, sizeof(linha), arquivo) != 0) {
        estado.linha++;
        tamanho = strlen(linha);
        if (tamanho > 0 && linha[tamanho - 1] != '\n' && !feof(arquivo)) {
            descartaRestoDaLinha(arquivo);
            registraFalha(b, &estado, BESTIARIO_ERRO_FORMATO, estado.linha, "linha longa demais");
            continue;
        }

        texto = removeQuebra(linha);
        if (texto[0] == '\0' || texto[0] == '#') {
            continue;
        }

        if (strcmp(texto, "[MONSTRO]") == 0) {
            if (estado.dentro) {
                if (!estado.invalido) {
                    registraFalha(b, &estado, BESTIARIO_REGISTRO_INCOMPLETO, estado.linhaInicio,
                                  "registro sem [/MONSTRO]");
                }
            }
            iniciaRegistro(&estado);
        } else if (strcmp(texto, "[/MONSTRO]") == 0) {
            if (!estado.dentro) {
                registraFalha(b, &estado, BESTIARIO_ERRO_FORMATO, estado.linha, "[/MONSTRO] sem [MONSTRO]");
            } else {
                fechaRegistro(b, &estado);
            }
        } else if (!estado.dentro) {
            registraFalha(b, &estado, BESTIARIO_ERRO_FORMATO, estado.linha, "conteudo fora de um registro");
        } else if (!estado.invalido) {
            interpretaCampo(b, &estado, texto);
        }
    }

    if (estado.dentro && !estado.invalido) {
        registraFalha(b, &estado, BESTIARIO_REGISTRO_INCOMPLETO, estado.linhaInicio, "registro sem [/MONSTRO]");
    }

    fclose(arquivo);
    return estado.primeiro;
}

ResultadoBestiario salvaBestiario(const Bestiario *b, const char *nomeArquivo) {
    FILE *arquivo;
    char valor[VALOR_MAXIMO];
    int i;
    int campo;

    if (b == 0 || nomeArquivo == 0) {
        return BESTIARIO_DADOS_INVALIDOS;
    }

    arquivo = fopen(nomeArquivo, "w");
    if (arquivo == 0) {
        return BESTIARIO_ERRO_ESCRITA;
    }

    fprintf(arquivo, "# BESTIARIO DO SISTEMA AUXILIAR DE MESTRE DE RPG\n# Codificacao: UTF-8\n\n");
    for (i = 0; i < b->quantidade; i++) {
        fprintf(arquivo, "[MONSTRO]\n");
        for (campo = 0; campo < NUM_CAMPOS_MONSTRO; campo++) {
            if (!monstroCampoParaTexto(&b->monstros[i], campo, valor, sizeof(valor))) {
                fclose(arquivo);
                return BESTIARIO_ERRO_ESCRITA;
            }
            fprintf(arquivo, "%s=%s\n", monstroNomeCampo(campo), valor);
        }
        fprintf(arquivo, "[/MONSTRO]\n\n");
    }

    if (ferror(arquivo)) {
        fclose(arquivo);
        return BESTIARIO_ERRO_ESCRITA;
    }
    if (fclose(arquivo) != 0) {
        return BESTIARIO_ERRO_ESCRITA;
    }
    return BESTIARIO_OK;
}
