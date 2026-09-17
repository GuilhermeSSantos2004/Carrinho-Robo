# Suporte reforçado para motor TT — v3 (T03)
Revisão estrutural real a partir das falhas relatadas na versão anterior.
Todas as medidas em mm. Eixos: **Z** = vertical (comprimento do motor), **Y** =
largura da carcaça amarela (22 mm confirmado), **X** = profundidade / direção
do eixo até a roda.

---

## 1. O que mudou em relação à versão anterior

| Problema relatado | Causa provável | O que foi feito nesta v3 |
|---|---|---|
| Folga lateral grande | Vão interno projetado em 38 mm para uma carcaça de 22 mm | Vão interno recalculado para **22,4 mm** (22,0 + 0,2 mm de folga por lado) |
| Peça frontal quebrou no aperto | Presilha fina (4 mm) de 62 mm de vão, trabalhando como mola/viga em balanço | Substituída por **tampa apoiada em duas orelhas maciças**, ligadas à base por **gussets** — a carga do parafuso vai direto para a estrutura, não dobra uma lâmina fina |
| Laterais frágeis | Guias de 2,3 mm | Paredes laterais estruturais de **5,5 mm**, parte da mesma peça que a base (sem emenda fina) |
| Distância entre parafusos de 50 mm | Presilha longa em ponte | Parafusos aproximados para **33,4 mm** entre si (um em cada parede lateral) |
| Postes altos com alívios | Concentração de tensão nas pontas | Sem postes isolados: reforços (gussets) triangulares maciços |

---

## 2. Dimensões usadas e origem de cada uma

### Confirmadas por você (usadas sem alteração)
- Largura da carcaça amarela (Y): **22,0 mm**
- Diâmetro do eixo: **5,35 mm**
- Furo impresso de 3,6 mm passou o parafuso maior no teste físico

### Informadas no anúncio (tratadas como referência, não como certeza absoluta)
- Comprimento do motor (Z): **70 mm**
- Redução 48:1, 3–6 V, ~200 RPM a 6 V, ~28 g

### **Assumidas por mim — AINDA NÃO CONFIRMADAS no seu exemplar** ⚠️
Pesquisei motores TT genéricos equivalentes (mesma família comercial "70×22 mm")
para ter um ponto de partida realista, mas isso **não prova** que seu motor é
idêntico. Estes são os valores que o modelo usa hoje e que você precisa
validar fisicamente antes da impressão final:

| Parâmetro | Valor assumido | Onde está no `params.json` |
|---|---|---|
| Espessura do corpo em X | 18,5 mm | `motor.thickness_x_assumed` |
| Comprimento livre do eixo (fora da carcaça) | 10,0 mm | `motor.shaft_len_assumed` |
| Altura do centro do eixo a partir do fundo da caixa amarela | 9,5 mm | `motor.shaft_height_from_gearbox_bottom_assumed` |
| Diâmetro do ressalto ao redor do eixo | 9,0 mm | `motor.shaft_boss_diam_assumed` |
| Largura entre-chaves da porca pequena | 5,6 mm | `screws.nut_across_flats_assumed` |

**Isso significa que a peça atual NÃO deve ser tratada como encaixe
perfeito garantido.** É uma estrutura corrigida nos pontos que você já
comprovou estarem errados (largura e resistência do aperto), mas as
dimensões acima ainda dependem de uma foto/medição sua.

---

## 3. Ponto crítico que precisa da sua confirmação: folga axial (direção X)

Este é o item mais sensível do projeto inteiro, e por isso não posso
declarar "encaixe garantido" nessa direção:

- Distância da face frontal do motor até o **furo/rebaixo onde a roda
  assenta**: **9,5 mm**
- Comprimento livre do eixo informado no anúncio: **10 mm** (não confirmado
  no seu exemplar)
- Ou seja, a margem de folga é de apenas **~0,5 mm**.

Se o eixo do seu motor for mais curto que 10 mm, a roda não vai assentar
corretamente contra a tampa frontal. Para reduzir esse risco eu:
- Minimizei a espessura estrutural na frente (tampa de 4,5 mm, orelha de
  5 mm de projeção — ainda com parede de 2,8 mm ao redor da porca, que é
  suficiente para PLA);
