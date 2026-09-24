# Relatório da atividade — Aula 19 / TinyML

**Estado: preparação concluída; prática física e dados reais pendentes.**

Disciplina: Project-based Maker Lab. Professora: Dra. Gedeane G. S. Kenshima. Grupo: Start-up One.

| Integrante | RM |
|---|---|
| Enricco Rossi de Souza Carvalho Miranda | RM551717 |
| Gabriel Marquez Trevisan | RM99227 |
| Guilherme Silva dos Santos | RM551168 |
| Danilo Urze Aldred | RM99465 |
| Laura Claro Mathias | RM98747 |

## 1. Sensor e variável

Sensor escolhido: **ultrassônico HC-SR04**, conforme hardware documentado do CUBI-04. Confirmar identificação na placa física antes da prática.

- Fenômeno: propagação e reflexão de uma onda ultrassônica no obstáculo.
- Sinal bruto: duração de pulso digital ECHO, em microssegundos (`unsigned long`). A saída elétrica é digital; a grandeza estimada é distância, uma variável quantitativa contínua, representada por `float` em centímetros.
- Conversão: `distancia_cm = duracao_us × 0.0343 / 2`, usando velocidade aproximada do som de 343 m/s. Não há compensação de temperatura.
- Faixa nominal do modelo: **2 a 400 cm**, conforme [datasheet](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf). Não equivale ao mínimo/máximo observado em nossa bancada.
- Valores de referência propostos para o ensaio: alvo parado em torno de 30 cm; alvo variável em aproximadamente 5–100 cm. São posições planejadas, ainda não medidas.
- Faixa escolhida para normalização: 0–100 cm; acima de 100 cm, saturação em 1.0. Leituras fora da faixa nominal ou sem eco são descartadas do processamento.

## 2. Caracterização experimental

Preencher com os arquivos reais; não usar os dados sintéticos dos testes.

| Item | Resultado |
|---|---|
| Data, placa exata e versão do pacote ESP32 | PENDENTE |
| Alvo/material, posição e condições do ambiente | PENDENTE |
| Duração e quantidade de registros por coleta | PENDENTE |
| Mínimo observado válido, ensaio variável | PENDENTE |
| Máximo observado válido, ensaio variável | PENDENTE |
| Média, amplitude e desvio padrão com alvo parado | PENDENTE |
| Leituras sem eco e fora da faixa | PENDENTE |
| Estabilidade observada ao longo do tempo | PENDENTE |
| Ruído com alvo parado e possíveis fontes | PENDENTE |
| Comportamento antes/depois do filtro | PENDENTE |

Critério de análise: relatar os números e as condições do ensaio. Desvio padrão com alvo parado descreve dispersão, mas não determina por si só a exatidão do sensor. Não interpretar movimentos voluntários do alvo como ruído. Para comparar visualmente antes/depois, repetir posições e condições; os sketches não fazem uma comparação pareada simultânea.

## 3. Códigos e pré-processamento

- **Antes:** [leitura_bruta.ino](firmware/leitura_bruta/leitura_bruta.ino), com pulso, distância sem filtragem e status. Mantém valores fora de faixa para diagnóstico.
- **Depois:** [leitura_processada.ino](firmware/leitura_processada/leitura_processada.ino), junto de [processamento.h](firmware/leitura_processada/processamento.h).

Foram implementadas três operações da lista da aula: **média móvel**, **limitação de faixa** e **normalização 0–1**. A janela tem 5 leituras válidas consecutivas; falhas reiniciam a janela. O CSV contém a feature resultante, não a distância bruta.

## 4. Problema e classes

Tipo: **classificação binária**, com duas classes. Feature: média móvel da distância, limitada a 100 cm, dividida por 100 e arredondada para 6 casas.

- Classe **0 — perto:** `feature < 0.2`.
- Classe **1 — longe:** `feature >= 0.2`.

A divisão corresponde a aproximadamente 20 cm depois da filtragem e arredondamento. O limiar foi escolhido para tornar o ensaio reproduzível em bancada. Os rótulos são produzidos por regra explícita; não foi treinado um modelo.

## 5. Dataset e evidências

Arquivo final esperado: `dados/dataset.csv`, com exatamente as colunas `feature,label`. **Ainda não coletado.** Contagens por classe e mínimo/máximo da feature: **PENDENTES**.

Evidências esperadas: capturas do Serial Monitor, foto da montagem, registros originais e condições do ensaio. Usar o [roteiro de coleta](README.md) e o [registro](dados/REGISTRO_COLETA.md).

## 6. Conclusão provisória

A estrutura de aquisição, pré-processamento e rotulagem está preparada. Não é possível concluir sobre estabilidade, ruído, qualidade do dataset ou desempenho no ESP32 sem executar os ensaios e anexar medições reais. A entrega da atividade depende dessas etapas físicas.
