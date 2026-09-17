# Código-fonte do ESP32

Esta pasta contém o firmware do carrinho-robô. O arquivo [`codigo.ino`](codigo.ino) representa sempre a versão mais recente em uso.

## Versões

| Versão | Arquivo | Alterações principais |
|---:|---|---|
| **v0.2** | [`v0.2/carrinho_robo_v0_2/carrinho_robo_v0_2.ino`](v0.2/carrinho_robo_v0_2/carrinho_robo_v0_2.ino) | HC-SR04, distância no painel e bloqueio frontal experimental |
| **v0.3** | [`v0.3/carrinho_robo_v0_3.ino`](v0.3/carrinho_robo_v0_3.ino) | PWM no L298N, LDR, tema automático claro/escuro, alerta sonoro de ré e bloqueio da ré somente abaixo de 5 cm |

## Estado atual — v0.3

- ESP32 cria a rede Wi-Fi local `tony`;
- senha: `stark369`;
- painel em `http://192.168.4.1`;
- comandos: frente, ré, esquerda, direita e parar;
- velocidade controlada por PWM;
- interface responsiva para celular;
- HC-SR04 mostra a distância em tempo real;
- o ultrassônico funciona como sensor de estacionamento para a ré;
- o alerta sonoro no celular acelera conforme o obstáculo se aproxima;
- abaixo de **5 cm**, a ré é bloqueada;
- a frente continua liberada;
- LDR no GPIO 34 mede a luminosidade;
- tema do painel pode ficar em `AUTO`, `CLARO` ou `ESCURO`;
- no modo `AUTO`, a luminosidade altera o tema do controle;
- o painel mostra percentual de luminosidade e ADC bruto;
- o Serial Monitor registra o LDR aproximadamente a cada 5 segundos.

## Pinagem atual

### L298N

| L298N | ESP32 | Função |
|---|---:|---|
| ENA | GPIO 25 | PWM do lado A |
| IN1 | GPIO 26 | Sentido A |
| IN2 | GPIO 27 | Sentido A |
| ENB | GPIO 33 | PWM do lado B |
| IN3 | GPIO 32 | Sentido B |
| IN4 | GPIO 23 | Sentido B |

Os jumpers `ENA` e `ENB` devem ser removidos quando o PWM é comandado pelo ESP32.

### HC-SR04

| HC-SR04 | ESP32 |
|---|---:|
| VCC | 5 V |
| GND | GND comum |
| TRIG | GPIO 18 |
| ECHO | GPIO 19 através de divisor resistivo |

Divisor utilizado no protótipo com três resistores de 1 kΩ:

```text
ECHO ── 1 kΩ ──┬── GPIO 19
                │
               1 kΩ
                │
               1 kΩ
                │
               GND
```

### LDR

Ligação esperada pelo firmware:

```text
3V3 ── LDR ──┬── GPIO 34
              │
           resistor
              │
             GND
```

O LDR deve trabalhar a partir de **3,3 V**, não de 5 V no GPIO 34.

## Comportamento do sensor de ré

A distância é lida continuamente. O som do navegador funciona como auxílio de estacionamento:

| Distância | Comportamento |
|---:|---|
| acima de 100 cm | sem som |
| 60–100 cm | bipes lentos |
| 40–60 cm | bipes moderados |
| 25–40 cm | bipes rápidos |
| 15–25 cm | bipes mais rápidos |
| 5–15 cm | bipes muito rápidos |
| até 5 cm | som contínuo e ré bloqueada |

O navegador do celular exige uma interação do usuário para liberar áudio; por isso existe o botão **Ativar alerta sonoro**.

## Gravação

1. Abrir [`codigo.ino`](codigo.ino) na Arduino IDE.
2. Instalar/usar `esp32 by Espressif Systems`.
3. Selecionar `ESP32 Dev Module`/`DOIT ESP32 DEVKIT V1` conforme a placa reconhecida.
4. Selecionar a porta do CP210x.
5. Fazer upload.
6. Abrir o Serial Monitor em **115200 baud**.
7. Conectar o celular à rede `tony`.
8. Abrir `http://192.168.4.1`.

Para os primeiros testes, deixar as rodas suspensas e começar com PWM baixo.
