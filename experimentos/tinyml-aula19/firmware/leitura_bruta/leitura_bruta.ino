#include <Arduino.h>

// Pinagem do ESP32 DevKit V1 do CUBI-04. Conferir antes de usar outra placa.
constexpr uint8_t TRIG_PIN = 18;
constexpr uint8_t ECHO_PIN = 19;  // ECHO somente atraves do divisor de tensao.
constexpr unsigned long TIMEOUT_US = 30000;
constexpr unsigned long INTERVALO_MS = 100;
unsigned long ultimaLeitura = 0;

void iniciarHardware() {
  // Defesa adicional: manter a ponte H desabilitada. Testar SEM bateria dos motores.
  const uint8_t pinosMotor[] = {25, 26, 27, 33, 32, 23};
  for (uint8_t pino : pinosMotor) {
    digitalWrite(pino, LOW);
    pinMode(pino, OUTPUT);
  }
  digitalWrite(TRIG_PIN, LOW);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(115200);
  delay(1000);
}

unsigned long medirPulso() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return pulseIn(ECHO_PIN, HIGH, TIMEOUT_US);
}

// Sem media, normalizacao ou corte da distancia. A conversao permite ler cm.
void setup() {
  iniciarHardware();
  Serial.println("tempo_ms,duracao_us,distancia_cm,status");
}

void loop() {
  const unsigned long agora = millis();
  if (agora - ultimaLeitura < INTERVALO_MS) return;
  ultimaLeitura = agora;
  const unsigned long duracao = medirPulso();
  Serial.print(agora);
  Serial.print(',');
  Serial.print(duracao);
  Serial.print(',');
  if (duracao == 0) {
    Serial.println(",SEM_ECO");  // Ausencia de eco nao significa distancia zero.
    return;
  }
  const float distancia = duracao * 0.0343f / 2.0f;
  Serial.print(distancia, 6);
  Serial.print(',');
  Serial.println(distancia >= 2.0f && distancia <= 400.0f ? "OK" : "FORA_FAIXA");
}
