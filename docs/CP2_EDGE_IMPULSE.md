# CP2 — Registro do projeto Edge Impulse

Esta página é o relatório rastreável do treinamento. Os campos entre colchetes
devem ser preenchidos com os valores reais do projeto da equipe; não usar números do
modelo sintético como se fossem resultados do Edge Impulse.

## Identificação

| Campo | Valor |
|---|---|
| Project ID / nome | [PREENCHER] |
| Link compartilhável | [PREENCHER] |
| Dataset importado | [PREENCHER: nome do CSV e hash opcional] |
| Data de importação | [PREENCHER] |
| Total de amostras | [PREENCHER] |
| Amostras por classe | [PREENCHER] |
| Responsável | [PREENCHER] |

## Configuração usada

Preencher depois de salvar o Impulse. A tabela é um roteiro, não uma alegação de que
esses valores já foram executados.

| Item | Valor real do projeto |
|---|---|
| Entradas | `esquerda_cm`, `centro_cm`, `direita_cm` |
| Labels | `LIVRE`, `OBSTACULO_ESQUERDA`, `OBSTACULO_CENTRO`, `OBSTACULO_DIREITA` |
| Divisão treino/teste | [PREENCHER] |
| Método de pré-processamento | [PREENCHER: exemplo, raw/features] |
| Janela e frequência, se aplicável | [PREENCHER] |
| Bloco de aprendizado | [PREENCHER] |
| Épocas | [PREENCHER] |
| Learning rate | [PREENCHER] |
| Tamanho do modelo/latência | [PREENCHER] |

## Evidências que devem ser salvas

Adicionar os arquivos em [`../experimentos/cp2-tinyml/evidencias/`](../experimentos/cp2-tinyml/evidencias/)
com nomes estáveis, sem depender apenas de links que podem expirar:

1. `01-dataset-importado.png` — amostras e classes;
2. `02-divisao-dataset.png` — treino/teste;
3. `03-configuracao-impulse.png` — processamento e blocos;
4. `04-treinamento.png` — parâmetros e resultado;
5. `05-matriz-confusao.png` — avaliação por classe;
6. `06-inferencia-amostras.png` — testes com exemplos reais;
7. `07-export-arduino.png` — confirmação da biblioteca ESP32/Arduino exportada.

## Resultado do treinamento

| Métrica | Treino | Teste |
|---|---:|---:|
| Acurácia | [PREENCHER] | [PREENCHER] |
| Precisão macro | [PREENCHER] | [PREENCHER] |
| Recall macro | [PREENCHER] | [PREENCHER] |
| F1 macro | [PREENCHER] | [PREENCHER] |
| Latência | [PREENCHER] | [PREENCHER] |

### Matriz de confusão

Colar a matriz real ou transcrevê-la mantendo a ordem dos labels:

```text
                         previsto
                    LIVRE ESQ CENTRO DIR
real LIVRE             [ ]  [ ]  [ ]   [ ]
     ESQ               [ ]  [ ]  [ ]   [ ]
     CENTRO            [ ]  [ ]  [ ]   [ ]
     DIR               [ ]  [ ]  [ ]   [ ]
```

## Teste de inferência

| Cenário físico | Entrada (esq, centro, dir) | Classe prevista | Confiança | Ação observada |
|---|---|---|---:|---|
| frente livre | [PREENCHER] | [PREENCHER] | [PREENCHER] | [PREENCHER] |
| obstáculo à esquerda | [PREENCHER] | [PREENCHER] | [PREENCHER] | [PREENCHER] |
| obstáculo no centro | [PREENCHER] | [PREENCHER] | [PREENCHER] | [PREENCHER] |
| obstáculo à direita | [PREENCHER] | [PREENCHER] | [PREENCHER] | [PREENCHER] |

## Auditoria do material V3 recebido em 14/10/2026

O pacote extraído em [`../data/CUBI04_Autonomo_V3/`](../data/CUBI04_Autonomo_V3/)
contém `model_auto_v3.h` e um firmware com inferência MLP local. O arquivo
`LEIA_PRIMEIRO_V3.txt` informa que o dataset é sintético. Não foram encontrados
nesse pacote o projeto/ID do Edge Impulse, prints do painel ou uma biblioteca
Arduino exportada com o classificador Edge Impulse. Portanto, os campos acima
continuam aguardando as evidências reais da equipe.

## Exportação para ESP32

Após o treinamento:

1. exportar o formato Arduino library/ESP32 no Edge Impulse;
2. colocar o ZIP original e/ou a pasta extraída em `model/edge-impulse/`;
3. registrar o nome exato do header principal e a versão do export;
4. integrar o classificador no firmware final;
5. compilar com a mesma placa/core usados no teste;
6. salvar a saída do Serial Monitor e o vídeo da ação dependente da classe.

| Campo | Valor |
|---|---|
| Arquivo/ZIP exportado | [PREENCHER] |
| Header principal | [PREENCHER] |
| Versão do export | [PREENCHER] |
| Core ESP32 | [PREENCHER] |
| Placa selecionada | [PREENCHER] |
| Data do upload | [PREENCHER] |

Até que essa seção seja preenchida, `model/model.h` é somente o modelo local de
referência e não deve ser chamado de biblioteca Edge Impulse.
