CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic
FONTES = main.c personagem.c inventario.c item.c monstro.c monstro_campos.c bestiario.c dados.c grupo.c \
	encontro.c combate.c texto.c entrada.c menu_bestiario.c menu_encontro.c menu_combate.c persistencia.c \
	tela.c catalogo.c cidade.c menu_cidade.c historia.c menu_historia.c som.c
CABECALHOS = $(wildcard *.h)

ifeq ($(OS),Windows_NT)
EXT = .exe
LIBS = -lwinmm
EXECUTA = .\personagens.exe
else
EXT =
LIBS =
EXECUTA = ./personagens
endif

.PHONY: all executar testes

all: personagens$(EXT)

personagens$(EXT): $(FONTES) $(CABECALHOS)
	$(CC) $(CFLAGS) $(FONTES) -o personagens$(EXT) $(LIBS)

executar: personagens$(EXT)
	$(EXECUTA)

testes:
	powershell -NoProfile -ExecutionPolicy Bypass -File testes/executa_testes.ps1
