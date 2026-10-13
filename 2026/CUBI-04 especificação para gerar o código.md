# CUBI-04: especificação para gerar o código

Oct 1, 2026 · @Guilherme Silva dos Santos

## Visão geral

O CUBI-04 é um carro 4×4 com direção tipo tanque: 4 motores TT, 2 por lado ligados em paralelo, controlados por um ESP32 de 30 pinos e uma ponte H L298N.

O código precisa ter **dois modos**, trocados pelo celular:

- **Automático:** o carro anda sozinho e desvia de obstáculos com 4 sensores ultrassônicos.
- **Manual:** o carro é dirigido por um joystick numa página web servida pelo próprio ESP32.

Nos dois modos, o ESP32 envia **telemetria** para o celular: posição estimada (encoders + giroscópio) e as 4 distâncias, para desenhar um mapa 2D do ambiente.

## Sistema de coordenadas e medidas gerais

Todas as posições deste documento usam o mesmo referencial, em milímetros:

- **Origem:** no chão, no centro do carro (no meio entre os 4 eixos).
- **X:** para a frente. **Y:** para a esquerda. **Z:** para cima.
- **Ângulos:** 0° = para a frente; positivo = girando para a esquerda (anti-horário visto de cima).

| Medida | Valor |
| --- | --- |
| Comprimento total, das rodas de trás até a face do sensor da frente | 185 mm (X de −90,5 a +94) |
| Largura total, por fora das rodas | 184 mm (Y de −92 a +92) |
| Distância entre eixos | 113 mm (eixos em X = ±56,5) |
| Bitola, de centro a centro das rodas | 159 mm (rodas em Y = ±79,5) |
| Altura do eixo das rodas | 34 mm |
| Vão livre embaixo do carro | 14 mm |
| Topo dos suportes dos sensores | 100 mm |
| Peso estimado | cerca de 600 g (aproximado) |

## Motores, rodas e ponte H

Os 2 motores de cada lado estão em paralelo na mesma saída da ponte H, então o código controla só **2 canais: lado esquerdo e lado direito**.

| Item | Posição / ligação | Detalhes |
| --- | --- | --- |
| 4 rodas | Centros em X = ±56,5, Y = ±79,5, Z = 34 | Ø68 × 25 mm, circunferência 213,6 mm |
| 4 motores TT 48:1 | Pendurados nas torres, eixo duplo | 3 a 6 V; 90 RPM a 3 V, 200 RPM a 6 V; 150 a 200 mA sem carga |
| Lado esquerdo (2 motores) | Saída OUT1/OUT2 | ENA = GPIO 25 (PWM), IN1 = GPIO 26, IN2 = GPIO 27 |
| Lado direito (2 motores) | Saída OUT3/OUT4 | ENB = GPIO 32 (PWM), IN3 = GPIO 14, IN4 = GPIO 33 |
| Ponte H L298N | Traseira da placa superior | Alimentada pela bateria 3S; regulador de 5 V ligado (jumper) |

Regras para o código:

- **PWM no máximo 55%** (≈140 de 255 em 8 bits). A bateria tem até 12,6 V e a L298N perde uns 2 V; acima disso os motores passam de 6 V.
- **Frequência do PWM:** 1 kHz, 8 bits.
- **Velocidade estimada:** a 55% ≈ 180 RPM ≈ 0,64 m/s. Para andar desviando, usar 35 a 45%.
- **PWM mínimo:** abaixo de um certo valor os motores não vencem o atrito. Deixar uma constante `PWM_MIN` para calibrar (começar em 25%).
- **Sentido dos motores:** o sentido de cada lado pode sair invertido na montagem. Deixar constantes `INVERTE_ESQ` e `INVERTE_DIR`.
- **Curvas:** girar no lugar = um lado para a frente e o outro para trás; curva suave = os dois lados para a frente com velocidades diferentes.

## Sensores ultrassônicos (HC-SR04)

São 4 sensores: os 3 da frente decidem o desvio e o de trás só libera a ré. A posição é a da **face dos olhos**, de onde a distância é medida.

| Sensor | Face (X, Y, Z) em mm | Aponta para | TRIG | ECHO |
| --- | --- | --- | --- | --- |
| Frente | (94, 0, 87) | 0° | GPIO 5 | GPIO 18 |
| Esquerdo | (70, +54, 87) | +50° | GPIO 19 | GPIO 23 |
| Direito | (70, −54, 87) | −50° | GPIO 4 | GPIO 13 |
| Traseiro | (−86, 0, 87) | 180° | GPIO 16 | GPIO 17 |

Como ligar e ler:

- **Alimentação:** 5 V. O pino **ECHO passa por um divisor** de 1 kΩ + 2 kΩ antes do ESP32. O TRIG liga direto.
- **Leitura:** um sensor de cada vez, com **pelo menos 30 ms** entre um disparo e outro, para um não ouvir o eco do outro. Timeout de 25 ms (≈4 m). Distância em cm = tempo do pulso em µs / 58.
- **Cone:** cerca de ±15° de abertura; alcance de 2 a 400 cm.

