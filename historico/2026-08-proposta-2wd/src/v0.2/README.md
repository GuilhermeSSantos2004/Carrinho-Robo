# Firmware v0.2 — Tarefa 18

Esta versão acrescenta o sensor ultrassônico HC-SR04 ao controle Wi-Fi do carrinho.

## Arquivo para Arduino IDE

[`carrinho_robo_v0_2/carrinho_robo_v0_2.ino`](carrinho_robo_v0_2/carrinho_robo_v0_2.ino)

A pasta e o arquivo possuem o mesmo nome para manter a estrutura esperada pela Arduino IDE.

## Alterações da v0.2

- leitura do HC-SR04 a cada 100 ms;
- exibição da distância frontal na interface web;
- bloqueio do comando de avanço abaixo de 20 cm;
- parada imediata se um obstáculo surgir enquanto o carrinho avança;
- manutenção dos comandos de recuo e giro para sair do obstáculo;
- versão informada na página, na API de status e no Monitor Serial.

## Critério de teste

Com as rodas suspensas, aproximar um objeto da frente do sensor. A página deve mostrar a distância e o comando de avanço deve permanecer bloqueado abaixo de 20 cm.
