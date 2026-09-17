# Arquivos STL — estado atual

A pasta `cad/STL` é o destino dos modelos finais/prontos para impressão 3D.

## Arquivos atuais recebidos e validados em 17/09/2026

| Peça | Nome canônico | Dimensões verificadas | Malha | SHA-256 |
|---|---|---:|---|---|
| T01 — base/chassi | `T01_base_chassi_v1.stl` | 147 × 140 × 10 mm | watertight, 1 componente | `eb64ccd5b728aa3ce2f8b160ebef9cd1e81fa0a3d14aa14748b384381642305c` |
| T02 — carcaça | `T02_carcaca_v1.stl` | 140 × 133 × 24 mm | watertight, 1 componente | `9b4c74ce3a754fc66779b3ae2e0d0ffea1d1760d4863c7cae51cd6d99ce83dc4` |

### T01

O chassi atual foi otimizado para impressão mais rápida e menor uso de material. A borda foi reduzida, os pedestais foram removidos e os pontos de fixação passaram para o piso da peça.

### T02

A carcaça atual é baixa, com aberturas necessárias para os suportes dos motores. Ela foi redesenhada após versões que criavam corpos separados ou pontes que exigiam muito suporte de impressão. A região frontal considera a montagem do ultrassônico.

## Demais peças

A montagem também utiliza as revisões atuais do T03A, T03B, T05 e T06. Consulte o histórico consolidado em [`../../historico/HISTORICO_PROJETO_CUBI04.md`](../../historico/HISTORICO_PROJETO_CUBI04.md).

## Regra de versionamento

- manter o código da peça no início do nome (`T01`, `T02`, ...);
- preservar revisões antigas na área de versões/histórico;
- o arquivo sem sufixo adicional deve representar a revisão física vigente;
- registrar no histórico toda alteração que afete encaixe, furação, impressão ou montagem.
