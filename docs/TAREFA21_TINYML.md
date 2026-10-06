# Tarefa 21 — Inferência TinyML no ESP32

## Objetivo

Implementar inferência TinyML no ESP32 usando os três sensores ultrassônicos frontais do CUBI-04.

O firmware lê:

- HC-SR04 frente esquerda: TRIG GPIO 16 / ECHO GPIO 17;
- HC-SR04 frente centro: TRIG GPIO 18 / ECHO GPIO 19;
- HC-SR04 frente direita: TRIG GPIO 4 / ECHO GPIO 36 (VP).

O sensor traseiro não participa deste modelo. Ele continua disponível para o firmware principal do carrinho.

## Classes

| Label | Classe | Interpretação |
|---:|---|---|
| 0 | LIVRE | os três sensores indicam espaço frontal suficiente |
| 1 | OBSTACULO_ESQUERDA | obstáculo predominante à esquerda |
| 2 | OBSTACULO_CENTRO | obstáculo predominante no centro |
| 3 | OBSTACULO_DIREITA | obstáculo predominante à direita |

## Dataset

Arquivo obrigatório da entrega:

- `docs/dataset.csv`

Colunas:

```text
esquerda_cm,centro_cm,direita_cm,label
```

O dataset contém **240 amostras sintéticas realistas**, balanceadas em **60 exemplos por classe**.

Os valores foram gerados para representar cenários coerentes com o uso dos HC-SR04 no carrinho:

- leituras aproximadamente entre 5 e 120 cm;
- classe LIVRE com sensores predominantemente acima de ~30–35 cm;
- classes de obstáculo com o sensor correspondente tipicamente abaixo de ~27 cm;
- pequenas variações adicionadas para representar ruído de medição.

> **Transparência:** estes dados não foram capturados fisicamente pelo carrinho. Eles são dados sintéticos usados para permitir o treinamento e a demonstração da inferência. O vídeo exigido pela atividade deve mostrar a validação física no ESP32.

## Modelo treinado

Arquivo:

- `model/model.h`

Foi treinado um classificador de **regressão logística multinomial com Softmax**.

Entradas:

```text
x0 = esquerda_cm / 150
x1 = centro_cm / 150
x2 = direita_cm / 150
```

Saídas:

```text
P(LIVRE)
P(OBSTACULO_ESQUERDA)
P(OBSTACULO_CENTRO)
P(OBSTACULO_DIREITA)
```

Divisão utilizada:

- 75% das amostras para treinamento;
- 25% para teste;
- divisão estratificada entre as quatro classes.

Resultado no conjunto sintético de teste:

```text
Acurácia: 100%
Matriz de confusão:

15  0  0  0
 0 15  0  0
 0  0 15  0
 0  0  0 15
```

Esse resultado mostra que o modelo aprendeu a separação do dataset sintético. **Não significa 100% de acurácia no mundo real**; a validação física depende da geometria do carrinho, ruído, material do obstáculo e posicionamento dos sensores.

## Inferência no ESP32

Sketch obrigatório:

- `src/tinyml_inferencia.ino`

O código:

1. lê os três HC-SR04;
2. realiza três medições por sensor e calcula a média;
3. rejeita leitura sem eco;
4. limita a faixa usada pelo modelo;
5. normaliza as três entradas;
6. calcula os logits do modelo;
7. aplica Softmax;
8. exibe classe, confiança e probabilidades no Serial Monitor.

O modelo foi exportado como pesos em `model.h`, portanto a inferência roda diretamente no ESP32 e não precisa de conexão com internet.

## Como testar

1. Mantenha a ponte H/motores desligados durante o primeiro teste.
2. Para compilar no Arduino IDE, crie uma pasta chamada `tinyml_inferencia` e coloque nela:
   - `src/tinyml_inferencia.ino`;
   - uma cópia de `model/model.h`.
3. Abra `tinyml_inferencia.ino` nessa pasta e grave no ESP32.
4. Abra o Serial Monitor em **115200 baud**.
5. Faça quatro situações:
   - sem obstáculo próximo na frente;
   - obstáculo a aproximadamente 10–20 cm do sensor esquerdo;
   - obstáculo a aproximadamente 10–20 cm do sensor central;
   - obstáculo a aproximadamente 10–20 cm do sensor direito.
6. Observe a mudança da classe e da confiança.

Exemplo esperado:

```text
---------- INFERENCIA TINYML ----------
ESQ: 74.1 cm | CENTRO: 13.8 cm | DIR: 80.2 cm
CLASSE: OBSTACULO_CENTRO | CONFIANCA: 99.8%
ACAO SUGERIDA: PARAR / ESCOLHER LADO LIVRE
---------------------------------------
```

## Roteiro do vídeo da entrega

Sugestão de vídeo curto:

1. mostrar fisicamente o carrinho e os três HC-SR04 frontais;
2. mostrar o Serial Monitor;
3. deixar a frente livre e mostrar `LIVRE`;
4. aproximar um objeto do sensor esquerdo e mostrar `OBSTACULO_ESQUERDA`;
5. repetir no centro;
6. repetir na direita;
7. mostrar a confiança mudando em tempo real.

## Estrutura da entrega

```text
Carrinho-Robo/
├── src/
│   └── tinyml_inferencia.ino
├── model/
│   └── model.h
└── docs/
    └── dataset.csv
```

Os arquivos antigos do CUBI-04 foram mantidos. O firmware principal continua em `src/codigo.ino`.