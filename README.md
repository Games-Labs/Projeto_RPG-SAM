Sistema Auxiliar de Mestre de RPG - Algoritmos 2

Identificacao da equipe
Mathias Silva Cunha (Unico)

Erros encontrados:
- Reportado; falta de validação na entrada de itens (Lu)
- Checar se o "Inventario" (se pode se chamar disso) Funciona
- Reportado; Adicionar integração ABNT pois o "Bestiario" tem letras com acentos incompartiveis em C. (Lu)

Estrutura do projeto
- item.h / item.c: definem o tipo Item, TipoItem, validacao e nomes textuais dos tipos.
- inventario.h / inventario.c: controlam o vetor de itens, ocupacao por espacos, busca, insercao, remocao e listagem.
- personagem.h / personagem.c: definem fichas, cadastro, equipamentos, CRUD, equipar, desequipar e calculo de atributos totais.
- monstro.h / monstro.c: enums do bestiario (com conversao texto<->enum), Dado, Monstro, validacao, exibicao e consultas ecologicas.
- monstro_campos.c: tabela chave=valor de cada campo do Monstro (interpretacao e gravacao), usada pelo carregamento, salvamento e edicao.
- bestiario.h / bestiario.c: Bestiario (ate 100 monstros), carga/gravacao de arquivo, mini-CRUD e filtros.
- dados.h / dados.c: semente, sorteio de inteiros e rolagem de Dado.
- grupo.h / grupo.c: grupo de 1 a 6 aventureiros e poder do grupo.
- encontro.h / encontro.c: contexto, faixas de dificuldade, filtro, poderes, sorteio ponderado, geracao e escolha de propostas.
- entrada.h / entrada.c: leitura segura de inteiros e textos.
- menu_bestiario.h / .c e menu_encontro.h / .c: menus do TP-2 e do TP-3 (clientes dos modulos).
- main.c: menu principal (opcoes 1 a 10 do TP-1, 11 Bestiario, 12 Encontros).
- DadosBestiario.txt: bestiario oficial (50 monstros). bestiario-saida.txt: arquivo produzido pela operacao de salvar.

Comando de compilacao
gcc -std=c11 -Wall -Wextra -pedantic main.c personagem.c inventario.c item.c monstro.c monstro_campos.c bestiario.c dados.c grupo.c encontro.c entrada.c menu_bestiario.c menu_encontro.c -o personagens_temp; .\personagens_temp.exe [semente]

O argumento opcional [semente] fixa a sequencia pseudoaleatoria (testes reproduziveis).

O exe pre-compilado personagens.exe esta somente na versao do github para evitar problemas na entrega do classroom!

Execucao
No Windows/PowerShell:
.\personagens_temp.exe

No Linux/macOS:
./personagens_temp

Decisoes relevantes
- O cadastro suporta ate 20 personagens por meio da constante CADASTRO_MAX_PERSONAGENS.
- Cada inventario aceita ate 50 itens e ate 50 espacos consumidos, usando INVENTARIO_MAX_ITENS e INVENTARIO_MAX_ESPACOS.
- Raca, classe, tipo de item, slot de equipamento e codigos de retorno usam enum.
- O cadastro e sempre passado por parametro; nao ha variavel global para armazenar personagens.
- Os registros de personagens e itens permanecem contiguos apos remocoes.
- Itens equipados deixam de aparecer no inventario e nao consomem seus espacos enquanto equipados.
- Armas de duas maos ocupam e bloqueiam as duas maos. O bonus de uma arma de duas maos e contado uma unica vez.
- As trocas de equipamento verificam antes se o item antigo pode voltar ao inventario, preservando a atomicidade.
- Os atributos totais sao calculados sob demanda e nao ficam armazenados como copia permanente.
- TP-2: o carregamento nunca encerra o programa. Registros invalidos sao rejeitados e contados (Bestiario.rejeitados); o primeiro erro fica em Bestiario.primeiroErro com linha, chave e motivo. Os validos continuam carregados.
- TP-2: faixas adotadas (o enunciado nao as define; baseadas nos extremos dos dados): CA 0..9, THAC0 9..20, inteligencia 0..15, moral 6..20, XP 1..9999. Sao constantes em monstro.h.
- TP-2: cadastrar e alterar monstro no menu usam linhas chave=valor (o mesmo formato do arquivo). Ao cadastrar e possivel copiar um monstro modelo e informar so as chaves que mudam.
- TP-3: poderTotal do GrupoAventura e recalculado a partir das fichas por calculaPoderGrupo (chamada antes de cada geracao); removePersonagemGrupo nao o recalcula por nao receber o cadastro.
- TP-3: escolheProposta recebe o numero exibido (1 a 3) e guarda indiceEscolhido = numero - 1 (inicial -1).



