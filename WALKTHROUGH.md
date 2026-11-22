# Walkthrough - Sistema Auxiliar de Mestre de RPG

Passo a passo de uma sessão completa, do zero até o resultado do combate, e depois um resumo do Modo História.

## 0. Compilar e abrir

Com make: `mingw32-make` (ou `make` no Linux) e depois `.\personagens.exe 42`. Sem make:

```powershell
gcc -std=c11 -Wall -Wextra -pedantic main.c personagem.c inventario.c item.c monstro.c monstro_campos.c bestiario.c dados.c grupo.c encontro.c combate.c texto.c entrada.c menu_bestiario.c menu_encontro.c menu_combate.c persistencia.c tela.c catalogo.c cidade.c menu_cidade.c historia.c menu_historia.c som.c -o personagens -lwinmm
.\personagens.exe 42
```

O número depois do nome do programa é a semente aleatória (opcional). Com a mesma semente e as mesmas escolhas, os sorteios se repetem. Sem semente, ela vem do relógio e aparece na tela inicial.

Ao abrir, aparece a abertura animada (qualquer tecla pula) e o programa pergunta o modo:

- **1** - Sistema Auxiliar de Mestre (passos 1 a 7 abaixo).
- **2** - Modo História (última seção).

Cada tela limpa o console. As mensagens de resultado aparecem no topo depois de `>>` e as listagens esperam Enter.

O bestiário (50 monstros) é carregado automaticamente de `DadosBestiario.txt`.

## 1. Cadastrar personagens (opção 1)

Informe ID, nome, raça, classe, nível, atributos e PV. Cadastre de 1 a 6 personagens para formar o grupo. A opção 5 lista todos.

## 2. Criar e equipar itens

1. Opção **6 (Administrar inventário)** > **1 (Adicionar item)**: escolha o personagem e preencha o item.
   - Para armas, informe o **dano** no formato `NdL` ou `NdL+B` (ex.: `1d6` rola um dado de 6 lados; `1d8+1` rola um d8 e soma 1).
2. Opção **8 (Equipar item)**: informe o ID do item e o slot. Arma de uma mão vai em qualquer mão; arma de duas mãos ocupa as duas.
3. A opção **7** mostra os equipamentos e a **10** os atributos totais (base + itens).

Com uma arma de uma mão em cada mão, o personagem ataca com as duas no turno; a segunda tem -2 no bônus de acerto. Arma na mão não preferida pede confirmação com `1` ou `sim`.

## 3. Consultar o bestiário (opção 11)

Liste, busque e filtre monstros. Também dá para adicionar, alterar, remover e salvar o bestiário em arquivo. Alterar e remover ficam bloqueados durante um combate.

## 4. Montar o encontro (opção 12)

1. **1** - selecione os personagens do grupo.
2. **2** - confira o grupo e o poder total.
3. **3** - informe terreno, período e dificuldade (FÁCIL 60-80%, MÉDIA 90-110%, DIFÍCIL 120-150% do poder do grupo).
4. **4** - veja os monstros elegíveis (tabela com ID, nome, frequência, peso e poder).
5. **5** - gere três propostas. O sorteio é aleatório; se vierem menos de três, não é erro: gere de novo ou mude os filtros.
6. **6** - escolha a proposta; **7** mostra o encontro escolhido.

**Encontro manual (opção 8):** depois de definir grupo e contexto, informe os IDs elegíveis e as quantidades (até 4 espécies). O encontro vira a proposta escolhida, e aparece um aviso se o poder estiver fora da faixa.

## 5. Combate (opção 13)

1. **1** - prepara o combate com o encontro escolhido (rola os PV dos monstros e a iniciativa).
2. **8** (opcional) - configura **cura** e/ou **magia** de um personagem:
   - Cura: no turno, se algum aliado estiver com PV <= 50%, cura o mais ferido rolando o dado informado (limitado ao PV máximo).
   - Magia: ataque que ignora a defesa e causa o dano do dado informado.
3. **3** executa uma rodada; **4** executa até o fim (máximo de 100 rodadas).
4. **9** tenta fugir; **10** usa um item entre as rodadas.
5. Regras: acerto = d20 + bônus >= defesa; 1 natural sempre erra, 20 natural sempre acerta.
6. **Moral e fuga dos monstros:** um monstro faz teste de moral quando chega a 50% do PV ou quando um aliado cai. Rola d20; se o resultado for maior que a moral dele, ele foge e sai do combate (não conta como derrotado nem dá XP).
7. Habilidades especiais dos monstros não têm efeito automático: o resumo mostra cada uma como ALERTA para o Mestre aplicar.
8. No fim, o programa mostra o resumo e pergunta se aplica os PV restantes às fichas. Na vitória, as moedas entram junto. A opção **7** grava `relatorio-combate.txt`.

## 6. Moedas, XP e cidade (menu principal)

1. Opção **16**: dá ou tira moedas de um personagem.
2. Opção **17**: entra na cidade com ele. Poções na loja, no hospital ou na cabana; armas e armaduras no ferreiro; runas na loja.
3. Opção **8**: equipa a runa no slot **11**. Opção **6 > 6**: usa uma poção.
4. Opção **18**: concede XP manualmente a um personagem.

## 7. Salvar, carregar e encerrar

- Opções **14** e **15**: gravam e recarregam personagens, itens e equipamentos em `DadosPersonagens.txt` (ou outro nome). Se o arquivo tiver erro, o programa informa a linha e não altera nada.
- Opção **19**: liga ou desliga o som.
- Opção **0**: encerra. O que não foi salvo com a opção 14 se perde.

## Modo História

1. **2** cria um herói (nome, raça e classe). Dá para criar até 6; a opção **3** monta o grupo.
2. **4** visita a cidade; **5** explora (terreno e dificuldade) e luta rodada a rodada; **6** descansa; **7** usa um item.
3. **1** abre as missões: a linha principal (uma por vez, em ordem) e as histórias secundárias. Vencer a missão principal N libera o capítulo N na livraria.
4. Na cidade, **9** é a livraria, com a história de Falmuth e os estudos. A loja e o ferreiro também compram itens pela metade do preço.
5. **8** salva e **9** carrega o jogo (heróis, grupo e progresso).
6. **10** liga ou desliga o som.
7. **99** abre o painel SAM (fichas, equipamentos e XP manual).

## Sons

O jogo toca sons curtos na abertura, no fim do combate, ao subir de nível, ao comprar ou vender e ao concluir missões.
