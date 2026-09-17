# CAD

Esta pasta reúne os modelos mecânicos do carrinho-robô e o histórico das revisões usadas no protótipo atual.

## Estrutura

- [`STL/`](STL/README.md): peças exportadas para impressão 3D e manifesto dos arquivos atuais;
- [`fonte-modelo/`](fonte-modelo/README.md): fontes/arquivos editáveis do processo de modelagem;
- [`versoes/`](versoes/): revisões preservadas para rastreabilidade.

## Estado mecânico atual

O projeto deixou de usar o conceito inicial de chassi 2WD simples e evoluiu para uma estrutura própria com quatro motores TT.

### T01 — base/chassi

- dimensão verificada no STL atual: **147 × 140 × 10 mm**;
- peça única e fechada;
- pedestais altos removidos para reduzir material/tempo;
- furos de montagem integrados ao piso;
- borda externa reduzida em relação às primeiras versões.

### T02 — carcaça

- dimensão verificada no STL atual: **140 × 133 × 24 mm**;
- conceito baixo, deixando os suportes dos motores aparentes/atravessando a região superior;
- aberturas laterais necessárias para a montagem dos suportes;
- região frontal preparada para integração do sensor ultrassônico;
- geometria otimizada para permanecer conectada sem gerar a grande quantidade de suporte observada nas revisões intermediárias.

## Peças relacionadas

| Código | Peça | Estado |
|---|---|---|
| T01 | Base/chassi | versão atual otimizada |
| T02 | Carcaça | versão atual baixa/aberta |
| T03A | Suporte do motor | v7 |
| T03B | Presilha do motor | v7 |
| T04 | Tampa | 2 mm; não obrigatória no conceito atual |
| T05 | Trava | pino vertical com nervuras |
| T06 | Pino/parafuso impresso | 8 previstos, 2 por motor |

O processo detalhado de erros, colisões, fragmentação da carcaça, overhangs e correções está em [`../historico/HISTORICO_PROJETO_CUBI04.md`](../historico/HISTORICO_PROJETO_CUBI04.md).