Geometria que o código deve considerar:

- **Frente:** a face do sensor do meio é o ponto mais à frente do carro, 3,5 mm à frente das rodas. A distância lida já é a folga até o obstáculo.
- **Laterais:** a face fica 38 mm para dentro da borda das rodas. O feixe só sai da largura do carro depois de uns 5 cm, então leituras laterais menores que isso não indicam a folga real do lado.
- **Altura:** o centro dos olhos fica a 87 mm do chão. Com o cone de 15°, a 20 cm o sensor enxerga até uns 33 mm do chão; a 10 cm, só acima de 60 mm. Obstáculos baixos e perto podem não aparecer.

## Encoders (LM393)

São 2 encoders ópticos, só nos motores da frente, um de cada lado. Cada um dá **12 pulsos por volta da roda**, ou seja, **17,8 mm andados por pulso**.

| Encoder | Posição aproximada (X, Y, Z) em mm | Pino | Alimentação |
| --- | --- | --- | --- |
| Esquerdo | (65, +35, 30) | GPIO 34 | 3,3 V |
| Direito | (65, −35, 30) | GPIO 35 | 3,3 V |

- **Disco:** Ø15 mm com 12 fendas, preso no eixo do motor, que é o mesmo eixo da roda.
- **Contagem:** interrupção na borda de subida (`RISING`) = 12 pulsos por volta. Contando as duas bordas (`CHANGE`) = 24 pulsos, ou 8,9 mm por pulso.
- **Pinos 34 e 35:** só servem como entrada e não têm pull-up interno. O módulo LM393 já tem o dele.
- **Ruído:** ignorar pulsos com menos de 1 ms entre si. Na velocidade máxima são uns 40 pulsos por segundo (25 ms entre eles).
- **Sentido:** o encoder não sabe se a roda gira para a frente ou para trás. Somar ou subtrair conforme o sentido que o código mandou para aquele lado.
- **Patinação:** girando no lugar, as rodas escorregam e o encoder erra. Usar os encoders só para a **distância** e o giroscópio para o **ângulo**.

## Giroscópio e acelerômetro (MPU6050)

O MPU6050 fica perto do centro do carro, deitado na placa superior: **(X −2, Y +28, Z 63) mm**. O giroscópio dá o ângulo do carro (rumo) e o acelerômetro detecta batida e tombo.

| Item | Valor |
| --- | --- |
| Orientação | Eixo X do módulo para a frente do carro, eixo Z para cima |
| Ligação | I2C: SDA = GPIO 21, SCL = GPIO 22, 400 kHz |
| Endereço | 0x68 (pino AD0 em GND) |
| Alimentação | 3,3 V |

- **Rumo (θ):** integrar o giro do eixo Z. Conferir o sinal: girar o carro para a esquerda tem que **aumentar** o ângulo.
- **Calibração:** com o carro parado, nos primeiros 2 segundos, tirar a média de umas 500 leituras do giro Z e descontar sempre (bias).
- **Configuração:** faixa de ±250 °/s e filtro interno (DLPF) entre 20 e 44 Hz, para filtrar a vibração dos motores.
- **Batida:** variação brusca da aceleração acima de 1,5 g → parar os motores.
- **Tombo:** inclinação acima de 35° → parar os motores.

## Energia, ESP32 e pinos

A bateria 3S alimenta tudo: ela passa pela chave liga/desliga, vai para os 12 V da L298N, e o regulador de 5 V da L298N alimenta o ESP32 e os sensores.

- **Bateria:** 3 × 18650 em série: 11,1 V nominal, 12,6 V cheia. Não deixar descer de uns 9,9 V (3,3 V por célula).
- **5 V (regulador da L298N):** VIN do ESP32 e os 4 HC-SR04.
- **3,3 V (do ESP32):** os 2 LM393 e o MPU6050.
- **GND:** todos ligados juntos.
- **Em aberto:** hoje não há medição da tensão da bateria. Dá para adicionar depois um divisor de tensão num pino de entrada analógica livre (por exemplo, GPIO 36).

Tabela completa de pinos do ESP32 (30 pinos):

| Função | GPIO | Tipo |
| --- | --- | --- |
| ENA, motores da esquerda (velocidade) | 25 | Saída PWM |
| IN1, motores da esquerda | 26 | Saída |
| IN2, motores da esquerda | 27 | Saída |
| ENB, motores da direita (velocidade) | 32 | Saída PWM |
| IN3, motores da direita | 14 | Saída |
| IN4, motores da direita | 33 | Saída |
| TRIG sensor da frente | 5 | Saída |
| ECHO sensor da frente | 18 | Entrada (com divisor) |
| TRIG sensor esquerdo | 19 | Saída |
| ECHO sensor esquerdo | 23 | Entrada (com divisor) |
| TRIG sensor direito | 4 | Saída |
| ECHO sensor direito | 13 | Entrada (com divisor) |
| TRIG sensor traseiro | 16 | Saída |
| ECHO sensor traseiro | 17 | Entrada (com divisor) |
| SDA do MPU6050 | 21 | I2C |
| SCL do MPU6050 | 22 | I2C |
| Encoder esquerdo | 34 | Entrada (só entrada) |
| Encoder direito | 35 | Entrada (só entrada) |
| LED de status (LED da placa) | 2 | Saída |

