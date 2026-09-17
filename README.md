# Carrinho-robô com ESP32 — CUBI-04

Projeto da disciplina **Project-based Maker Lab — FIAP**, desenvolvido pelo grupo **Start-up One**.

## Integrantes

| Nome | RM |
|---|---|
| Enricco Rossi de Souza Carvalho Miranda | RM551717 |
| Gabriel Marquez Trevisan | RM99227 |
| Guilherme Silva dos Santos | RM551168 |
| Danilo Urze Aldred | RM99465 |
| Laura Claro Mathias | RM98747 |

## Objetivo

Desenvolver um carrinho-robô funcional integrando projeto mecânico, fabricação digital, eletrônica, programação, integração e documentação. O controle é feito pelo celular através de uma página web criada pelo próprio ESP32, sem depender de internet ou aplicativo externo.

## Estado atual do projeto

O conceito inicial de 2 motores evoluiu para uma montagem própria com **4 motores TT**, estrutura impressa em 3D e novos recursos de software.

### Recursos implementados

- ESP32 DevKit V1;
- rede Wi-Fi própria criada pelo ESP32;
- controle por celular em `http://192.168.4.1`;
- comandos de frente, ré, esquerda, direita e parada;
- quatro motores TT, dois por lado;
- ponte H dupla L298N;
- controle de velocidade por **PWM**;
- sensor ultrassônico HC-SR04;
- sensor de estacionamento para a ré;
- bipes progressivos no celular conforme o obstáculo se aproxima;
- bloqueio da ré em **5 cm ou menos**;
- frente permanece liberada;
- LDR no GPIO 34;
- detecção de luminosidade com indicação de DIA / MEIA-LUZ / NOITE;
- temas `AUTO`, `CLARO` e `ESCURO` no painel;
- logs periódicos do LDR no Serial Monitor;
- CAD próprio para chassi, carcaça e suportes dos motores.

## Firmware atual — v0.3

O arquivo principal é [`src/codigo.ino`](src/codigo.ino). A versão congelada está em [`src/v0.3/carrinho_robo_v0_3.ino`](src/v0.3/carrinho_robo_v0_3.ino).

### Wi-Fi

| Item | Valor |
|---|---|
| SSID | `tony` |
| Senha | `stark369` |
| IP | `192.168.4.1` |
| Serial | `115200 baud` |

### Pinagem do L298N

| L298N | ESP32 | Função |
|---|---:|---|
| ENA | GPIO 25 | PWM canal A |
| IN1 | GPIO 26 | direção A |
| IN2 | GPIO 27 | direção A |
| ENB | GPIO 33 | PWM canal B |
| IN3 | GPIO 32 | direção B |
| IN4 | GPIO 23 | direção B |
| GND | GND | referência comum |

Os jumpers `ENA` e `ENB` devem ser removidos para o controle PWM.

### Ultrassônico HC-SR04

| HC-SR04 | ESP32 |
|---|---:|
| VCC | 5 V |
| GND | GND comum |
| TRIG | GPIO 18 |
| ECHO | GPIO 19 com divisor resistivo |

O divisor utilizado com os resistores disponíveis é:

```text
ECHO ── 1 kΩ ──┬── GPIO 19
                │
               1 kΩ
                │
               1 kΩ
                │
               GND
```

O HC-SR04 funciona atualmente como **sensor de estacionamento de ré**. O firmware não bloqueia mais a frente a 20 cm. Durante a ré, os bipes aceleram conforme a distância cai e a ré é interrompida a `<= 5 cm`.

### LDR

O LDR está ligado ao **GPIO 34**:

```text
3V3 ── LDR ──┬── GPIO 34
              │
           resistor
              │
             GND
```

O firmware faz média das leituras, mostra o ADC bruto e grava um log aproximadamente a cada 5 segundos. No modo `AUTO`, o valor medido decide se o painel usa fundo claro ou escuro.

## Alimentação atual

A montagem atual usa um suporte com **3 células 18650 em série**:

