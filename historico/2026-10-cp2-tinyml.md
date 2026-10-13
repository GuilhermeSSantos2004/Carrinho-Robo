# Evolução CP1 → CP2 — TinyML — 13/10/2026

Este registro documenta a organização do repositório para o Check Point 02 sem
confundir preparação de software com validação física.

## Ponto de partida

O projeto já tinha:

- firmware principal com controle Wi-Fi e movimentação;
- sketches isolados de leitura e pré-processamento do HC-SR04;
- `docs/dataset.csv` e `model/model.h` da Tarefa 21;
- commits de preparação de TinyML no histórico Git;
- vídeo recente do carrinho em `2026/2026-atual.mp4`.

## Problemas encontrados na auditoria

1. O dataset existente é explicitamente sintético e não atende sozinho à exigência
   de dados coletados pelo grupo.
2. A inferência existente imprime a classe no Serial Monitor, mas o sketch original
   não aciona os motores.
3. Não havia uma página central do CP2 ligando coleta, Edge Impulse, exportação e
   demonstração.
4. O vídeo recente mostra movimento/controle pelo celular, mas não mostra evidência
   suficiente do ciclo TinyML.
5. Faltavam campos estruturados para registrar o projeto Edge Impulse, métricas,
   export da biblioteca e teste no ESP32.

## Alterações desta organização

- criada a página central `docs/CP2_TINYML.md` com matriz de atendimento;
- criado o protocolo de coleta real em `docs/CP2_COLETA_DATASET.md`;
- criado o registro do Edge Impulse em `docs/CP2_EDGE_IMPULSE.md`;
- criado o índice de evidências em `docs/CP2_EVIDENCIAS.md`;
- criado firmware de coleta de três sensores em `experimentos/cp2-tinyml/`;
- criado firmware integrado de referência em `src/tinyml_carrinho_cp2.ino`, no qual
  a classe inferida decide avançar, parar/escolher lado ou girar;
- documentadas as mídias de 2026 em `2026/README.md`;
- documentada a diferença entre `model.h` local/sintético e a biblioteca real do
  Edge Impulse em `model/README.md`.

## Próximo registro obrigatório

Depois da execução física, acrescentar um commit para cada etapa relevante, por
exemplo:

```text
feat(cp2): adiciona coleta real do HC-SR04
docs(cp2): registra projeto e métricas do Edge Impulse
feat(cp2): integra biblioteca Edge Impulse ao ESP32
test(cp2): valida ação do carrinho por classe
docs(cp2): adiciona vídeo e evidências finais
```

Os nomes acima são sugestões para commits futuros; não representam atividades já
realizadas. O histórico existente continua preservado.
