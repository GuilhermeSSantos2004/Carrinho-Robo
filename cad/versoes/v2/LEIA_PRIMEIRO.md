# Teste do motor em pé — CUBI T03 v2

Imprima **1 suporte T03A e 1 presilha T03B**, ambos a **100%**, em milímetros.
Este conjunto é um teste de bancada do encaixe do motor. A estrutura completa
do robô v1 ainda não foi redesenhada com as novas medidas.

## O que foi corrigido

O teste anterior foi dimensionado para um corpo de 70 × 22 × 18 mm.
A especificação enviada agora informa 70 × 37 × 22,5 mm e eixo de 5,35 mm.
Nesta versão há **38 mm de largura livre**, para acomodar um corpo de até
37 mm com folga lateral, e uma presilha cuja posição ajusta a espessura.
Foram conferidos envelopes de 18 a 22,5 mm de espessura e 22 a 37 mm de largura.

O motor fica com o corpo comprido na vertical, a parte metálica e os fios para
cima, e o eixo das rodas na horizontal. O topo fica aberto, inclusive para a
pequena ponta do motor. A janela traseira libera a ponta interna do eixo; a
parte da frente abaixo da presilha também fica aberta. A base em U deixa uma
abertura para a lingueta amarela inferior. A presilha aperta a carcaça amarela,
sem usar o eixo como apoio.

As medidas gerais não informam a posição do eixo, o tamanho dos ressaltos nem
o formato exato da lingueta. As fotos também não permitem medi-los com precisão.
Por isso, esta peça usa aberturas amplas; o encaixe perfeito só pode ser confirmado
no seu motor. O desenho do motor na prévia é ilustrativo, com corpo menor que
o envelope máximo de 37 mm; não é uma cópia dimensional do seu exemplar.

## Peças e impressão

| Arquivo | Quantidade | Dimensões na posição de impressão |
|---|---:|---:|
| `STL/T03A_suporte_motor_em_pe_v2.stl` | 1 | 33 × 66 × 56 mm |
| `STL/T03B_presilha_regulavel_v2.stl` | 1 | 16 × 62 × 4 mm |

- PLA, bico de 0,4 mm, camada de 0,20 mm, 4 paredes, 5 camadas inferiores e superiores, 25% de preenchimento.
- Os STLs já estão orientados: suporte apoiado na base; presilha deitada na face plana.
- O teto da janela traseira foi feito a 45°. Não há suporte interno projetado; confira no fatiador os pequenos furos horizontais de 3,6 mm. Se necessário, use suportes localizados apenas nesses furos.
- Brim externo de 4–5 mm é opcional para melhorar a adesão do suporte.
- As duas peças têm **32,3 g de PLA se fossem totalmente maciças**, usando densidade de 1,24 g/cm³. Estimativa para impressão: **25–33 g**, mais brim/suportes se usados. O valor final deve ser lido no fatiador.
- Use seu perfil de impressora e gere um G-code novo. O G-code antigo pertence ao outro STL e não serve para esta versão.

## Ferragens

Use **2 parafusos com cerca de 27 mm de comprimento útil e as 2 porcas que
efetivamente rosqueiam neles**. Os quatro furos impressos têm passagem de
**3,6 mm**, o tamanho que você confirmou no teste anterior. Isso é o tamanho
do furo, não uma identificação da rosca do parafuso.

As porcas ficam acessíveis atrás dos dois postes laterais. Não há um encaixe
hexagonal com dimensão presumida. Segure cada porca com uma chave ou alicate
enquanto aperta o parafuso pela frente. Os espaçadores hexagonais compridos
não são necessários neste teste.

## Como montar e conferir

1. Retire eventual brim e limpe os quatro furos, sem ampliar o encaixe do motor.
2. Deixe a presilha separada e introduza o motor **pela frente**, com a parte metálica para cima. Posicione a lingueta inferior na abertura da base e a ponta interna do eixo na janela traseira.
3. Apoie as laterais inferiores da carcaça amarela nos dois apoios elevados. A lingueta não deve sustentar o peso do motor.
4. Centralize o corpo e encoste a presilha na face amarela, na altura dos furos dos postes. Ela se ajusta à espessura do corpo pelo aperto dos parafusos.
5. Passe os dois parafusos pela presilha e pelos postes; coloque as porcas por trás. Aperte aos poucos e de forma alternada, apenas até remover a folga. Não deforme a caixa de redução.
6. Confira que as duas pontas do eixo, os ressaltos e a lingueta não raspam no suporte. Para um teste de giro, respeite a alimentação de 3–6 V informada para o motor.
7. Se colocar a roda de 70 mm, eleve o conjunto ou deixe a roda fora da borda da mesa: neste gabarito a roda fica abaixo da base. A distância axial real entre roda e suporte também precisa ser conferida no seu conjunto.

Se alguma saliência encostar, fotografe o motor montado no suporte, de frente
e por trás, mostrando o ponto. Corrija a região do STL; não escale a peça
inteira, pois isso altera também os furos e a distância entre os parafusos.

## Verificações realizadas

- Dois sólidos CAD válidos; cada STL contém uma única peça conectada.
- Malhas fechadas, sem arestas abertas ou não manifold, sem triângulos degenerados e com volume positivo.
- 68 verificações de interferência entre suportes, presilha e envelopes de motor, eixos, lingueta, parafusos e roda.
- Eixo de 5,35 mm conferido, como envelope, em alturas de 16, 20 e 27 mm a partir da base e deslocamentos laterais de −2, 0 e +2 mm. Esses valores são faixas verificadas, não cotas medidas do motor.
- A roda foi representada com uma face interna a 8 mm da carcaça: esta folga depende do seu cubo de roda e não foi medida.

O teste físico, a resistência sob carga e a impressão não foram executados.
Este suporte é um gabarito de bancada. Após confirmar o encaixe, é necessário
integrar o suporte à base definitiva do robô, verificando rodas e carcaça.

## Fonte editável

Requer Python com `cadquery==2.7.0` e `numpy`.

```bash
python FONTE/gerar_teste_motor_v2.py
```

Edite as constantes no dicionário `P` e a geometria correspondente no script.
`parametros.json` é um registro dos parâmetros usados, não uma entrada automática.
A geometria tem dimensões locais explícitas; alterar apenas `P` não redimensiona
todo o suporte. O STEP contém as duas peças na posição de montagem para o
envelope de 22,5 mm de espessura.

Referência consultada para comparação com o modelo inicial:
[Adafruit TT 3777, corpo de 70 × 22 × 18 mm](https://www.adafruit.com/product/3777).
As novas medidas gerais vieram da especificação enviada pelo usuário.