- Acrescentei um **rebaixo (alívio) de 2,5 mm** na face externa da tampa
  para o cubo da roda se aproximar mais do motor, sobrando 2,0 mm de parede
  nessa região (não estrutural, só de vedação/alinhamento).

**Ação recomendada antes de imprimir a peça definitiva:** meça com um
paquímetro o comprimento de eixo exposto (da face da carcaça amarela até a
ponta) e me informe. Se for diferente de 10 mm, eu ajusto
`front_cap.thickness_x`, `screws.boss_protrusion_x` e
`structure.base_margin_front_x` no `params.json` e regenero os STLs — é
uma mudança de parâmetro, não um redesenho.

---

## 4. Caminho dos esforços (como a carga chega à base)

1. O motor apoia suas duas faces de 22 mm contra as paredes laterais
   (folga de 0,2 mm por lado) — isso já impede a maior parte do
   deslocamento lateral e da rotação em torno de Z.
2. A tampa frontal empurra o motor contra a parede traseira ao ser
   aparafusada — o aperto axial (direção X) é o que impede o motor de
   sair pela frente durante partidas/freadas/inversões.
3. Cada parafuso passa por uma **orelha maciça** (13 × 12 × 5 mm) que é
   parte da mesma peça das paredes laterais — não é um poste isolado.
4. Cada orelha tem um **gusset triangular sólido** ligando-a diretamente
   à base — a força de aperto vai da tampa → orelha → gusset → base,
   sem passar por uma seção fina que possa fletir.
5. A porca fica cativa num rebaixo hexagonal **atrás** da orelha (lado
   da cavidade do motor mas fora da zona onde o motor realmente encosta),
   então o aperto não deforma a carcaça amarela — a reação de torque fica
   na orelha, não no plástico fino.
6. A base (7 mm) e as paredes (5,5 mm) trabalham como uma peça única
   (sem emendas coladas/rosqueadas entre si), reduzindo o risco de
   delaminação entre camadas nos pontos de carga.

**O que isso NÃO prova:** validade de malha e ausência de colisão não são
ensaio de resistência. Não fiz simulação de elementos finitos nem tenho
dados de rigidez real do PLA impresso nesta orientação — a confirmação
final tem que ser o teste físico (aperto, partidas/paradas/inversões,
inspeção de trincas).

### Retenção contra rotação/deslocamento durante inversão de sentido
O projeto atual confia em:
- Encaixe lateral justo (0,2 mm/lado) — impede rotação em torno de Z e
  deslocamento em Y;
- Aperto axial pela tampa — impede deslocamento em X.

**Não modelei** furos passantes usando os furos de fixação próprios do
motor (parafuso + porca através da carcaça), porque a posição, o
diâmetro e até a existência desses furos no seu exemplar **não foram
confirmados** (item pendente da seção 3 do seu pedido). Isso é uma
retenção estrutural adicional que os suportes comerciais costumam usar —
recomendo fotografar a carcaça (as duas faces de 22 mm e a face de trás)
para eu acrescentar esse recurso numa v4, se você quiser essa retenção
extra.

---

## 5. Ferragens — o que usar e quantidade

| Item | Uso neste suporte (1 motor) | Você tem | Observação |
|---|---|---|---|
| Parafuso pequeno (8–10 mm) | 2 (tampa → porca) | 6 | Comprimento suficiente: o aperto real precisa de ~9,5 mm de curso (tampa + parede da orelha + engate na porca). Os parafusos de 27 mm ficariam sobrando muito — prefira os curtos aqui. |
| Porca que rosqueia no parafuso pequeno | 2 | (a confirmar quantidade) | Fica cativa no rebaixo hexagonal atrás de cada orelha. **Confirme a largura entre-chaves real antes de imprimir em definitivo** — o modelo assume 5,6 mm. |
| Parafuso maior (27 mm) + porca | Não usado nesta peça de teste | 7 | Reservados para quando o suporte for integrado ao chassi do robô (fixação do conjunto à base), ou para a fixação passante pelos furos próprios do motor, se confirmarmos essa opção depois. |
| Espaçador hexagonal 29–30 mm | Não usado | 6 | Não reaproveitado como limitador — o comprimento não foi verificado para essa função, conforme você pediu. |
| Arruelas | Recomendado 1 por parafuso, sob a cabeça, do lado externo da tampa | — | Distribui a carga do aperto sobre o PLA e reduz o risco do parafuso “afundar” na tampa depois de ciclos de vibração. Avise se quiser que eu redimensione o rebaixo da cabeça do parafuso para elas. |

