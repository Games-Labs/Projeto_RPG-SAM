# Sistema Auxiliar de Mestre de RPG - SAM

Projeto desenvolvido em **C (C11)** para a disciplina de Algoritmos 2.

## Equipe

- **Mathias Silva Cunha** — desenvolvimento individual.

## Sobre o projeto

O programa possui dois modos principais:

- **Sistema Auxiliar de Mestre:** reúne as ferramentas para cadastrar personagens, organizar equipamentos, consultar o bestiário, montar encontros e executar combates.
- **Modo História:** permite criar heróis, explorar o mundo de Falmuth, cumprir missões, visitar locais da cidade e acompanhar a progressão do grupo.

O projeto foi dividido em módulos para separar as regras do jogo, o armazenamento dos dados e os menus.

## Como compilar e executar

### Requisitos

- GCC com suporte a C11.
- No Windows, MinGW para usar o `mingw32-make`.
- No Linux, `make` e, para reprodução dos sons, `aplay` ou `paplay`.

### Compilação

Na pasta principal do projeto, execute:

```bash
mingw32-make
```

No Linux, use:

```bash
make
```

Também é possível compilar manualmente no Windows:

```powershell
gcc -std=c11 -Wall -Wextra -pedantic main.c personagem.c inventario.c item.c monstro.c monstro_campos.c bestiario.c dados.c grupo.c encontro.c combate.c texto.c entrada.c menu_bestiario.c menu_encontro.c menu_combate.c persistencia.c tela.c catalogo.c cidade.c menu_cidade.c historia.c menu_historia.c som.c -o personagens -lwinmm
```

### Execução

Windows (PowerShell):

```powershell
.\personagens.exe
```

Linux/macOS:

```bash
./personagens
```

O programa também aceita uma semente opcional para controlar a sequência pseudoaleatória. Isso é útil para repetir cenários durante os testes:

```powershell
.\personagens.exe 12345
```

O repositório do GitHub contém o executável `personagens.exe` já compilado. Ele foi mantido lá para facilitar a execução e evitar problemas com a entrega pelo Classroom.

## Organização dos arquivos

| Arquivo / módulo | Responsabilidade |
|---|---|
| `item.h/.c` | Tipos de item, validações e nomes dos tipos. |
| `inventario.h/.c` | Inserção, remoção, busca, listagem e controle dos espaços ocupados. |
| `personagem.h/.c` | Fichas, cadastro, equipamentos, operações CRUD e cálculo dos atributos totais. |
| `monstro.h/.c` | Estruturas e enums do bestiário, conversões, validações e consultas ecológicas. |
| `monstro_campos.c` | Leitura e gravação dos campos do monstro no formato chave=valor. |
| `bestiario.h/.c` | Armazenamento de até 100 monstros, leitura, gravação, CRUD e filtros. |
| `dados.h/.c` | Dados, conversões, validação, sementes e rolagens. |
| `grupo.h/.c` | Grupo de 1 a 6 aventureiros e cálculo de poder. |
| `encontro.h/.c` | Contexto, dificuldade, filtros, sorteio ponderado e geração de propostas. |
| `combate.h/.c` | Preparação e execução de combates, turnos, rodadas, dano, histórico e resultados. |
| `texto.h/.c` | Contagem de caracteres UTF-8 e alinhamento de colunas. |
| `entrada.h/.c` | Leitura e validação de entradas e configuração do console em UTF-8. |
| `menu_bestiario.h/.c` | Menu do bestiário. |
| `menu_encontro.h/.c` | Menu de encontros. |
| `menu_combate.h/.c` | Menu de combate. |
| `persistencia.h/.c` | Salvamento e carregamento de personagens e jogos. |
| `tela.h/.c` | Organização das telas e mensagens no console. |
| `catalogo.h/.c` | Leitura do catálogo de itens. |
| `cidade.h/.c` | Serviços e locais de Falmuth. |
| `menu_cidade.h/.c` | Menus da cidade. |
| `historia.h/.c` | Regras e progresso do Modo História. |
| `menu_historia.h/.c` | Menus do Modo História. |
| `som.h/.c` | Geração e reprodução dos efeitos sonoros. |
| `main.c` | Menu principal e inicialização do programa. |

### Arquivos de dados

