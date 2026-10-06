#include <Arduino.h>
#include <math.h>
#include "model.h"

// =====================================================
// CUBI-04 - Tarefa 21 - Inferencia TinyML no ESP32
//
// Usa os 3 HC-SR04 frontais:
//   esquerda -> TRIG 16 / ECHO 17
//   centro   -> TRIG 18 / ECHO 19
//   direita  -> TRIG 4  / ECHO 36 (VP)
//
// IMPORTANTE:
// Cada ECHO deve chegar ao ESP32 pelo divisor resistivo.
// O modelo usa 3 distancias e classifica:
//   0 LIVRE
//   1 OBSTACULO_ESQUERDA
//   2 OBSTACULO_CENTRO
//   3 OBSTACULO_DIREITA
//
// Para esta atividade o sketch NAO aciona os motores.
// Isso deixa a demonstracao da inferencia isolada e segura.
//
// Arduino IDE:
// copie model/model.h para a MESMA pasta do sketch antes de compilar.
// O repositorio mantem o model.h em /model porque essa e a estrutura
// exigida na entrega.
// =====================================================

// Frente esquerda
#define TRIG_ESQ 16
#define ECHO_ESQ 17

// Frente centro
#define TRIG_CENTRO 18
#define ECHO_CENTRO 19

// Frente direita
#define TRIG_DIR 4
#define ECHO_DIR 36

const float DIST_MIN_CM = 2.0f;
const float DIST_MAX_CM = 150.0f;

struct ResultadoInferencia {
  int classe;
  float confianca;
  float probabilidades[MODEL_NUM_CLASSES];
};


// =====================================================
// HC-SR04
// =====================================================

float medirUmaVez(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(3);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  unsigned long duracao =
    pulseIn(echo, HIGH, 25000);

  if (duracao == 0) {
    return -1.0f;
  }

  float distancia =
    duracao * 0.0343f / 2.0f;

  if (
    distancia < DIST_MIN_CM ||
    distancia > 400.0f
  ) {
    return -1.0f;
  }

  return distancia;
}


float medirMedia(
  int trig,
  int echo,
  int amostras = 3
) {
  float soma = 0.0f;
  int validas = 0;

  for (int i = 0; i < amostras; i++) {
    float d =
      medirUmaVez(trig, echo);

    if (d > 0) {
      soma += d;
      validas++;
    }

    // Reduz interferencia entre ecos.
    delay(45);
  }

  if (validas == 0) {
    return -1.0f;
  }

  return soma / validas;
}


float limitarDistancia(float distancia) {
  if (distancia < DIST_MIN_CM) {
    return DIST_MIN_CM;
  }

  if (distancia > DIST_MAX_CM) {
    return DIST_MAX_CM;
  }

  return distancia;
}


// =====================================================
// TINYML - SOFTMAX
// =====================================================

ResultadoInferencia inferir(
  float esquerdaCm,
  float centroCm,
  float direitaCm
) {
  ResultadoInferencia resultado;

  float entrada[MODEL_NUM_FEATURES] = {
    limitarDistancia(esquerdaCm) /
      MODEL_NORMALIZATION_CM,

    limitarDistancia(centroCm) /
      MODEL_NORMALIZATION_CM,

    limitarDistancia(direitaCm) /
      MODEL_NORMALIZATION_CM
  };

  float logits[MODEL_NUM_CLASSES];

  for (int c = 0; c < MODEL_NUM_CLASSES; c++) {
    float valor =
      MODEL_BIAS[c];

    for (int f = 0; f < MODEL_NUM_FEATURES; f++) {
      valor +=
        MODEL_WEIGHTS[c][f] *
        entrada[f];
    }

    logits[c] = valor;
  }

  // Softmax numericamente estavel.
  float maxLogit =
    logits[0];

  for (int c = 1; c < MODEL_NUM_CLASSES; c++) {
    if (logits[c] > maxLogit) {
      maxLogit = logits[c];
    }
  }

  float somaExp = 0.0f;

  for (int c = 0; c < MODEL_NUM_CLASSES; c++) {
    resultado.probabilidades[c] =
      expf(logits[c] - maxLogit);

    somaExp +=
      resultado.probabilidades[c];
  }

  resultado.classe = 0;
  resultado.confianca = 0.0f;

  for (int c = 0; c < MODEL_NUM_CLASSES; c++) {
    resultado.probabilidades[c] /=
      somaExp;

    if (
      resultado.probabilidades[c] >
      resultado.confianca
    ) {
      resultado.confianca =
        resultado.probabilidades[c];

      resultado.classe = c;
    }
  }

  return resultado;
}