**Para os quatro motores** (quando integrar ao chassi): serão necessários
8 parafusos pequenos + 8 porcas pequenas no total — você tem 6 parafusos
pequenos, faltam 2. As porcas pequenas você precisa confirmar quantas
tem.

---

## 6. Verificações executadas (reais, nesta sessão)

### Malha / STL
| Verificação | Corpo principal (T03A) | Tampa (T03B) |
|---|---|---|
| Malha fechada (watertight) | ✅ Sim | ✅ Sim |
| Uma única peça conectada | ✅ Sim (1 corpo) | ✅ Sim (1 corpo) |
| Faces degeneradas/quebradas | ✅ 0 encontradas | ✅ 0 encontradas |
| Volume positivo | ✅ 41.731 mm³ | ✅ 12.609 mm³ |
| Sólido topologicamente válido (BRep, `BRepCheck_Analyzer`) | ✅ Válido | ✅ Válido |

Ferramentas usadas: CadQuery/OCP para o sólido, `trimesh` para checar a
malha exportada (watertight, contagem de corpos, faces quebradas).

### Dimensões e unidades
- Todas as operações foram feitas em milímetros (padrão CadQuery); STL e
  STEP exportados sem fator de escala.
- Conferi manualmente as caixas delimitadoras (bounding box) de cada
  peça contra os parâmetros esperados após cada alteração — os números
  batem com `params.json` (ver seção 2).

### Montagem / geometria (verificado no CAD real, não em desenho ilustrativo)
- ✅ Motor (volume simplificado 22×18,5×70 mm) cabe na cavidade com a
  folga programada de 0,2 mm por lado.
- ✅ Tampa frontal encosta nas duas orelhas sem precisar se deformar
  (junta plana simples, mais um rebaixo circular de centragem em cada
  parafuso).
- ✅ Furo passante de 3,6 mm alinhado entre orelha e tampa nas duas
  posições.
- ✅ Rebaixo hexagonal cativo não atravessa a parede da orelha (sobra
  2,8 mm de material sólido).
- ✅ Janela traseira desobstrui a ponta interna do eixo (16×16 mm,
  centrada na altura assumida do eixo).
- ⚠️ Folga axial para a roda: **apenas ~0,5 mm de margem** frente ao
  comprimento de eixo assumido — ver seção 3, pendente de confirmação
  física.
- ⚠️ Não verifiquei colisão com a roda real (68 mm) além de um raio
  genérico, porque a distância real carcaça↔face interna da roda não foi
  medida no seu conjunto (você pediu explicitamente para eu não reciclar
  o valor de 8 mm da versão anterior). O modelo deixa a região de saída
  do eixo livre e aberta; a verificação final de giro livre é física.

### O que eu NÃO fiz (para não inventar certeza)
- Não rodei simulação estrutural/FEA nem apresentei propriedades
  genéricas de PLA como prova de resistência.
- Não modelei os furos de fixação próprios do motor (posição não
  confirmada).
- Não modelei a lingueta amarela inferior com precisão — deixei um vão
  de alívio genérico (10×10 mm) na base, que precisa ser conferido/ajustado
  contra o motor real.
- Não gerei G-code — os STLs devem ser fatiados no seu perfil de
  impressora.

---

## 7. Impressão

- Material: PLA (ponto de partida). PETG é uma alternativa razoável se
  quiser mais tenacidade nas orelhas/gussets, mas troca de material não
  substitui a correção estrutural já feita.
- Perfil de fatiamento sugerido: bico 0,4 mm, camada 0,20 mm, ~6
  perímetros, 5–6 camadas de topo/fundo. Não é necessário 100% de
  preenchimento — a resistência já vem da geometria (paredes e orelhas
  maciças), não do preenchimento interno.
- **Orientação de impressão sugerida:**
  - **Corpo principal (T03A):** imprimir em pé, com a base (Z=0) no
    prato — assim a carga de aperto (direção X) fica **no plano das
    camadas**, e não perpendicular a elas nas orelhas/gussets, reduzindo
    o risco de delaminação exatamente nos pontos que quebraram antes.
  - **Tampa (T03B):** imprimir deitada, com a face traseira (a que
    encosta nas orelhas) no prato — mesma lógica: a força de aperto e o
    encaixe do rebaixo hexagonal ficam no plano das camadas.
