---
trigger: always_on
---

# Personalização, Idioma e Estrutura de Diretórios

## 1. Convenções de Idioma e Nomenclatura
Todo o código deve ser escrito utilizando **Português do Brasil**.

* **Comentários:** Devem ser em português, claros e diretos.
* **Nomes de Arquivos:** `circulo.c`, `lista.h`, `arvore_segmentos.c`, etc.
* **Nomes de Funções:**
    * Use `snake_case` para funções.
    * Exemplos: `criar_circulo`, `obter_raio`, `inserir_lista`, `verificar_visibilidade`.
* **Nomes de Structs e Tipos:**
    * Podem usar `CamelCase` ou `snake_case`, mas mantenha a consistência.
    * Exemplo: `typedef void* Circulo;` (no .h) e `typedef struct circulo { ... } Circulo;` (no .c).
* **Variáveis:** Em português (`largura`, `altura`, `ponto_inicial`).

## 2. Estrutura de Pastas e Arquivos
A organização do projeto deve seguir estritamente a hierarquia abaixo. O `Makefile` deve estar localizado dentro da pasta `src` e deve ser capaz de compilar recursivamente ou encontrar os arquivos nas subpastas de `lib`.

### Árvore do Projeto
```text
/ (Raiz do Repositório)
|
+--- src/
|    |
|    +--- Makefile          <-- O script de compilação principal
|    +--- main.c            <-- Ponto de entrada (argumentos e fluxo principal)
|    |
|    +--- lib/              <-- Todos os módulos do projeto
|         |
|         +--- argumentos/  <-- Módulo para tratar argv/argc e flags (-e, -f, -o)
|         |    +--- argumentos.h
|         |    +--- argumentos.c
|         |
|         +--- formas/      <-- Módulos das formas geométricas básicas
|         |    +--- circulo/
|         |    |    +--- circulo.h
|         |    |    +--- circulo.c
|         |    +--- retangulo/
|         |    |    +--- retangulo.h
|         |    |    +--- retangulo.c
|         |    +--- linha/
|         |    |    +--- linha.h
|         |    |    +--- linha.c
|         |    +--- texto/
|         |         +--- texto.h
|         |         +--- texto.c
|         |
|         +--- estruturas/  <-- Estruturas de Dados Genéricas e Específicas
|         |    +--- lista/
|         |    |    +--- lista.h
|         |    |    +--- lista.c
|         |    +--- arvore/ (Árvore de Segmentos Ativos)
|         |         +--- arvore.h
|         |         +--- arvore.c
|         |
|         +--- geometria/   <-- Matemática e algoritmos geométricos
|         |    +--- ponto/
|         |    |    +--- ponto.h
|         |    |    +--- ponto.c
|         |    +--- segmento/ (Conceito de segmento para o algoritmo)
|         |    |    +--- segmento.h
|         |    |    +--- segmento.c
|         |    +--- poligono/ (Região de visibilidade)
|         |         +--- poligono.h
|         |         +--- poligono.c
|         |    +--- calculos/ (Determinantes, intersecções, etc)
|         |         +--- calculos.h
|         |         +--- calculos.c
|         |
|         +--- svg/         <-- Módulo de geração de saída gráfica
|              +--- svg.h
|              +--- svg.c