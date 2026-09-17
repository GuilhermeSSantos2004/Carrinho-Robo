# Componentes — configuração atual do protótipo

Esta página registra o hardware que está sendo realmente usado na montagem atual. A lista antiga de alternativas (8 pilhas AA / pack 2S) foi substituída pelo estado físico atual do carrinho.

## Componentes principais

| Quantidade | Componente | Estado atual |
|---:|---|---|
| 1 | ESP32 DevKit V1 / NodeMCU-ESP32 | Em uso |
| 1 | Ponte H dupla L298N | Em uso, com PWM em ENA/ENB |
| 4 | Motores TT amarelos 3–6 V, redução anunciada 48:1 | Em uso |
| 1 | Sensor ultrassônico HC-SR04 | Em uso como sensor de estacionamento de ré |
| 1 | LDR | Em uso no GPIO 34 para luminosidade/tema do painel |
| 3 | Células 18650 no suporte em série | Alimentação de potência atual |
| 3 | Resistores de 1 kΩ | Divisor do ECHO do HC-SR04 (1 kΩ / 2 kΩ) |
| — | Jumpers | Interligações |
| 1 | Chassi impresso T01 | Versão atual otimizada |
| 1 | Carcaça impressa T02 | Versão atual baixa/otimizada |
| 4 | Suportes T03A | v7 |
| 4 | Presilhas T03B | v7 |
| 4 | Travas T05 | Fixação vertical dos suportes |
| 8 | Pinos/parafusos impressos T06 | 2 por motor |

## Alimentação

O suporte em uso comporta **3 células 18650 em série**:

- 3,7 V nominais por célula;
- aproximadamente **11,1 V nominal** no conjunto;
- até aproximadamente **12,6 V** com as três células carregadas a 4,2 V;
- pack ligado à entrada de potência da L298N;
- ESP32 alimentado separadamente por USB/power bank durante os testes;
- todos os GNDs devem permanecer em comum.

### Atenção aos motores

Os motores TT são 3–6 V, portanto a tensão do pack 3S é superior à especificação dos motores. O firmware trabalha com PWM reduzido durante os testes, mas isso não transforma 12,6 V em uma alimentação 6 V equivalente. Evitar velocidade máxima e verificar aquecimento. A alimentação definitiva dos motores deve ser adequada à tensão deles.

## Pinagem consolidada

| Função | GPIO |
|---|---:|
| PWM canal A (`ENA`) | 25 |
| IN1 | 26 |
| IN2 | 27 |
| PWM canal B (`ENB`) | 33 |
| IN3 | 32 |
| IN4 | 23 |
| HC-SR04 TRIG | 18 |
| HC-SR04 ECHO | 19 (com divisor) |
| LDR analógico | 34 |

## Divisor do HC-SR04

Foram usados três resistores `MPV` (marrom-preto-vermelho), equivalentes a **1 kΩ** cada:

```text
ECHO HC-SR04
   │
  1 kΩ
   │
   ├──── GPIO 19
   │
  1 kΩ
   │
  1 kΩ
   │
  GND
```

## LDR

Ligação prevista:

```text
3V3 ── LDR ──┬── GPIO 34
              │
           resistor
              │
             GND
```

O firmware exibe o ADC bruto para facilitar a calibração real de DIA / MEIA-LUZ / NOITE.

## Mecânica atual

O projeto deixou de usar o conceito inicial de kit 2WD simples. A versão atual possui **quatro motores TT**, suportes impressos individuais e peças CAD próprias. Consulte [`../../cad`](../../cad) e o histórico de desenvolvimento para as revisões mecânicas.
