#include <Arduino.h>
#include <math.h>
#include "model.h"  // copie model/model.h para a pasta do sketch ao usar o baseline local

// CUBI-04 — CP2 — TinyML + ação nos motores.
//
// Este sketch é a integração local de referência. Ele usa model.h para permitir
// validar o ciclo sensor -> inferência -> L298N antes do export do Edge Impulse.
// Para a entrega final, substituir a função inferir() pelo classificador gerado
// pelo projeto real e registrar a biblioteca em model/edge-impulse/.
//
// Requer ESP32 Arduino core 3.x por usar ledcAttach/ledcWrite por pino.
// Perfil de pinagem CP2 da especificação 2026:
//   HC-SR04 esquerdo:  TRIG 19 / ECHO 23
//   HC-SR04 central:   TRIG 5  / ECHO 18
//   HC-SR04 direito:   TRIG 4  / ECHO 13
//   L298N esquerdo:    ENA 25 / IN1 26 / IN2 27
//   L298N direito:     ENB 32 / IN3 14 / IN4 33
//
// Todos os ECHO devem passar pelo divisor resistivo. Testar primeiro com as rodas
// suspensas e com a alimentação dos motores adequada à especificação dos TT.

// --------------------------- Sensores ---------------------------
constexpr uint8_t TRIG_ESQ = 19;
constexpr uint8_t ECHO_ESQ = 23;
constexpr uint8_t TRIG_CENTRO = 5;
constexpr uint8_t ECHO_CENTRO = 18;
constexpr uint8_t TRIG_DIR = 4;
constexpr uint8_t ECHO_DIR = 13;

constexpr unsigned long TIMEOUT_ECHO_US = 25000;
constexpr float DISTANCIA_MIN_CM = 2.0f;
constexpr uint8_t AMOSTRAS_SENSOR = 3;

// ---------------------------- Motores ---------------------------
constexpr uint8_t ENA = 25;
constexpr uint8_t IN1 = 26;
constexpr uint8_t IN2 = 27;
constexpr uint8_t ENB = 32;
constexpr uint8_t IN3 = 14;
constexpr uint8_t IN4 = 33;

constexpr uint32_t PWM_FREQUENCIA_HZ = 1000;
constexpr uint8_t PWM_RESOLUCAO = 8;
constexpr uint8_t PWM_MAX = 140;  // aproximadamente 55% de 255.
constexpr uint8_t PWM_CURVA = 115;
constexpr unsigned long TEMPO_GIRO_MS = 420;
constexpr unsigned long INTERVALO_INFERENCIA_MS = 150;

struct Leituras {
  float esquerda;
  float centro;
  float direita;
};

struct ResultadoInferencia {
  uint8_t classe;
  float confianca;
  float probabilidades[MODEL_NUM_CLASSES];
};

unsigned long ultimaInferencia = 0;
unsigned long girandoAte = 0;
const char* acaoAtual = "PARADO";

const char* nomeClasse(uint8_t classe) {
  if (classe >= MODEL_NUM_CLASSES) return "INDEFINIDA";
  return MODEL_LABELS[classe];
}

// ------------------------- Leitura do sensor --------------------
float medirUmaVez(uint8_t trig, uint8_t echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(3);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  const unsigned long duracao = pulseIn(echo, HIGH, TIMEOUT_ECHO_US);
  if (duracao == 0) return -1.0f;

  const float cm = duracao * 0.0343f / 2.0f;
  if (cm < DISTANCIA_MIN_CM || cm > DISTANCIA_MAX_CM) return -1.0f;
  return cm;
}

float medirMedia(uint8_t trig, uint8_t echo) {
  float soma = 0.0f;
  uint8_t validas = 0;
  for (uint8_t i = 0; i < AMOSTRAS_SENSOR; ++i) {
    const float cm = medirUmaVez(trig, echo);
    if (cm > 0.0f) {
      soma += cm;
      ++validas;
    }
    delay(35);
  }
  return validas == 0 ? -1.0f : soma / validas;
}

bool lerSensores(Leituras& leituras) {
  leituras.esquerda = medirMedia(TRIG_ESQ, ECHO_ESQ);
  leituras.centro = medirMedia(TRIG_CENTRO, ECHO_CENTRO);
  leituras.direita = medirMedia(TRIG_DIR, ECHO_DIR);
  return leituras.esquerda > 0.0f && leituras.centro > 0.0f && leituras.direita > 0.0f;
}

// ----------------------- Modelo local de referência --------------
float limitarDistancia(float cm) {
  if (cm < DISTANCIA_MIN_CM) return DISTANCIA_MIN_CM;
  if (cm > MODEL_NORMALIZATION_CM) return MODEL_NORMALIZATION_CM;
  return cm;
}

ResultadoInferencia inferir(const Leituras& leituras) {
  ResultadoInferencia resultado = {};
  const float entrada[MODEL_NUM_FEATURES] = {
      limitarDistancia(leituras.esquerda) / MODEL_NORMALIZATION_CM,
      limitarDistancia(leituras.centro) / MODEL_NORMALIZATION_CM,
      limitarDistancia(leituras.direita) / MODEL_NORMALIZATION_CM};
  float logits[MODEL_NUM_CLASSES] = {};

  for (uint8_t classe = 0; classe < MODEL_NUM_CLASSES; ++classe) {
    logits[classe] = MODEL_BIAS[classe];
    for (uint8_t feature = 0; feature < MODEL_NUM_FEATURES; ++feature) {
      logits[classe] += MODEL_WEIGHTS[classe][feature] * entrada[feature];
    }
  }

  float maior = logits[0];
  for (uint8_t classe = 1; classe < MODEL_NUM_CLASSES; ++classe) {
    if (logits[classe] > maior) maior = logits[classe];
  }

  float soma = 0.0f;
  for (uint8_t classe = 0; classe < MODEL_NUM_CLASSES; ++classe) {
    resultado.probabilidades[classe] = expf(logits[classe] - maior);
    soma += resultado.probabilidades[classe];
  }

  for (uint8_t classe = 0; classe < MODEL_NUM_CLASSES; ++classe) {
    resultado.probabilidades[classe] /= soma;
    if (resultado.probabilidades[classe] > resultado.confianca) {
      resultado.confianca = resultado.probabilidades[classe];
      resultado.classe = classe;
    }
  }
  return resultado;
}