- aproximadamente **11,1 V nominal**;
- até aproximadamente **12,6 V** com as células totalmente carregadas;
- pack ligado à entrada de potência da L298N;
- ESP32 alimentado por USB/power bank durante os testes;
- todos os GNDs interligados.

> **Atenção:** os motores TT utilizados são 3–6 V. O pack 3S ultrapassa a tensão especificada dos motores. O PWM reduzido é útil nos testes, mas não substitui uma fonte/regulador adequado aos motores. Não aplicar 11,1–12,6 V diretamente ao ESP32 ou a GPIOs.

## CAD e fabricação 3D

O projeto mecânico passou por várias revisões até chegar ao conceito atual.

| Peça | Estado atual |
|---|---|
| T01 — base/chassi | 147 × 140 × 10 mm, otimizado |
| T02 — carcaça | 140 × 133 × 24 mm, baixa e com aberturas dos suportes |
| T03A — suporte de motor | v7 |
| T03B — presilha | v7 |
| T04 — tampa | 2 mm, não obrigatória no conceito atual |
| T05 — trava | pino vertical com nervuras |
| T06 — pino/parafuso impresso | 8 previstos, 2 por motor |

As principais mudanças mecânicas incluíram redução do chassi, remoção de pedestais, reposicionamento dos motores, criação de porcas cativas, correções de colisão, eliminação de peças soltas na carcaça e redução de material de impressão.

Mais detalhes: [`cad/README.md`](cad/README.md).

## Dificuldades e evolução

O desenvolvimento foi iterativo. Entre os problemas reais encontrados estão:

- suporte do motor com folga e região frontal frágil;
- cortes CAD feitos fora do material e que aparentemente passavam despercebidos;
- peças com malha válida, mas divididas em vários corpos soltos;
- pedestais e apoios ultrapassando a área real do chassi;
- carcaça gerando aproximadamente 45 minutos extras de suporte no fatiador;
- premissas erradas sobre a área ocupada pelos suportes dos motores;
- colisão de reforços com a guia da bateria;
- várias revisões das dimensões do encaixe do ultrassônico;
- mudança da lógica do HC-SR04 de bloqueio frontal para sensor de ré;
- necessidade de expor o ADC bruto do LDR para calibrar o sensor real;
- mudança da alimentação em relação à proposta inicial.

O histórico completo está em [`historico/HISTORICO_PROJETO_CUBI04.md`](historico/HISTORICO_PROJETO_CUBI04.md).

## Como testar

1. Conferir toda a fiação com a alimentação desligada.
2. Alimentar o ESP32 por USB/power bank.
3. Abrir `src/codigo.ino` na Arduino IDE.
4. Selecionar `ESP32 Dev Module`/`DOIT ESP32 DEVKIT V1` e a porta CP210x.
5. Fazer upload.
6. Abrir o Serial Monitor em 115200 baud.
7. Conectar o celular ao Wi-Fi `tony` com senha `stark369`.
8. Abrir `http://192.168.4.1`.
9. Ativar o alerta sonoro no próprio painel — navegadores exigem interação do usuário para liberar áudio.
10. Testar inicialmente com as rodas suspensas e PWM baixo.

## Estrutura do repositório

```text
Carrinho-Robo/
├── README.md
├── cad/
│   ├── STL/
│   ├── fonte-modelo/
│   └── versoes/
├── docs/
├── hardware/
│   ├── arquitetura/
│   └── componentes/
├── historico/
│   └── HISTORICO_PROJETO_CUBI04.md
├── organizacao/
└── src/
    ├── codigo.ino
    ├── v0.2/
    └── v0.3/
```

## Status do Check Point 01

O repositório registra a evolução do projeto até o protótipo atual, incluindo mecânica, eletrônica, firmware, histórico de dificuldades e decisões de projeto. Próximas validações físicas devem registrar no histórico qualquer alteração de encaixe, alimentação, calibração do LDR ou comportamento do sensor ultrassônico.
