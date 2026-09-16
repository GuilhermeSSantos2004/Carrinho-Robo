# Código-fonte do ESP32

Esta pasta contém o firmware do carrinho-robô. O arquivo [`codigo.ino`](codigo.ino) sempre representa a versão mais recente.

## Versões

| Versão | Arquivo | Alterações principais |
|---:|---|---|
| **v0.2** | [`v0.2/carrinho_robo_v0_2/carrinho_robo_v0_2.ino`](v0.2/carrinho_robo_v0_2/carrinho_robo_v0_2.ino) | Inclusão do HC-SR04, medição da distância, exibição no painel e bloqueio do avanço abaixo de 20 cm |

O `codigo.ino` e o arquivo versionado da `v0.2` possuem o mesmo conteúdo nesta entrega. A cópia versionada preserva o estado exigido pela **Tarefa 18**.

## Recursos implementados

- rede Wi-Fi própria `Carrinho-ESP32`;
- página web disponível em `http://192.168.4.1`;
- comandos avançar, recuar, esquerda, direita e parar;
- controle de velocidade por PWM na L298N;
- leitura do HC-SR04;
- bloqueio de avanço quando há obstáculo a menos de 20 cm;
- parada automática se o celular deixar de enviar comandos.

## Sensor ultrassônico — versão v0.2

| HC-SR04 | ESP32 | Observação |
|---|---:|---|
| VCC | 5 V regulados | Usar a saída do LM2596 |
| GND | GND | Deve ser comum ao ESP32 e à L298N |
| TRIG | GPIO 18 | Pulso de disparo |
| ECHO | GPIO 19 | Usar divisor de tensão com resistores de 1 kΩ e 2 kΩ |

Comportamento implementado:

1. o sensor é consultado a cada 100 ms;
2. a distância aparece na página web;
3. se um obstáculo estiver a menos de 20 cm, o avanço é interrompido;
4. recuar e girar continuam disponíveis para permitir que o carrinho saia do obstáculo.

## Gravação

1. Abrir `codigo.ino` ou `v0.2/carrinho_robo_v0_2/carrinho_robo_v0_2.ino` na Arduino IDE com o pacote **ESP32 by Espressif Systems 3.x**.
2. Selecionar `ESP32 Dev Module` e a porta COM da placa.
3. Enviar o código.
4. Conectar o celular à rede `Carrinho-ESP32`, senha `carrinho123`.
5. Manter a conexão mesmo se o celular informar que a rede está sem internet.
6. Abrir `http://192.168.4.1`.

Fazer o primeiro teste com as rodas suspensas e conferir o esquema em [`hardware/arquitetura`](../hardware/arquitetura/README.md).
