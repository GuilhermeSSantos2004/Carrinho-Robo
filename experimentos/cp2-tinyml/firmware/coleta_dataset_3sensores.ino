#include <Arduino.h>

// CUBI-04 — CP2 — coleta rotulada para o dataset do Edge Impulse.
//
// Faça uma sessão por classe. Antes de cada upload, altere CLASSE_COLETA
// para o cenário físico que será colocado diante dos sensores.
// O sketch não aciona motores e não inventa rótulos a partir da distância.

// Perfil de pinagem da especificação 2026 do CUBI-04.
constexpr uint8_t TRIG_ESQ = 19;
constexpr uint8_t ECHO_ESQ = 23;
constexpr uint8_t TRIG_CENTRO = 5;
constexpr uint8_t ECHO_CENTRO = 18;
constexpr uint8_t TRIG_DIR = 4;
constexpr uint8_t ECHO_DIR = 13;

// 0 LIVRE, 1 OBSTACULO_ESQUERDA, 2 OBSTACULO_CENTRO, 3 OBSTACULO_DIREITA.
constexpr uint8_t CLASSE_COLETA = 0;
constexpr unsigned long INTERVALO_MS = 250;
constexpr unsigned long TIMEOUT_ECHO_US = 25000;
constexpr float DISTANCIA_MIN_CM = 2.0f;
constexpr float DISTANCIA_MAX_CM = 400.0f;

unsigned long ultimaColeta = 0;

const char* nomeClasse(uint8_t classe) {
  switch (classe) {
    case 0: return "LIVRE";
    case 1: return "OBSTACULO_ESQUERDA";
    case 2: return "OBSTACULO_CENTRO";
    case 3: return "OBSTACULO_DIREITA";
    default: return "INVALIDA";
  }
}

float medirDistancia(uint8_t trig, uint8_t echo) {
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

void configurarSensor(uint8_t trig, uint8_t echo) {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  digitalWrite(trig, LOW);
}

void setup() {
  Serial.begin(115200);
  configurarSensor(TRIG_ESQ, ECHO_ESQ);
  configurarSensor(TRIG_CENTRO, ECHO_CENTRO);
  configurarSensor(TRIG_DIR, ECHO_DIR);

  if (CLASSE_COLETA > 3) {
    Serial.println("ERRO: CLASSE_COLETA deve estar entre 0 e 3.");
    while (true) delay(1000);
  }

  Serial.println();
  Serial.println("# CUBI-04 CP2 — COLETA REAL");
  Serial.print("# classe_configurada=");
  Serial.print(CLASSE_COLETA);
  Serial.print(" (");
  Serial.print(nomeClasse(CLASSE_COLETA));
  Serial.println(")");
  Serial.println("# Salve o TXT bruto desta sessao antes de trocar a classe.");
  Serial.println("esquerda_cm,centro_cm,direita_cm,label");
}

void loop() {
  const unsigned long agora = millis();
  if (agora - ultimaColeta < INTERVALO_MS) return;
  ultimaColeta = agora;

  const float esquerda = medirDistancia(TRIG_ESQ, ECHO_ESQ);
  delay(35);
  const float centro = medirDistancia(TRIG_CENTRO, ECHO_CENTRO);
  delay(35);
  const float direita = medirDistancia(TRIG_DIR, ECHO_DIR);

  if (esquerda < 0 || centro < 0 || direita < 0) {
    Serial.print("# SEM_ECO,esq=");
    Serial.print(esquerda, 1);
    Serial.print(",centro=");
    Serial.print(centro, 1);
    Serial.print(",dir=");
    Serial.println(direita, 1);
    return;
  }

  Serial.print(esquerda, 2);
  Serial.print(',');
  Serial.print(centro, 2);
  Serial.print(',');
  Serial.print(direita, 2);
  Serial.print(',');
  Serial.println(CLASSE_COLETA);
}