- `DadosBestiario.txt`: bestiário inicial, com 50 monstros.
- `bestiario-saida.txt`: arquivo gerado ao salvar o bestiário.
- `DadosPersonagens.txt`: arquivo padrão para salvar personagens pelo menu principal.
- `DadosHistoria.txt`: textos de abertura, capítulos e missões.
- `DadosItens.txt`: catálogo de itens disponíveis nos estabelecimentos.
- `DadosHistoriaSalvo.txt`: arquivo padrão de salvamento do Modo História.

Há outros arquivos auxiliares no projeto que não aparecem na tabela por não serem módulos principais.

## Funcionalidades

### Personagens e inventário — TP-1

- Cadastro de até 20 personagens, com ID único.
- Criação, consulta, alteração e remoção de fichas.
- Inventário com limite de 50 itens e 50 espaços ocupados.
- Equipamentos para elmo, peitoral, manoplas, calça, botas, anel, colar, cinto e mãos.
- Verificação de compatibilidade entre item e espaço de equipamento.
- Armas de duas mãos ocupam as duas mãos e têm o bônus contado uma única vez.
- Itens equipados não ocupam espaço no inventário.
- Cálculo dos atributos totais a partir dos equipamentos.

### Bestiário — TP-2

- Cadastro de até 100 monstros.
- Importação e exportação em arquivo de texto, no formato chave=valor.
- Busca por ID e nome, alteração, remoção e listagem.
- Filtros por terreno e período de atividade.
- Validação dos campos durante a leitura e a edição.
- Registros inválidos são rejeitados sem interromper o carregamento dos demais. O sistema informa a linha, a chave e o motivo do primeiro erro.

O arquivo inicial possui 50 monstros válidos.

### Grupos e encontros — TP-3

- Grupos com 1 a 6 personagens, sem repetição.
- Cálculo do poder total do grupo a partir das fichas.
- Seleção de contexto por terreno, período e dificuldade.
- Filtragem de monstros compatíveis com o contexto.
- Sorteio ponderado de monstros.
- Geração de até três propostas distintas de encontro.
- Escolha de uma das propostas pelo Mestre.
- Possibilidade de montar um encontro manualmente pelo menu.

### Combate — TP-4

- Preparação do combate com personagens e exemplares de monstros.
- Rolagem de PV e iniciativa.
- Ordem de ação com critérios de desempate definidos.
- Verificação de acerto, dano e derrota.
- Ataques múltiplos de monstros, respeitando a ordem cadastrada na ficha.
- Ataques desarmados quando o personagem não possui arma.
- Histórico de ações, resumo do combate e relatório em arquivo.
- Atualização das fichas após o encerramento, mediante confirmação do Mestre.
- Limite de 100 rodadas para evitar combates sem fim.

O menu também permite consultar alertas de habilidades especiais. A execução automática dessas habilidades não foi implementada.

### Persistência e encontros manuais

- As opções 14 e 15 do menu principal salvam e carregam personagens, inventários e equipamentos em `DadosPersonagens.txt`.
- A leitura é feita de forma atômica: se houver erro, o programa informa a linha e mantém os dados atuais.
- O carregamento e as alterações que poderiam invalidar as fichas ficam bloqueados durante um combate em andamento.
- Na opção 8 de encontros, o Mestre escolhe os monstros e as quantidades. O programa calcula o poder e informa se o encontro está dentro da faixa da dificuldade escolhida.

### Cidade de Falmuth e Modo História

As funcionalidades abaixo foram adicionadas ao projeto e não fazem parte dos enunciados TP-1 a TP-4.

- Abertura com texto animado, carregado de `DadosHistoria.txt`. O jogador escolhe entre o Sistema Auxiliar de Mestre e o Modo História.
- Telas organizadas no console, com mensagens de resultado exibidas no menu seguinte.
- Itens do tipo runa e consumível, além dos equipamentos tradicionais.
- Consumíveis com efeitos de cura, bônus temporário, veneno ou efeito de suspeita.
- Moedas individuais para cada personagem.
- Catálogo de itens com estabelecimentos associados: loja, hospital, cabana e ferreiro.
- Compra, venda e descarte de itens.
- Locais da cidade com serviços e eventos: loja, hospital, academia, bar, praça, cabana escura, ferreiro e ferro-velho.
- Fuga do grupo durante o combate.
- Recompensas em moedas e experiência.
- Criação de heróis, formação de grupo, exploração, encontros, combate, descanso e painel do Mestre.
- Missões principais e histórias secundárias.
- Capítulos liberados conforme o avanço nas missões do Modo História.
- Livraria com a história de Falmuth e opções de estudo que podem melhorar os personagens.
- Salvamento do progresso, incluindo experiência, moedas, grupo, missões concluídas e bônus temporários.

