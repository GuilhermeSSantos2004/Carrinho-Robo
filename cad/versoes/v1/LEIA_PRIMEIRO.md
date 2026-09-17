# CUBI-04 v1 - primeiro teste o encaixe

Kit autoral de robô 4WD com corpo quadrado, inspirado no WALL-E. Unidade: **milímetros**, escala **100%**.

Esta versão usa hipóteses para componentes não medidos. O encaixe perfeito NÃO foi confirmado. Comece pelo guia PDF e pelos testes; não imprima toda a estrutura antes de conferir motor, hexágonos e placas reais.

## Conteúdo

- `STL/`: 11 modelos principais (15 peças ao todo) e 2 calços opcionais.
- `TESTES/`: 4 testes pequenos; imprima T03 junto de uma presilha 05.
- `PREVIAS/`: imagens da geometria montada, aberta, explodida e traseira.
- `FONTE/`: gerador editável em CadQuery, medidas de referência e STEP em posição de montagem.
- `VALIDACAO/`: dimensões, integridade de malhas e interseções geométricas.
- `CUBI_04_guia.pdf`: medidas, orientações, suportes, montagem e alimentação.

## Hipóteses críticas

| Item | Dimensão usada |
|---|---|
| Parafuso grande | M4 x 27 mm, 7 unidades |
| Porca grande | M4, 7 mm entre faces, 7 unidades necessárias |
| Parafuso pequeno | M3 x 9 mm, 6 unidades |
| Espaçador hexagonal | 29 mm de comprimento, 9 mm entre faces, rosca M3; 6 unidades |
| Motor TT vertical | Corpo de referência 70 x 22 x 18 mm; eixo a 12 mm da ponta de baixo |
| Suporte 3x18650 | 75 x 60 mm; reserva de altura de 26 mm com células |
| ESP32 | Modelo não informado: envelope de placa 31 x 58 mm |
| Sensor | HC-SR04 presumido; cápsulas de 16 mm, passo de 26 mm |

Todos esses encaixes precisam de confirmação física. A foto do hexágono não tem escala. Meça a largura entre faces planas, a rosca e o comprimento sob a cabeça dos parafusos. Confira o formato completo do motor, inclusive ressaltos, linguetas e eixo. A base usa o encaixe hexagonal de 9,4 mm do teste T02.

## Impressão

Todas as peças cabem individualmente em 180 x 180 x 180 mm. A maior base tem 163 x 156 x 89 mm; a carcaça tem 160 x 161,55 x 128 mm. O robô nominal montado mede aproximadamente 218 x 165 x 233 mm, com rodas de 70 mm e 18 mm de distância do chão à base.

Arquivos já orientados para impressão. Use camada de 0,20 mm, bico de 0,4 mm, 4 paredes e 25-30% de preenchimento como ponto de partida. PETG é preferível nas travas. A carcaça exige suporte nos tetos das janelas laterais; cabeça e pescoço exigem suportes localizados. Consulte a prévia de camadas no fatiador. Não foi feito fatiamento, G-code ou teste físico; massa e tempo devem vir do seu fatiador.

Use também cintas de nylon de 2,5 mm e isolamento para as placas e bateria. A reserva para um segundo driver e dois reguladores é opcional e tem envelopes definidos no guia.

## Alimentação

Três células em série chegam a 12,6 V, incompatíveis com ligação direta aos motores de 3-6 V ou ao ESP32. É necessário adequar alimentação, proteção e corrente. Um L298N possui dois canais: ligar dois motores em cada um depende da corrente de partida/travamento e da dissipação. O anúncio só informa corrente sem carga. Veja as referências e a explicação no PDF antes de energizar. A capacidade de 9.800 mAh não foi verificada.

## Fonte editável

O STEP contém as peças estruturais CAD em posição de montagem; não é um arquivo para imprimir tudo unido. Os pinos estão nos STLs e no gerador. O código não inclui firmware.

Reprodução dos STLs, em ambiente com Python e CadQuery 2.7.x:

```bash
python FONTE/gerar_cubi.py --cache ./temporarios_cubi
```

`parametros.json` documenta as medidas de referência e permite ajustes utilizados no gerador. Nem todas as cotas são independentes: mudar o tamanho de uma placa, do corpo, do eixo ou do motor exige revisar as relações geométricas no código e repetir a validação. Não escale o STL inteiro para corrigir uma medida. Para outro hexágono, ajuste a medida nominal e a folga do soquete e teste antes da base.

O computador verificou integridade dos sólidos/malhas e montagem nominal. Não verificou seu hardware, resistência das peças, vida útil das travas, estabilidade, corrente, aquecimento ou desempenho no chão.

Para repetir as verificações geométricas após uma edição:

```bash
python FONTE/validar_malhas.py
python FONTE/validar_montagem.py
```

Os envelopes usados nas verificações também precisam ser atualizados quando as medidas mudarem. Os validadores não substituem fatiamento e teste físico.