Checklist do TP-1 (Principais pedidos.)
- [x] Struct de personagem com todos os campos obrigatorios.
- [x] Cadastro com vetor estatico, quantidade e capacidade maxima 20.
- [x] Validacao de ID positivo, nome, raca, classe, nivel, PV, ataque, defesa, iniciativa e poder.
- [x] CRUD de personagens com ID unico e remocao contigua.
- [x] Struct de item com ID, nome, tipo, espacos, bonus e poder.
- [x] Inventario com capacidade de 50 espacos calculada por funcao.
- [x] Adicao, busca, remocao, listagem e ocupacao do inventario.
- [x] Equipamentos fixos: elmo, peitoral, manoplas, calca, botas, anel, colar, cinto e duas maos.
- [x] Compatibilidade entre tipo de item e slot de equipamento.
- [x] Regra de arma de duas maos bloqueando as duas maos.
- [x] Desequipar recusado quando nao ha espaco suficiente no inventario.
- [x] Calculo de ataque, defesa, iniciativa, PV maximos e poder totais usando apenas itens equipados.
- [x] Interface publica em personagem.h e implementacao em personagem.c.
- [x] Separacao adicional em item.h/.c e inventario.h/.c.
- [x] Menu textual com todas as operacoes minimas exigidas.
- [x] Leitura textual com fgets, sem uso de gets.

Checklist do TP-2 (Principais pedidos.)
Fase A - Desenvolvimento
- [x] monstro.h/.c: enums (Frequencia, Clima, Terreno, Atividade, Dieta, TipoMovimento, Manobrabilidade, Tamanho, TipoEspecial, EixoOrdem, EixoMoral) com conversao texto<->enum e validacao (sec. 3, 4, 7.1).
- [x] Structs Dado, Movimento, Ataque, HabilidadeEspecial, Tendencia, PoderEncontro e Monstro (sec. 3 e 4).
- [x] converteTextoDado, dadoParaTexto, validaDado (4d10, 1d8-1, 2d4+2) (sec. 7.1).
- [x] bestiario.h/.c: Bestiario com ate 100 monstros contiguos (sec. 5).
- [x] carregaBestiario com fgets, ignorando #, linhas vazias e CR do CRLF; erro com linha/chave/motivo; sem exit (sec. 6).
- [x] salvaBestiario recarregavel sem perda (sec. 7.2).
- [x] validaMonstro com todas as validacoes da sec. 8.
- [x] CRUD: insere, buscaPorId, consultaPorId, buscaPorNome, altera, remove, lista, exibe (sec. 7.3).
- [x] Consultas: viveNoTerreno, ativoNoPeriodo, listaMonstrosPorTerreno (sec. 7.4).
- [x] main.c com menu 1-9 e 0 (sec. 9).
Fase B - Testes
- [x] Carregar DadosBestiario.txt: 50 registros validos.
- [x] Goblin por ID e por nome; ID inexistente; ID repetido; remover do meio; FLORESTA+NOTURNO.
- [x] resistenciaMagia=101 recusada; arquivo inexistente; bloco sem [/MONSTRO]; salvar e recarregar.
Fase C - Validacao
- [x] Compilacao sem warnings; bestiario-saida.txt gerado; README.txt/README.md atualizado.

