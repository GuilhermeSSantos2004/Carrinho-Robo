# Código-fonte do ESP32

O arquivo [`codigo.ino`](codigo.ino) contém o firmware do carrinho.

## Recursos implementados

- rede Wi-Fi própria `Carrinho-ESP32`;
- página web disponível em `http://192.168.4.1`;
- comandos avançar, recuar, esquerda, direita e parar;
- controle de velocidade por PWM na L298N;
- leitura do HC-SR04;
- bloqueio de avanço quando há obstáculo a menos de 20 cm;
- parada automática se o celular deixar de enviar comandos.

## Gravação

1. Abrir `codigo.ino` na Arduino IDE com o pacote **ESP32 by Espressif Systems 3.x**.
2. Selecionar `ESP32 Dev Module` e a porta COM da placa.
3. Enviar o código.
4. Conectar o celular à rede `Carrinho-ESP32`, senha `carrinho123`.
5. Manter a conexão mesmo se o celular informar que a rede está sem internet.
6. Abrir `http://192.168.4.1`.

Fazer o primeiro teste com as rodas suspensas e conferir o esquema em [`hardware/arquitetura`](../hardware/arquitetura/README.md).