### Progressão, missões e outras adições

- **Experiência e níveis:** ao vencer, a experiência dos monstros derrotados é dividida entre os personagens que permanecem de pé. A cada nível, o personagem recebe PV máximos, poder e um bônus de ataque ou defesa.
- **Mão preferida:** personagens podem ser destros ou canhotos. Usar uma arma de uma mão na mão errada aplica penalidade aos atributos de ataque e defesa.
- **Missões:** há quatro missões principais em sequência e três histórias secundárias. As missões principais liberam os capítulos correspondentes.
- **Livraria:** oferece a história de Falmuth e estudos que podem conceder bônus.
- **Painel do Mestre:** no Modo História, o acesso é feito pelo código `99`.
- **Salvamento do jogo:** o arquivo do Modo História registra fichas, itens, XP, mão preferida, bônus temporários, grupo e missões concluídas. Arquivos de versões anteriores continuam sendo aceitos.

### Sons

O módulo `som.h/.c` gera efeitos sonoros sem bibliotecas de terceiros. As melodias são montadas em memória como arquivos WAV.

- No Windows, a reprodução usa `PlaySound`, da biblioteca `winmm`.
- No Linux, o programa tenta usar `aplay` ou `paplay`.
- Se não houver um reprodutor disponível, usa a campainha do terminal.
- Os sons são reproduzidos em eventos como abertura, vitória, derrota, fuga, subida de nível, compra, venda e conclusão de missão.
- A reprodução pode ser ligada ou desligada pelo menu.
- Em testes com entrada redirecionada, os sons ficam desativados.

## Regras e decisões de implementação

Esta seção registra algumas escolhas feitas durante o desenvolvimento. Os valores que não eram definidos pelos enunciados foram adotados como regras do projeto.

### Limites e estrutura dos dados

- O cadastro suporta até 20 personagens (`CADASTRO_MAX_PERSONAGENS`).
- Cada inventário aceita até 50 itens e 50 espaços ocupados (`INVENTARIO_MAX_ITENS` e `INVENTARIO_MAX_ESPACOS`).
- Raça, classe, tipo de item, espaço de equipamento e códigos de retorno são representados por `enum`.
- O cadastro de personagens é passado por parâmetro; não há variável global para armazenar as fichas.
- Após uma remoção, os registros permanecem contíguos.
- Os atributos totais são calculados quando necessário, sem manter uma cópia permanente dos valores derivados.

### Equipamentos e atributos

- Itens equipados deixam de aparecer no inventário e não consomem seus espaços.
- Armas de duas mãos bloqueiam as duas mãos. Seu bônus é contado apenas uma vez.
- A arma principal é a da mão direita; se ela estiver vazia, é usada a da mão esquerda.
- Sem arma equipada, o personagem ataca com `1d2`.
- Antes de trocar um equipamento, o programa verifica se o item antigo pode voltar ao inventário. Assim, a operação não deixa o personagem em um estado incompleto.
- O PV inicial no combate considera o menor valor entre os PV atuais da ficha e os PV máximos totais com equipamentos. Se os PV máximos ficarem abaixo de 1, é usado o mínimo de 1.

### Bestiário e encontros

- Como o TP-2 não especificava faixas para alguns atributos, foram adotados estes limites: CA de 0 a 9, THAC0 de 9 a 20, inteligência de 0 a 15, moral de 6 a 20 e XP de 1 a 9999.
- O cadastro e a edição de monstros usam o mesmo formato chave=valor do arquivo. Na edição, é possível partir de um monstro existente e alterar apenas os campos necessários.
- O poder do grupo é recalculado antes de cada geração de encontro.
- A escolha de proposta recebe o número mostrado no menu (1 a 3) e armazena o índice correspondente (0 a 2).
- O sorteio de monstros respeita os pesos definidos no projeto. A geração não reduz a quantidade sorteada para forçar um encontro dentro da faixa de poder.

