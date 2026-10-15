# Check Point 02 — TinyML aplicado ao Carrinho-Robô

> CUBI-04 · Project-based Maker Lab · grupo Start-up One

Esta é a página principal da entrega do CP2. Ela concentra a rastreabilidade entre
**sensor → coleta → CSV → Edge Impulse → treinamento → inferência → ESP32 → ação**.

## Equipe

| Integrante | RM |
|---|---|
| Enricco Rossi de Souza Carvalho Miranda | RM551717 |
| Gabriel Marquez Trevisan | RM99227 |
| Guilherme Silva dos Santos | RM551168 |
| Danilo Urze Aldred | RM99465 |
| Laura Claro Mathias | RM98747 |

## O que já está preparado no repositório

- firmware de coleta para os três sensores frontais: [`experimentos/cp2-tinyml/firmware/coleta_dataset_3sensores.ino`](../experimentos/cp2-tinyml/firmware/coleta_dataset_3sensores.ino);
- firmware de inferência com ação nos motores: [`src/tinyml_carrinho_cp2.ino`](../src/tinyml_carrinho_cp2.ino);
- modelo local de referência e documentação de suas limitações: [`model/README.md`](../model/README.md);
- protocolo de coleta e formato do CSV: [`CP2_COLETA_DATASET.md`](CP2_COLETA_DATASET.md);
- roteiro de Edge Impulse e lista de evidências: [`CP2_EDGE_IMPULSE.md`](CP2_EDGE_IMPULSE.md);
- índice de fotos, vídeos e o que cada arquivo comprova: [`CP2_EVIDENCIAS.md`](CP2_EVIDENCIAS.md);
- materiais atualizados de 2026: [`../2026/README.md`](../2026/README.md);
- histórico da evolução CP1 → CP2: [`../historico/2026-10-cp2-tinyml.md`](../historico/2026-10-cp2-tinyml.md).

## Matriz de atendimento ao enunciado

| Critério | Evidência no repositório | Situação real em 14/10/2026 |
|---|---|---|
| Sensor utilizado | HC-SR04, pinagem e divisor documentados em [`hardware/arquitetura/README.md`](../hardware/arquitetura/README.md) | Preparado; conferir a fiação final na bancada |
| Dados próprios do grupo | Protocolo, sketch de coleta e espaço para TXT/CSV em `experimentos/cp2-tinyml/` | **Pendente de coleta física** |
| Classes | LIVRE, OBSTACULO_ESQUERDA, OBSTACULO_CENTRO e OBSTACULO_DIREITA | Definidas; confirmar rótulos com a equipe |
| CSV | Formato definido em `CP2_COLETA_DATASET.md` | **Pendente: não apresentar `docs/dataset.csv` como dado real** |
| Preparação no Edge Impulse | Roteiro em `CP2_EDGE_IMPULSE.md` | **Pendente: projeto, prints e link ainda não registrados** |
| Divisão, Impulse e processamento | Tabela de configuração pronta para preencher com os valores usados | **Pendente de execução e evidência** |
| Treinamento e métricas | Modelo local sintético documentado separadamente em `docs/TAREFA21_TINYML.md` | **Não é a métrica do CP2**; registrar a métrica real do Edge Impulse |
| Biblioteca Edge Impulse | Pasta reservada em `model/edge-impulse/` | **Pendente: copiar a biblioteca exportada pelo projeto real** |
| Inferência no ESP32 | `src/tinyml_carrinho_cp2.ino` lê sensores, calcula classe e imprime probabilidades | Código preparado; validar compilação/upload na placa |
| Ação perceptível | O sketch CP2 aciona frente, giro à esquerda/direita e parada conforme a classe | Validar fisicamente e registrar no vídeo |
| Vídeo | [`../2026/2026-atual.mp4`](../2026/2026-atual.mp4) | Demonstra movimento/controle Wi-Fi, mas **não comprova TinyML** |
| Evolução CP1 → CP2 | Commits TinyML existentes + histórico em `historico/2026-10-cp2-tinyml.md` | Organizado; novos commits devem registrar coleta, treino e integração |

## Classes e ação esperada

| Classe | Significado | Ação do carrinho no firmware CP2 |
|---|---|---|
| `LIVRE` | frente livre | avançar |
| `OBSTACULO_ESQUERDA` | obstáculo predominante à esquerda | girar para a direita |
| `OBSTACULO_CENTRO` | obstáculo predominante no centro | escolher o lado com maior distância e girar |
| `OBSTACULO_DIREITA` | obstáculo predominante à direita | girar para a esquerda |

O resultado do modelo não fica apenas no Serial Monitor: a função `executarAcao()`
do sketch altera os sinais da L298N. A ação é interrompida quando uma leitura não
é válida, como medida de segurança.

## O que ainda precisa ser feito antes de marcar 100%

Estas tarefas dependem do carrinho, da placa e da conta Edge Impulse da equipe:

1. gravar o sketch de coleta no ESP32 e coletar sessões reais para as quatro classes;
2. guardar os TXT originais, preencher o registro da coleta e gerar o CSV sem inventar linhas;
3. importar o CSV no Edge Impulse e salvar prints do dataset, divisão e configuração do Impulse;
4. treinar, testar e registrar acurácia, matriz de confusão e exemplos de inferência;
5. exportar a biblioteca Arduino/ESP32 e colocá-la em `model/edge-impulse/`;
6. ligar o modelo exportado ao firmware final, compilar e gravar no ESP32;
7. gravar um vídeo curto em que apareçam simultaneamente sensor, classe/probabilidade e a ação do carrinho;
8. atualizar os campos `[PREENCHER]` desta documentação e fazer commits separados por etapa.

Enquanto esses itens não forem executados, a entrega deve ser descrita como
**preparada para o CP2**, e não como TinyML validado em campo. O dataset sintético
de `docs/dataset.csv` continua preservado apenas como referência da Tarefa 21.

## Atualização da auditoria — 14/10/2026

Foi adicionada a pasta [`../data/CUBI04_Autonomo_V3/`](../data/CUBI04_Autonomo_V3/),
extraída do pacote recebido. Ela contém firmware e um modelo local, mas o arquivo
de origem declara que o dataset tem 6.000 amostras **sintéticas**, não medidas no
carrinho. Esse modelo usa quatro entradas e cinco classes, enquanto o protocolo
CP2 desta documentação usa três sensores frontais e quatro classes.

Por isso, o material foi organizado e preservado como **baseline V3**, mas não foi
marcado como dataset real, treinamento Edge Impulse ou biblioteca Edge Impulse. A
matriz acima continua refletindo somente o que é comprovável no repositório.

## Checklist final para o Teams

- [ ] Link do GitHub atualizado.
- [ ] Caminho indicado: `docs/CP2_TINYML.md`.
- [ ] CSV real e TXT bruto anexados no repositório.
- [ ] Link/ID do projeto Edge Impulse preenchido.
- [ ] Prints do Edge Impulse adicionados em `experimentos/cp2-tinyml/evidencias/`.
- [ ] Biblioteca exportada adicionada em `model/edge-impulse/`.
- [ ] Firmware final compilado e carregado no ESP32.
- [ ] Vídeo CP2 mostra inferência e ação dependente da classe.
- [ ] Nomes e RMs conferidos.
