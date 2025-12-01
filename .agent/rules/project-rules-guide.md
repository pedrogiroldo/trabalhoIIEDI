---
trigger: model_decision
description: Use these rules to understand the project requirements when implementing any functionality.
---

# Guia de Instruções do Projeto - Estrutura de Dados I

## Contexto do Projeto

Este projeto simula um cenário onde alienígenas invadem a Terra lançando três tipos de bombas (destruição, pintura e clonagem). O objetivo é implementar um sistema que processa formas geométricas e determina regiões afetadas por explosões, considerando anteparos que bloqueiam propagações.

## Estrutura de Entrada e Saída

### Arquivos de Entrada

**Arquivo .geo**: Define formas geométricas no plano cartesiano
- Cada forma possui um identificador inteiro único (i, j, k ≥ 1)
- Coordenadas são valores reais (x, y)
- Sistema de coordenadas invertido: origem (0,0) no canto superior esquerdo

**Arquivo .qry**: Contém comandos a serem executados sobre as formas
- Transforma formas em anteparos
- Lança bombas de diferentes tipos
- Clona e modifica formas existentes

### Arquivos de Saída

**Arquivo .txt**: Relatório textual com informações solicitadas pelos comandos
**Arquivos .svg**: Representações visuais das formas antes e depois do processamento

## Formas Geométricas

### Círculo (comando `c`)
- **Parâmetros**: `c i x y r corb corp`
- **Âncora**: Centro do círculo (x, y)
- **Dimensão**: Raio r
- **Cores**: corb (borda), corp (preenchimento)

### Retângulo (comando `r`)
- **Parâmetros**: `r i x y w h corb corp`
- **Âncora**: Canto inferior esquerdo (x, y)
- **Dimensões**: Largura w, altura h
- **Cores**: corb (borda), corp (preenchimento)

### Linha (comando `l`)
- **Parâmetros**: `l i x1 y1 x2 y2 cor`
- **Âncoras**: Duas extremidades (x1, y1) e (x2, y2)
- **Cor**: Cor única da linha

### Texto (comando `t`)
- **Parâmetros**: `t i x y corb corp a txto`
- **Âncora**: Definida pelo parâmetro `a`
  - `i`: início do texto
  - `m`: meio do texto
  - `f`: fim do texto
- **Texto**: Último parâmetro, pode conter espaços

### Estilo de Texto (comando `ts`)
- **Parâmetros**: `ts fFamily fWeight fSize`
- **Font Family**: sans (sans-serif), serif, cursive
- **Font Weight**: n (normal), b (bold), b+ (bolder), l (lighter)

## Comandos do Arquivo .qry

### Transformação em Anteparos (comando `a`)
- **Parâmetros**: `a i j [v|h]`
- **Função**: Transforma formas no intervalo [i, j] em anteparos bloqueantes
- **Conversões**:
  - **Círculo**: Vira segmento horizontal (h) ou vertical (v) passando pelo centro
  - **Retângulo**: Vira 4 segmentos (seus lados)
  - **Linha**: Permanece linha, mas vira anteparo
  - **Texto**: Vira segmento horizontal conforme fórmula de conversão
- **Saída TXT**: Reportar ID, tipo da figura original, ID e extremos dos segmentos

### Conversão de Texto para Segmento
Dado texto com âncora (xt, yt) e |t| caracteres:
- **Âncora 'i'**: x1 = xt, y1 = yt, x2 = xt + 10.0 × |t|, y2 = yt
- **Âncora 'f'**: x1 = xt - 10.0 × |t|, y1 = yt, x2 = xt, y2 = yt
- **Âncora 'm'**: x1 = xt - 10.0 × |t| / 2, y1 = yt, x2 = xt + 10.0 × |t| / 2, y2 = yt

### Bomba de Destruição (comando `d`)
- **Parâmetros**: `d x y sfx`
- **Função**: Lançada em (x, y), destrói formas na região de visibilidade
- **Saída TXT**: ID e tipo das formas destruídas
- **Saída SVG**: Desenhar região de visibilidade
  - Se sfx = "-": desenha no arquivo SVG final
  - Caso contrário: cria arquivo separado com sufixo sfx