// ------------------------- Controle dos motores ------------------
void escreverMotor(uint8_t inA, uint8_t inB, uint8_t enable, int16_t comando) {
  const int16_t limitado = constrain(comando, -static_cast<int16_t>(PWM_MAX),
                                     static_cast<int16_t>(PWM_MAX));
  if (limitado > 0) {
    digitalWrite(inA, HIGH);
    digitalWrite(inB, LOW);
  } else if (limitado < 0) {
    digitalWrite(inA, LOW);
    digitalWrite(inB, HIGH);
  } else {
    digitalWrite(inA, LOW);
    digitalWrite(inB, LOW);
  }
  ledcWrite(enable, abs(limitado));
}

void mover(int16_t esquerdo, int16_t direito, const char* acao) {
  escreverMotor(IN1, IN2, ENA, esquerdo);
  escreverMotor(IN3, IN4, ENB, direito);
  acaoAtual = acao;
}

void parar() {
  mover(0, 0, "PARADO");
}

void avancar() {
  mover(PWM_CURVA, PWM_CURVA, "AVANCAR: LIVRE");
}

void girarEsquerda() {
  mover(-PWM_CURVA, PWM_CURVA, "GIRAR_ESQUERDA: OBSTACULO_DIREITA");
}

void girarDireita() {
  mover(PWM_CURVA, -PWM_CURVA, "GIRAR_DIREITA: OBSTACULO_ESQUERDA");
}

// Esta é a ligação explícita entre a saída da inferência e o comportamento físico.
void executarAcao(const ResultadoInferencia& resultado, const Leituras& leituras) {
  if (resultado.classe == 0) {
    girandoAte = 0;
    avancar();
    return;
  }

  if (resultado.classe == 1) {
    girandoAte = millis() + TEMPO_GIRO_MS;
    girarDireita();
    return;
  }

  if (resultado.classe == 2) {
    girandoAte = millis() + TEMPO_GIRO_MS;
    if (leituras.esquerda >= leituras.direita) {
      girarEsquerda();
    } else {
      girarDireita();
    }
    return;
  }

  girandoAte = millis() + TEMPO_GIRO_MS;
  girarEsquerda();
}

void imprimirResultado(const Leituras& leituras, const ResultadoInferencia& resultado) {
  Serial.println();
  Serial.println("---------- CP2 TINYML + ACAO ----------");
  Serial.print("ENTRADA cm: ESQ=");
  Serial.print(leituras.esquerda, 1);
  Serial.print(" CENTRO=");
  Serial.print(leituras.centro, 1);
  Serial.print(" DIR=");
  Serial.println(leituras.direita, 1);
  Serial.print("CLASSE: ");
  Serial.print(nomeClasse(resultado.classe));
  Serial.print(" | CONFIANCA: ");
  Serial.print(resultado.confianca * 100.0f, 1);
  Serial.println('%');
  Serial.print("ACAO DOS MOTORES: ");
  Serial.println(acaoAtual);
  Serial.print("PROBABILIDADES: ");
  for (uint8_t classe = 0; classe < MODEL_NUM_CLASSES; ++classe) {
    if (classe > 0) Serial.print(" | ");
    Serial.print(nomeClasse(classe));
    Serial.print('=');
    Serial.print(resultado.probabilidades[classe] * 100.0f, 1);
    Serial.print('%');
  }
  Serial.println();
  Serial.println("----------------------------------------");
}

void configurarPinos() {
  pinMode(TRIG_ESQ, OUTPUT);
  pinMode(ECHO_ESQ, INPUT);
  pinMode(TRIG_CENTRO, OUTPUT);
  pinMode(ECHO_CENTRO, INPUT);
  pinMode(TRIG_DIR, OUTPUT);
  pinMode(ECHO_DIR, INPUT);
  digitalWrite(TRIG_ESQ, LOW);
  digitalWrite(TRIG_CENTRO, LOW);
  digitalWrite(TRIG_DIR, LOW);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  ledcAttach(ENA, PWM_FREQUENCIA_HZ, PWM_RESOLUCAO);
  ledcAttach(ENB, PWM_FREQUENCIA_HZ, PWM_RESOLUCAO);
  parar();
}

void setup() {
  Serial.begin(115200);
  delay(800);
  configurarPinos();
  Serial.println("CUBI-04 CP2 — inferencia TinyML com acao nos motores");
  Serial.println("Modelo local de referencia: substituir pelo export real do Edge Impulse.");
  Serial.println("Serial Monitor: 115200 baud");
}

void loop() {
  if (girandoAte != 0 && static_cast<long>(millis() - girandoAte) >= 0) {
    girandoAte = 0;
    parar();
  }

  if (girandoAte != 0) return;
  if (millis() - ultimaInferencia < INTERVALO_INFERENCIA_MS) return;
  ultimaInferencia = millis();

  Leituras leituras;
  if (!lerSensores(leituras)) {
    parar();
    Serial.println("[SEGURANCA] leitura invalida; motores parados");
    return;
  }

  const ResultadoInferencia resultado = inferir(leituras);
  executarAcao(resultado, leituras);
  imprimirResultado(leituras, resultado);
}