### Combate

- A iniciativa de personagens é `1d20 + iniciativa total`; a de monstros é `1d20`. Ela é sorteada uma única vez por combate.
- Em caso de empate, agem primeiro os personagens; depois, o menor ID e o menor número de exemplar definem a ordem.
- O acerto é calculado por `d20 + bônus >= defesa`. Um resultado natural 1 sempre erra e um 20 natural sempre acerta.
- Para monstros, o bônus de ataque é `20 - THAC0` e a defesa é `20 - CA`.
- Personagens com 0 PV entram como derrotados, não agem e não podem ser escolhidos como alvos.
- O combate termina quando um dos lados fica sem combatentes ativos ou quando alcança 100 rodadas.
- Ao final, o Mestre escolhe se aplica os PV restantes às fichas ou descarta o resultado. Essa decisão é feita uma única vez por combate.
- Enquanto um combate estiver em andamento, alterações e remoções de personagens ou monstros ficam bloqueadas para evitar que os dados usados pelo combate sejam invalidados.
- Moral e fuga não faziam parte da implementação original do TP-4. As regras adicionais de fuga foram definidas para o projeto.

### Itens, economia e progressão

- Armas exigem um dado de dano válido, como `1d6` ou `1d8+1`. Itens que não são armas devem ter dano zerado.
- Itens podem ter bônus inteiros, inclusive negativos.
- O bônus temporário dura até o fim do próximo combate e entra no cálculo dos atributos totais sem alterar os valores-base da ficha.
- O veneno usado fora de combate não reduz os PV abaixo de 1.
- A recompensa em moedas por vitória é calculada a partir do XP dos monstros derrotados.
- O nível seguinte exige `100 × nível atual` pontos de experiência. Ao subir de nível, o personagem recebe +4 PV máximos, +1 poder e +1 em ataque (nível par) ou defesa (nível ímpar), respeitando os limites da ficha.
- Usar uma arma de uma mão na mão não preferida reduz ataque e defesa em 20%, com arredondamento desfavorável ao personagem e mínimo de 1. O programa pede confirmação antes de equipá-la.

## Validações e cuidados

Algumas correções foram feitas durante a revisão do projeto:

- A leitura de inteiros usa `strtol` e verifica a linha inteira. Entradas como `12abc`, vazias ou fora do limite são recusadas.
- Nomes vazios, compostos apenas por espaços ou maiores que o limite não são aceitos.
- Raça, classe, tipo de item, espaço, nível, PV, ataque, defesa, iniciativa, poder, espaços e dano são validados durante a digitação.
- A edição de monstros recusa chaves ou valores inválidos sem alterar o registro.
- IDs de itens não podem se repetir entre a bolsa e os equipamentos.
- Nomes de personagens com `;` são gravados com vírgula para não quebrar o formato do arquivo.

## Testes e entregas acadêmicas

### TP-1 — Personagens e inventário

- [x] Estrutura de personagem com os campos obrigatórios.
- [x] Cadastro em vetor estático, com quantidade e limite de 20.
- [x] Validação dos campos da ficha.
- [x] CRUD com ID único e remoção contígua.
- [x] Estrutura de item e inventário com limite de 50 espaços.
- [x] Inserção, busca, remoção, listagem e cálculo de ocupação.
- [x] Equipamentos fixos e compatibilidade entre item e espaço.
- [x] Regra de arma de duas mãos.
- [x] Verificação de espaço antes de desequipar.
- [x] Cálculo dos atributos totais com base nos itens equipados.
- [x] Interface pública em `personagem.h` e implementação em `personagem.c`.
- [x] Menus e leitura com `fgets`, sem uso de `gets`.

### TP-2 — Bestiário

- [x] Enums e conversões entre texto e enum.
- [x] Estruturas de dados de monstros, ataques, movimentos e habilidades.
- [x] Conversão, validação e rolagem de dados.
- [x] Bestiário com até 100 monstros.
- [x] Leitura de arquivo com tratamento de comentários, linhas vazias e CRLF.
- [x] Rejeição de registros inválidos com diagnóstico.
- [x] Salvamento e recarregamento do bestiário.
- [x] CRUD e consultas por terreno e período.
- [x] Testes de busca, remoção, filtros, dados inválidos, arquivo inexistente e salvamento.