### Bomba de Pintura (comando `p`)
- **Parâmetros**: `p x y cor sfx`
- **Função**: Pinta formas na região de visibilidade com a cor especificada
- **Efeito**: Altera cores de borda e preenchimento
- **Saída TXT**: ID e tipo das formas pintadas
- **Saída SVG**: Similar ao comando d

### Bomba de Clonagem (comando `cln`)
- **Parâmetros**: `cln x y dx dy sfx`
- **Função**: Clona formas na região de visibilidade
- **Translação**: Clones deslocados por (dx, dy)
- **Identificadores**: Clones recebem IDs únicos
- **Saída TXT**: ID e tipo das figuras originais e dos clones
- **Saída SVG**: Similar ao comando d

## Região de Visibilidade

A região de visibilidade é um polígono representado por sequência ordenada de vértices ou segmentos. Anteparos bloqueiam a propagação das explosões.

### Detecção de Sobreposição

**Otimização inicial**: Testar sobreposição de bounding boxes primeiro

#### Para Segmentos (Linhas):
1. Qualquer extremidade do segmento está dentro do polígono
2. Segmento intersecta qualquer aresta do polígono

#### Para Retângulos:
1. Qualquer vértice do retângulo está dentro do polígono
2. Qualquer vértice do polígono está dentro do retângulo
3. Qualquer aresta do retângulo intersecta qualquer aresta do polígono

#### Para Círculos:
1. Centro do círculo está dentro do polígono
2. Existe alguma aresta do polígono cuja distância ao centro ≤ r
3. Algum vértice do polígono está dentro do círculo (distância ≤ r)

## Requisitos de Implementação

### Estruturas de Dados
- **TADs obrigatórios**: Implementar conforme especificações da Sala de Aula
- **Árvore**: Para armazenar segmentos ativos
- **Listas**: Para armazenar as formas geométricas
- **Proibido**: Definir structs em arquivos de cabeçalho (.h)

### Modularização
- Cada estrutura de dados em módulo separado (.h e .c)
- Arquivo .h bem documentado (contrato da interface)
- Testes unitários para cada módulo (arquivo .c separado)

### Algoritmos de Ordenação
- Usar **qsort** da biblioteca padrão do C
- Implementar versão modificada do **mergesort**
- Quando subvetor for "pequeno" (parâmetro -i), usar **insertionsort**
- Algoritmo de visibilidade utiliza ordenação

### Compilação
- Opção obrigatória: `-fstack-protector-all`
- Padrão C99: `-std=c99`
- Usar makefile provido

## Parâmetros do Programa

### Obrigatórios
- **-f a.geo**: Arquivo com descrição das formas (sob diretório BED)
- **-o ah**: Diretório-base de saída (BSD)

### Opcionais
- **-e ah**: Diretório-base de entrada (BED)
- **-q ac.qry**: Arquivo com consultas (sob diretório BED)
- **-to [q|m]**: Tipo de ordenação (q: qsort, m: mergesort). Default: q
- **-i n**: Tamanho mínimo para insertionsort. Default: 10

## Nomenclatura de Arquivos de Saída

### Apenas .geo fornecido:
- `arq.svg`

### Com .geo e .qry:
- `arq.svg`
- `arq-arqcons.svg`
- `arq-arqcons.txt`

### Com sufixo em comandos:
- `arq.svg`
- `arq-arqcons.svg`
- `arq-arqcons.txt`
- `arq-arqcons-sufx.svg` e/ou `arq-arqcons-sufx.txt`

## Critérios de Penalização

- **-2.5**: Definir struct em arquivo .h
- **até -2.0**: Modularização pobre, .h mal documentado
- **até -1.0**: Procedimentos extensos/complicados
- **até -2.0**: Estrutura pouco eficiente
- **até -3.0**: Poucos commits ou concentrados no final
- **até -1.5**: Implementação ineficiente/desidiosa
- **Nota ZERO**: Erro de compilação ou FRAUDE detectada

## Entrega

Submeter arquivo .txt com formato:
```
apelido url-do-repositorio
```

Exemplo:
```
joseMane https://github.com/zemane/t2.git
```

O repositório deve estar organizado conforme descrição geral do projeto.