// =====================================================
// ACAO SUGERIDA
// =====================================================

const char* acaoSugerida(int classe) {
  switch (classe) {
    case 0:
      return "SEGUIR EM FRENTE";

    case 1:
      return "DESVIAR PARA DIREITA";

    case 2:
      return "PARAR / ESCOLHER LADO LIVRE";

    case 3:
      return "DESVIAR PARA ESQUERDA";

    default:
      return "INDEFINIDA";
  }
}


// =====================================================
// SETUP
// =====================================================

void setup() {
  Serial.begin(115200);
  delay(1200);

  pinMode(TRIG_ESQ, OUTPUT);
  pinMode(ECHO_ESQ, INPUT);

  pinMode(TRIG_CENTRO, OUTPUT);
  pinMode(ECHO_CENTRO, INPUT);

  pinMode(TRIG_DIR, OUTPUT);
  pinMode(ECHO_DIR, INPUT);

  digitalWrite(TRIG_ESQ, LOW);
  digitalWrite(TRIG_CENTRO, LOW);
  digitalWrite(TRIG_DIR, LOW);

  Serial.println();
  Serial.println(
    "========================================"
  );

  Serial.println(
    "   CUBI-04 - TINYML - TAREFA 21"
  );

  Serial.println(
    "========================================"
  );

  Serial.println(
    "Modelo carregado: softmax 3 entradas / 4 classes"
  );

  Serial.println(
    "Sensores: HC-SR04 ESQ + CENTRO + DIR"
  );

  Serial.println(
    "Serial Monitor: 115200 baud"
  );

  Serial.println();
  Serial.println(
    "Classes:"
  );

  for (int i = 0; i < MODEL_NUM_CLASSES; i++) {
    Serial.print("  ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.println(MODEL_LABELS[i]);
  }

  Serial.println();
  Serial.println(
    "Iniciando inferencia..."
  );

  Serial.println(
    "========================================"
  );
}


// =====================================================
// LOOP
// =====================================================

void loop() {
  // Mede sequencialmente para reduzir interferencia.
  float esquerda =
    medirMedia(
      TRIG_ESQ,
      ECHO_ESQ
    );

  delay(45);

  float centro =
    medirMedia(
      TRIG_CENTRO,
      ECHO_CENTRO
    );

  delay(45);

  float direita =
    medirMedia(
      TRIG_DIR,
      ECHO_DIR
    );

  // Se qualquer sensor falhar, nao inventa entrada.
  if (
    esquerda < 0 ||
    centro < 0 ||
    direita < 0
  ) {
    Serial.println();
    Serial.println(
      "[ERRO] Falha de leitura em um dos HC-SR04"
    );

    Serial.print("ESQ: ");
    Serial.print(esquerda, 1);

    Serial.print(" | CENTRO: ");
    Serial.print(centro, 1);

    Serial.print(" | DIR: ");
    Serial.println(direita, 1);

    Serial.println(
      "Revise alimentacao, ECHO/divisor e posicao do alvo."
    );

    delay(700);
    return;
  }

  ResultadoInferencia resultado =
    inferir(
      esquerda,
      centro,
      direita
    );

  Serial.println();
  Serial.println(
    "---------- INFERENCIA TINYML ----------"
  );

  Serial.print("ESQ: ");
  Serial.print(esquerda, 1);
  Serial.print(" cm");

  Serial.print(" | CENTRO: ");
  Serial.print(centro, 1);
  Serial.print(" cm");

  Serial.print(" | DIR: ");
  Serial.print(direita, 1);
  Serial.println(" cm");

  Serial.print("CLASSE: ");
  Serial.print(
    MODEL_LABELS[
      resultado.classe
    ]
  );

  Serial.print(" | CONFIANCA: ");
  Serial.print(
    resultado.confianca * 100.0f,
    1
  );

  Serial.println("%");

  Serial.print("ACAO SUGERIDA: ");
  Serial.println(
    acaoSugerida(
      resultado.classe
    )
  );

  Serial.print("PROBABILIDADES: ");

  for (int c = 0; c < MODEL_NUM_CLASSES; c++) {
    Serial.print(
      MODEL_LABELS[c]
    );

    Serial.print("=");

    Serial.print(
      resultado.probabilidades[c] *
      100.0f,
      1
    );

    Serial.print("%");

    if (
      c < MODEL_NUM_CLASSES - 1
    ) {
      Serial.print(" | ");
    }
  }

  Serial.println();
  Serial.println(
    "---------------------------------------"
  );

  delay(500);
}