- **Suportes de impressão:** prováveis apenas sob os gussets triangulares
  (saliência em balanço) — confira no seu fatiador com o corpo nessa
  orientação; caso o fatiador não precise, ótimo, mas não estou
  prometendo isso sem você conferir no seu software.

### Estimativa de material
- Corpo principal: 41,73 cm³ de volume sólido → **~51,7 g** se fosse
  100% maciço.
- Tampa: 12,61 cm³ → **~15,6 g** se fosse 100% maciça.
- **Esta é uma estimativa por volume geométrico (densidade PLA
  1,24 g/cm³), não uma estimativa do fatiador.** Como a peça é composta
  principalmente por paredes e reforços maciços (poucas cavidades
  internas grandes), o consumo real ao fatiar com perímetros + 100% ou
  alto preenchimento deve ficar próximo desses valores; com preenchimento
  baixo pode cair um pouco nas regiões mais espessas da base. Recomendo
  conferir o número real no seu fatiador antes de finalizar.

---

## 8. Instruções de montagem (resumo)

1. Insira o motor pela abertura superior do corpo principal (T03A),
   encostando a carcaça amarela contra a parede traseira.
2. Confirme visualmente que a ponta interna do eixo fica livre na janela
   traseira e que os fios saem pelo topo aberto.
3. Coloque as duas porcas pequenas nos rebaixos hexagonais (parte de
   trás de cada orelha).
4. Encaixe a tampa frontal (T03B) contra as orelhas — os dois rebaixos
   circulares centralizam a peça.
5. Passe os parafusos pequenos pela face externa da tampa e aperte até
   a tampa encostar nas orelhas (esse encosto é o limitador — quando
   bater, o motor já deve estar firme; se ainda balançar, é sinal de que
   a espessura real do motor em X é diferente do valor assumido de
   18,5 mm, e o parâmetro `motor.thickness_x_assumed` precisa ser
   corrigido para eu regenerar a peça).
6. Verifique giro livre do eixo antes de instalar a roda.

---

## 9. Pendências que só você pode resolver

1. **Comprimento livre do eixo** (crítico — ver seção 3).
2. **Espessura real do motor em X** (18,5 mm é uma estimativa de
   referência de motores TT semelhantes, não do seu exemplar).
3. **Largura entre-chaves real da porca/espaçador hexagonal** (assumi
   5,6 mm; você pediu explicitamente para não usar 9 mm).
4. Foto da carcaça amarela (as duas faces estreitas e o fundo) para eu
   posicionar corretamente a lingueta inferior e avaliar os furos de
   fixação próprios do motor, caso você queira a retenção passante
   parafuso+porca pela carcaça numa v4.
5. Distância real carcaça↔face interna da roda com a roda encaixada
   (com régua), se quiser que eu valide a folga da roda com precisão em
   vez de um raio genérico de verificação.

Assim que tiver essas medidas, eu atualizo `params.json` e regenero os
arquivos — é uma revisão de parâmetro, não preciso remodelar do zero.

---

## 10. Arquivos entregues

- `T03A_suporte_motor_reforcado_v3.stl` — corpo principal (impressão)
- `T03A_suporte_motor_reforcado_v3.step` — corpo principal (editável)
- `T03B_frente_apoiada_v3.stl` — tampa frontal (impressão)
- `T03B_frente_apoiada_v3.step` — tampa frontal (editável)
- `build.py` — fonte paramétrica CadQuery (controla a geometria de
  verdade)
- `params.json` — todos os parâmetros dimensionais, com marcação clara
  do que é confirmado vs. assumido
- `preview_01_frontal.png`, `preview_02_traseira.png`,
  `preview_03_explodida.png`, `preview_04_detalhe_aperto.png` — gerados
  a partir do CAD real (motor e roda são volumes simplificados,
  claramente identificados como tal)
- Este relatório (`RELATORIO_v3.md`)

Não há um `T03C_limitador_v3` nesta versão: o próprio encosto da tampa
contra as orelhas funciona como limitador, então uma terceira peça não
se mostrou necessária com as informações disponíveis. Se a medição do
comprimento do eixo ou da espessura do motor mostrar que isso não é
suficiente, um limitador/calço separado pode ser adicionado na v4.