### TP-3 — Grupos e encontros

- [x] Grupo de 1 a 6 personagens, sem repetição.
- [x] Cálculo do poder do grupo.
- [x] Semente única e semente fixa para testes.
- [x] Cálculo das faixas de dificuldade.
- [x] Compatibilidade e filtragem de monstros.
- [x] Sorteio ponderado.
- [x] Cálculo do poder do encontro.
- [x] Geração de propostas distintas e escolha pelo Mestre.
- [x] Testes de faixas, filtros, sorteio, propostas repetidas e escolhas válidas e inválidas.

### TP-4 — Combate

- [x] Dano de armas e ataque desarmado.
- [x] Preparação do combate e criação dos exemplares de monstros.
- [x] Rolagem de PV e iniciativa.
- [x] Ordem de combate com desempate determinístico.
- [x] Regras de acerto, dano e derrota.
- [x] Seleção de alvos ativos.
- [x] Ataques de personagens e ataques múltiplos de monstros.
- [x] Execução de turnos, rodadas e encerramento.
- [x] Histórico, alertas e resumo do combate.
- [x] Atualização das fichas após o encerramento.
- [x] Testes dos casos previstos no enunciado, com as limitações registradas abaixo.
- [x] Extensões: aplicação ou descarte dos efeitos nas fichas e geração de relatório em arquivo.

## Pontos que ainda precisam de atenção

Algumas partes ainda têm limitações conhecidas:

- **Habilidades especiais:** aparecem como alertas no resumo, mas não são executadas automaticamente.
- **Geração de encontros:** como a quantidade sorteada não é truncada, algumas combinações são rejeitadas por ultrapassar a faixa de poder. O exemplo de cinco Goblins com poder 36 é validado no cálculo, mas nem sempre é gerado pelo sorteio. Grupos com poder muito alto também podem não receber propostas, devido aos limites de espécies e monstros.
- **Windows e UTF-8:** o arquivo do bestiário usa UTF-8 e CRLF. Em consoles sem UTF-8, alguns acentos podem aparecer incorretamente.
- **Formato dos dados:** o enunciado menciona `bestiario.txt`, mas o projeto utiliza `DadosBestiario.txt`.
- **Faixas do TP-2:** os limites de CA, THAC0, inteligência, moral e XP foram definidos pelo projeto porque o enunciado não os especificava.
- **Poder do grupo no TP-3:** o campo `poderTotal` foi mantido porque o enunciado exige sua existência. Ele é recalculado antes da geração de encontros.
- **XP no TP-4:** o resumo apresenta a soma do XP dos monstros derrotados. A divisão entre os personagens é feita nas funcionalidades adicionais de progressão.
- **Padrão de nomes:** o código mistura dois estilos de nome de função. Alguns módulos usam `modulo_acao` (ex.: `cadastro_buscar_por_id`, `historia_conclui_principal`) e outros usam camelCase (ex.: `carregaBestiario`, `preparaCombate`, `inicializaEstadoSalvo`). Foi uma falha minha de padronização ao longo dos TPs; o comportamento não muda, mas a ideia é unificar numa próxima versão.

## Histórico de alterações importantes

Durante a revisão e a implementação das extensões, foram corrigidos problemas que podiam causar perda de dados ou resultados errados:

- A leitura de inteiros passou a rejeitar caracteres extras depois do número.
- Entradas longas ou vazias passaram a ser recusadas, em vez de serem truncadas.
- A validação passou a acontecer durante a digitação, com nova solicitação quando necessário.
- A troca de equipamentos passou a verificar o espaço disponível antes de alterar a ficha.
- A cura em combate passou a respeitar o PV máximo-base ao atualizar o arquivo.
- O salvamento passou a tratar o caractere `;` nos nomes.
- O resumo de combate é descartado quando alterações posteriores poderiam fazer suas referências apontarem para fichas diferentes.

---

**Observação:** este README descreve a versão atual do projeto. Para conhecer o fluxo de utilização pelos menus, consulte também o arquivo `WALKTHROUGH.md`.