Checklist do TP-3 (Principais pedidos.)
Evidencias: ver "Resultado dos testes" no fim deste arquivo.
Fase A - Desenvolvimento
- [x] grupo.h/.c: GrupoAventura (1 a 6, sem repeticao), inicializa, adiciona, remove, calculaPoderGrupo (recalculado), exibe (sec. 3.1, 14).
- [x] dados.h/.c: rolaDado, semente unica e semente fixa para testes (sec. 7, passo 4).
- [x] encontro.h/.c: Dificuldade, Periodo, ContextoEncontro, calculaFaixaDificuldade com aritmetica inteira (sec. 4).
- [x] monstroCompativel e filtraCandidatos; erro especifico sem candidatos (sec. 5).
- [x] sorteiaMonstroPonderado com pesos 1/2/4/8/12 (sec. 6).
- [x] poderIndividualContextual, poderComponente, calculaPoderEncontro; 5 Goblins em floresta a noite = 36 (sec. 8).
- [x] tentaGerarEncontro (ate 4 especies, sem especie repetida, rejeicao sem truncar) e geraPropostas (ate 500 tentativas, 3 propostas distintas, diagnostico) (sec. 10, 11).
- [x] encontrosIguais (independe da ordem), escolheProposta (indice inicial -1), exibeEncontro (sec. 9, 14).
- [x] main.c com menu 1-7 e 0 (sec. 15).
Fase B - Testes
- [x] Poder 40: faixas 24-32, 36-44, 48-60; filtro sem candidatos; sorteio com semente fixa; proposta repetida; so duas solucoes; escolha valida e invalida (sec. 17).
Fase C - Validacao
- [x] Compilacao sem warnings com todos os modulos; fluxo ponta a ponta; README atualizado.

Checklist do TP-4 (Principais pedidos.)
Em breve

Divergencias, ambiguidades e limitacoes registradas (auditoria)
- Dados: o enunciado cita bestiario.txt; o arquivo do projeto e DadosBestiario.txt (constante ARQUIVO_BESTIARIO em menu_bestiario.h). O arquivo usa CRLF e UTF-8; o carregador remove o CR. O codigo PASSO_ARBOREO esta no arquivo com acento (PASSO_ARBÓREO) e foi preservado exatamente; em consoles Windows sem UTF-8 (chcp 65001) os acentos das descricoes aparecem trocados.
- Enunciado TP-2: diz "faixas adotadas no enunciado" para CA, THAC0, inteligencia, moral e XP, mas nao as define. Foram adotadas as faixas listadas em "Decisoes relevantes".
- Enunciado TP-2: listaMonstrosPorTerreno devolve Monstro *resultado[] a partir de const Bestiario *. Para nao descartar const, o vetor de saida e const Monstro *. Foi adicionada listaMonstrosPorPeriodo para o filtro por atividade do menu.
- Enunciado TP-2: Bestiario recebeu os campos extras rejeitados e primeiroErro (diagnostico sem I/O no modulo). Monstro.poder foi detalhado em PoderEncontro (com modTerreno por terreno).
- Enunciado TP-2: o sufixo de manobrabilidade (VOO:12:A) e opcional; ataquesEspeciais aceita NENHUM e defesasEspeciais aceita NENHUMA (ambos tratados como lista vazia nas duas chaves). Na gravacao: NENHUM para ataques e NENHUMA para defesas. Altura e gravada com %g (1.0 vira 1), equivalente na releitura.
- Enunciado TP-3: GrupoAventura exige o campo poderTotal armazenado, o que contraria a regra do CLAUDE.md de nao guardar valores derivados. Mantido o campo por exigencia do enunciado, sempre recalculado.
- Enunciado TP-3: os enums Dificuldade e Periodo comecam em 1 (convencao do projeto) em vez de 0.
- Enunciado TP-3: a quantidade rolada de numeroEncontrado (ex.: 4d10 Goblins) frequentemente excede a faixa de poder e a tentativa e rejeitada, como manda o enunciado (sem truncar). Por isso os exemplos de referencia (5 Goblins, poder 36) nao sao gerados na pratica; ja o calculo e verificado (5 Goblins em floresta a noite = 36). Para grupos de poder muito alto (ex.: 1000) o limite de 4 especies e 50 monstros impede propostas e o sistema informa zero propostas.
- personagens.exe e a versao TP-1 e nao foi recompilado.0