## Comportamento esperado

O carro liga **parado e em modo manual**. A troca de modo é feita pelo celular, e o botão **Parar** funciona nos dois modos.

### Modo manual

- **Comando:** joystick com X e Y de −1 a +1, e um controle de velocidade de 0 a 100%.
- **Mistura para tanque:** esquerda = Y + X, direita = Y − X, limitando cada lado entre −1 e +1 e depois multiplicando pela velocidade e pelo PWM máximo.
- **Segurança:** sem comando do celular por 500 ms → parar. Mesmo no manual, bloquear ir para a frente com o sensor da frente abaixo de 15 cm e dar ré com o traseiro abaixo de 15 cm.

### Modo automático

Cada estado pode ir para outro conforme as distâncias:

1. **Andar:** em frente a 40% enquanto a frente tiver mais de 40 cm livres. Se um sensor lateral ler menos de 15 cm, fazer uma curva suave para o outro lado, sem parar.
2. **Obstáculo na frente:** frente abaixo de 20 cm → parar e comparar o sensor esquerdo com o direito.
3. **Girar:** girar no lugar para o lado com mais espaço, usando o giroscópio para saber quanto já virou, até a frente passar de 40 cm. Limite de 180°.
4. **Tudo bloqueado:** frente, esquerda e direita abaixo de 20 cm. Se o traseiro tiver mais de 20 cm, dar **ré devagar** por até 15 cm (contados pelos encoders) ou até o traseiro baixar de 20 cm. Depois girar 180° pelo giroscópio e voltar ao estado 1.
5. **Cercado:** se o traseiro também estiver abaixo de 20 cm, parar e piscar o LED. Tentar de novo a cada 2 segundos.

### Segurança nos dois modos

- Batida (acelerômetro acima de 1,5 g) ou tombo (mais de 35°) → parar e voltar para o modo manual.
- PWM nunca acima de 55%.
- Perdeu a conexão com o celular no modo manual → parar. No automático, continua sozinho.

### Telemetria e comandos

O ESP32 envia **10 vezes por segundo**, por WebSocket, a posição estimada e as leituras. Posição: θ vem do giroscópio, a distância andada é a média dos dois encoders, e a cada atualização x += d · cos θ e y += d · sen θ.

```json
{"modo":"auto","estado":"andar","x":1250,"y":-340,"th":37.5,"d":{"frente":82,"esq":45,"dir":120,"tras":200},"enc":{"esq":412,"dir":405},"pwm":{"esq":102,"dir":102}}
```

Distâncias em cm, x e y em mm, θ em graus.

Comandos que o celular manda:

```json
{"cmd":"modo","valor":"auto"}
{"cmd":"dirigir","x":0.2,"y":0.8}
{"cmd":"velocidade","valor":0.4}
{"cmd":"parar"}
```

**Mapa:** o celular desenha. Cada leitura vira um ponto no mapa usando a posição do carro + a posição e o ângulo do sensor (tabela dos ultrassônicos). Leituras acima de 200 cm são descartadas.

## Requisitos para o código

Pedido para o agente: gerar **um único sketch para Arduino IDE** que faça tudo o que está acima, pronto para gravar no ESP32.

- **Plataforma:** ESP32 Arduino core 3.x. Usar a API nova de PWM (`ledcAttach` e `ledcWrite` por pino).
- **Wi-Fi:** modo ponto de acesso, com rede **"CUBI-04"** e página em **192.168.4.1**. Servidor web e WebSocket (por exemplo, ESPAsyncWebServer ou WebServer + WebSocketsServer). Dizer quais bibliotecas instalar.
- **Página no celular:** HTML embutido no próprio código, leve e feito para tela de celular, com:
  - joystick por toque e controle de velocidade;
  - botões Manual, Automático e Parar;
  - as 4 distâncias ao vivo;
  - um mapa 2D em canvas, desenhado com a posição do carro e os pontos dos sensores.
- **Sem travar:** nada de `delay()` longo; tudo com `millis()`. Sensores ultrassônicos lidos em sequência, um a cada 30 ms.
- **Encoders por interrupção** (`attachInterrupt`, borda `RISING`).
- **Constantes no topo do código**, fáceis de ajustar: `PWM_MAX`, `PWM_MIN`, velocidades, distâncias de parada e desvio, `INVERTE_ESQ`, `INVERTE_DIR`, mm por pulso e timeouts.
- **Organização:** funções separadas para motores, ultrassônicos, encoders, MPU6050, posição (odometria), página e WebSocket, e a máquina de estados do modo automático.
- **Comentários em português**, explicando cada parte.
- **Monitor serial a 115200:** imprimir as leituras e o estado, para facilitar os primeiros testes.
