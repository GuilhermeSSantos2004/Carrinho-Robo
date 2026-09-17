# Arquitetura eletrônica — estado atual

O protótipo atual usa **ESP32 DevKit V1**, ponte H **L298N**, **4 motores TT amarelos 3–6 V**, sensor ultrassônico **HC-SR04**, LDR e alimentação por células 18650. O ESP32 cria sua própria rede Wi-Fi e hospeda o painel de controle local.

## Alimentação atual

O suporte disponível usa **3 células 18650 em série**:

- tensão nominal aproximada: **11,1 V**;
- tensão máxima com as três células totalmente carregadas: **12,6 V**;
- positivo do pack vai para `12V/Vs` da L298N;
- negativo do pack vai para o GND da L298N;
- durante os testes, o ESP32 pode ser alimentado por USB do notebook ou power bank;
- ESP32, L298N, HC-SR04 e LDR precisam compartilhar referência de GND.

> **Atenção:** os motores TT usados são especificados para 3–6 V. O pack 3S é maior que a tensão nominal dos motores. O firmware limita o PWM para testes, mas PWM não substitui uma alimentação adequada dos motores. Evitar PWM alto e observar aquecimento. Para solução definitiva, usar alimentação/regulação compatível com os motores.

> Nunca ligar 11,1–12,6 V diretamente no `3V3`, `5V` ou GPIO do ESP32.

## ESP32 → L298N

Os jumpers `ENA` e `ENB` foram removidos porque agora há controle de velocidade por PWM.

| ESP32 | L298N | Função |
|---:|---|---|
| GPIO 25 | ENA | PWM do canal A |
| GPIO 26 | IN1 | Direção canal A |
| GPIO 27 | IN2 | Direção canal A |
| GPIO 33 | ENB | PWM do canal B |
| GPIO 32 | IN3 | Direção canal B |
| GPIO 23 | IN4 | Direção canal B |
| GND | GND | Referência comum |

A saída `5V` da L298N não é usada para alimentar o ESP32 no arranjo atual de testes.

## Quatro motores TT → L298N

O carrinho usa dois motores por lado:

| Lado | Ligação |
|---|---|
| 2 motores do lado A | em paralelo em `OUT1/OUT2` |
| 2 motores do lado B | em paralelo em `OUT3/OUT4` |

Os quatro motores são TT amarelos, 3–6 V, redução anunciada de 48:1.

## HC-SR04 → ESP32

| HC-SR04 | Ligação |
|---|---|
| VCC | 5 V do ESP32 enquanto ele é alimentado por USB/power bank |
| GND | GND comum |
| TRIG | GPIO 18 |
| ECHO | GPIO 19 através de divisor de tensão |

O `ECHO` do HC-SR04 pode chegar a aproximadamente 5 V e **não deve ser ligado diretamente** ao GPIO 19. O divisor montado com os resistores disponíveis usa três resistores de 1 kΩ:

```text
ECHO ── 1 kΩ ──┬── GPIO 19
                │
               1 kΩ
                │
               1 kΩ
                │
               GND
```

Isso forma 1 kΩ no ramo superior e 2 kΩ no ramo inferior.

## LDR → ESP32

O sensor de luminosidade usa o **GPIO 34** como entrada analógica.

```text
3V3
 │
 LDR
 │
 ├──────── GPIO 34
 │
 resistor
 │
 GND
```

- alimentar o divisor do LDR com `3V3`;
- não aplicar 5 V ao GPIO 34;
- o firmware faz média de 10 leituras;
- o valor bruto ADC e a luminosidade percentual aparecem no painel;
- logs do LDR são enviados ao Serial Monitor aproximadamente a cada 5 s.

## GND comum

A referência final deve ser comum:

```text
GND ESP32 ─────┬──── GND HC-SR04
               ├──── GND do divisor do ECHO
               ├──── GND do LDR
               └──── GND L298N / negativo da bateria
```

## Comportamento do ultrassônico no firmware atual

O HC-SR04 deixou de bloquear o avanço a 20 cm. Ele agora funciona como **sensor de estacionamento de ré**:

- frente: liberada normalmente;
- ré: liberada enquanto a distância for maior que 5 cm;
- ré a `<= 5 cm`: motores param e o estado passa para `RE_BLOQUEADA`;
- painel mostra a distância;
- durante a ré, o celular emite bipes que aceleram conforme o obstáculo se aproxima;
- em distância crítica, o alerta se torna contínuo.

## Verificação antes de ligar

1. Montar tudo com a alimentação desligada.
2. Conferir polaridade das três 18650.
3. Não conectar o pack 3S diretamente ao ESP32.
4. Conferir GND comum.
5. Conferir divisor do `ECHO` antes de ligar o HC-SR04.
6. Conferir que `ENA` e `ENB` estão sem jumper para PWM.
7. Fazer o primeiro teste com as rodas suspensas.
8. Começar com PWM baixo por causa dos motores 3–6 V.
9. Abrir o Serial Monitor em 115200 baud para conferir sensores e estados.
