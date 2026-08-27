/*
  Carrinho-robô Wi-Fi com ESP32, L298N, 2 motores e HC-SR04

  Firmware: v0.2
  Tarefa 18: acréscimo do sensor ultrassônico e bloqueio de obstáculos

  Rede criada pelo ESP32:
    Nome: Carrinho-ESP32
    Senha: carrinho123
    Painel: http://192.168.4.1

  Desenvolvido para Arduino-ESP32 3.x.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// -------------------- Wi-Fi e servidor --------------------
constexpr char FIRMWARE_VERSION[] = "0.2";
const char* WIFI_NAME = "Carrinho-ESP32";
const char* WIFI_PASSWORD = "carrinho123";  // Minimo de 8 caracteres.

IPAddress localIP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

// -------------------- L298N --------------------
// Remova os jumpers ENA e ENB do L298N para controlar a velocidade por PWM.
constexpr uint8_t PIN_ENA = 25;
constexpr uint8_t PIN_IN1 = 26;
constexpr uint8_t PIN_IN2 = 27;
constexpr uint8_t PIN_ENB = 33;
constexpr uint8_t PIN_IN3 = 32;
constexpr uint8_t PIN_IN4 = 14;

constexpr uint32_t PWM_FREQUENCY = 10000;
constexpr uint8_t PWM_RESOLUTION = 8;  // Valores de 0 a 255.

// Motores montados espelhados normalmente precisam de um lado invertido.
// Se um lado girar ao contrario, troque true/false somente aqui.
constexpr bool LEFT_MOTOR_INVERTED = false;
constexpr bool RIGHT_MOTOR_INVERTED = true;

// -------------------- HC-SR04 --------------------
constexpr uint8_t PIN_TRIG = 18;
constexpr uint8_t PIN_ECHO = 19;
constexpr float STOP_DISTANCE_CM = 20.0F;
constexpr uint32_t ULTRASONIC_INTERVAL_MS = 100;

// O ECHO do HC-SR04 trabalha em 5 V. Use divisor resistivo:
// ECHO -- 1 kOhm -- GPIO 19 -- 2 kOhm -- GND.

// -------------------- Seguranca --------------------
constexpr uint32_t COMMAND_TIMEOUT_MS = 800;

enum class Direction : uint8_t {
  Stop,
  Forward,
  Backward,
  Left,
  Right
};

Direction currentDirection = Direction::Stop;
uint8_t currentSpeed = 180;
float distanceCm = -1.0F;
bool obstacleBlocked = false;
uint32_t lastCommandAt = 0;
uint32_t lastUltrasonicAt = 0;

// -------------------- Pagina web --------------------
const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html lang="pt-BR">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
  <title>Carrinho ESP32</title>
  <style>
    :root { color-scheme: dark; font-family: system-ui, -apple-system, sans-serif; }
    * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }
    body { margin: 0; min-height: 100vh; display: grid; place-items: center; background: #07111f; color: #e5eefb; }
    main { width: min(94vw, 430px); padding: 22px; border-radius: 24px; background: #101d30; box-shadow: 0 24px 70px #0008; }
    h1 { margin: 0 0 4px; font-size: 1.55rem; }
    .subtitle { margin: 0 0 18px; color: #94a9c7; }
    .status { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-bottom: 18px; }
    .card { min-height: 72px; padding: 12px; border-radius: 15px; background: #182a43; }
    .card small { display: block; color: #9cb0cc; }
    .card strong { display: block; margin-top: 5px; font-size: 1.1rem; }
    .blocked { color: #ff8c8c; }
    .free { color: #75e6a7; }
    .pad { display: grid; grid-template-columns: repeat(3, 1fr); gap: 12px; user-select: none; touch-action: none; }
    button { min-height: 78px; border: 0; border-radius: 18px; color: white; background: #243b5d; font-size: 2rem; font-weight: 800; box-shadow: inset 0 -4px #0004; }
    button:active, button.active { transform: translateY(2px); background: #2e65b7; box-shadow: inset 0 -2px #0005; }
    button.stop { background: #b62d45; }
    .empty { visibility: hidden; }
    .speed { margin-top: 20px; }
    .speed-row { display: flex; justify-content: space-between; margin-bottom: 8px; }
    input[type=range] { width: 100%; accent-color: #5ea1ff; }
    .hint { margin: 16px 0 0; color: #8ea4c3; font-size: .85rem; text-align: center; }
  </style>
</head>
<body>
  <main>
    <h1>Carrinho ESP32</h1>
    <p class="subtitle">Controle Wi-Fi local • Firmware v0.2</p>

    <section class="status">
      <div class="card"><small>Distancia frontal</small><strong id="distance">-- cm</strong></div>
      <div class="card"><small>Estado</small><strong id="state" class="free">Pronto</strong></div>
    </section>

    <section class="pad" aria-label="Controles de movimento">
      <span class="empty"></span>
      <button data-dir="F" aria-label="Avancar">▲</button>
      <span class="empty"></span>
      <button data-dir="L" aria-label="Virar a esquerda">◀</button>
      <button data-dir="S" class="stop" aria-label="Parar">■</button>
      <button data-dir="R" aria-label="Virar a direita">▶</button>
      <span class="empty"></span>
      <button data-dir="B" aria-label="Recuar">▼</button>
      <span class="empty"></span>
    </section>

    <section class="speed">
      <div class="speed-row"><span>Velocidade</span><strong id="speedValue">180</strong></div>
      <input id="speed" type="range" min="90" max="255" value="180" step="5">
    </section>

    <p class="hint">Segure uma seta para mover. Solte para parar.</p>
  </main>

  <script>
    const speed = document.querySelector('#speed');
    const speedValue = document.querySelector('#speedValue');
    const distance = document.querySelector('#distance');
    const state = document.querySelector('#state');
    const buttons = [...document.querySelectorAll('button[data-dir]')];
    let activeDirection = 'S';
    let keepAlive = null;

    speed.addEventListener('input', () => speedValue.textContent = speed.value);

    function updateStatus(data) {
      distance.textContent = data.distance < 0 ? '-- cm' : `${data.distance.toFixed(1)} cm`;
      state.textContent = data.blocked ? 'Obstaculo' : (data.command === 'S' ? 'Parado' : 'Movendo');
      state.className = data.blocked ? 'blocked' : 'free';
    }

    async function sendCommand(direction) {
      try {
        const response = await fetch(`/api/cmd?dir=${direction}&speed=${speed.value}`, {cache: 'no-store'});
        updateStatus(await response.json());
      } catch (_) {
        state.textContent = 'Sem conexao';
        state.className = 'blocked';
      }
    }

    function stopKeepAlive() {
      if (keepAlive) clearInterval(keepAlive);
      keepAlive = null;
      buttons.forEach(button => button.classList.remove('active'));
    }

    function startMovement(event) {
      event.preventDefault();
      const button = event.currentTarget;
      const direction = button.dataset.dir;
      stopKeepAlive();
      activeDirection = direction;
      button.classList.add('active');
      sendCommand(direction);
      if (direction !== 'S') {
        keepAlive = setInterval(() => sendCommand(direction), 250);
      }
    }

    function stopMovement(event) {
      if (event) event.preventDefault();
      if (activeDirection !== 'S') sendCommand('S');
      activeDirection = 'S';
      stopKeepAlive();
    }

    buttons.forEach(button => {
      button.addEventListener('pointerdown', startMovement);
      button.addEventListener('pointerup', stopMovement);
      button.addEventListener('pointercancel', stopMovement);
      button.addEventListener('pointerleave', stopMovement);
    });

    window.addEventListener('blur', stopMovement);
    document.addEventListener('visibilitychange', () => {
      if (document.hidden) stopMovement();
    });

    setInterval(async () => {
      try {
        const response = await fetch('/api/status', {cache: 'no-store'});
        updateStatus(await response.json());
      } catch (_) {}
    }, 500);
  </script>
</body>
</html>
)HTML";

// -------------------- Controle dos motores --------------------
void setMotor(uint8_t input1, uint8_t input2, uint8_t enablePin, int16_t power, bool inverted) {
  power = constrain(power, -255, 255);
  if (inverted) power = -power;

  if (power > 0) {
    digitalWrite(input1, HIGH);
    digitalWrite(input2, LOW);
  } else if (power < 0) {
    digitalWrite(input1, LOW);
    digitalWrite(input2, HIGH);
  } else {
    digitalWrite(input1, LOW);
    digitalWrite(input2, LOW);
  }

  ledcWrite(enablePin, abs(power));
}

void stopMotors() {
  setMotor(PIN_IN1, PIN_IN2, PIN_ENA, 0, LEFT_MOTOR_INVERTED);
  setMotor(PIN_IN3, PIN_IN4, PIN_ENB, 0, RIGHT_MOTOR_INVERTED);
  currentDirection = Direction::Stop;
}

void moveRobot(Direction direction, uint8_t speedValue) {
  if (direction == Direction::Forward && obstacleBlocked) {
    stopMotors();
    return;
  }

  currentSpeed = speedValue;
  currentDirection = direction;

  switch (direction) {
    case Direction::Forward:
      setMotor(PIN_IN1, PIN_IN2, PIN_ENA, speedValue, LEFT_MOTOR_INVERTED);
      setMotor(PIN_IN3, PIN_IN4, PIN_ENB, speedValue, RIGHT_MOTOR_INVERTED);
      break;
    case Direction::Backward:
      setMotor(PIN_IN1, PIN_IN2, PIN_ENA, -speedValue, LEFT_MOTOR_INVERTED);
      setMotor(PIN_IN3, PIN_IN4, PIN_ENB, -speedValue, RIGHT_MOTOR_INVERTED);
      break;
    case Direction::Left:
      setMotor(PIN_IN1, PIN_IN2, PIN_ENA, -speedValue, LEFT_MOTOR_INVERTED);
      setMotor(PIN_IN3, PIN_IN4, PIN_ENB, speedValue, RIGHT_MOTOR_INVERTED);
      break;
    case Direction::Right:
      setMotor(PIN_IN1, PIN_IN2, PIN_ENA, speedValue, LEFT_MOTOR_INVERTED);
      setMotor(PIN_IN3, PIN_IN4, PIN_ENB, -speedValue, RIGHT_MOTOR_INVERTED);
      break;
    case Direction::Stop:
    default:
      stopMotors();
      break;
  }
}

// -------------------- Sensor ultrassonico --------------------
float readDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  const uint32_t duration = pulseIn(PIN_ECHO, HIGH, 25000UL);
  if (duration == 0) return -1.0F;
  return (duration * 0.0343F) / 2.0F;
}

// -------------------- API web --------------------
const char* directionCode(Direction direction) {
  switch (direction) {
    case Direction::Forward: return "F";
    case Direction::Backward: return "B";
    case Direction::Left: return "L";
    case Direction::Right: return "R";
    case Direction::Stop:
    default: return "S";
  }
}

String statusJson() {
  String json = "{";
  json += "\"version\":\"";
  json += FIRMWARE_VERSION;
  json += "\",";
  json += "\"distance\":";
  json += String(distanceCm, 1);
  json += ",\"blocked\":";
  json += (obstacleBlocked ? "true" : "false");
  json += ",\"command\":\"";
  json += directionCode(currentDirection);
  json += "\",\"speed\":";
  json += String(currentSpeed);
  json += ",\"clients\":";
  json += String(WiFi.softAPgetStationNum());
  json += "}";
  return json;
}

Direction parseDirection(const String& value) {
  if (value == "F") return Direction::Forward;
  if (value == "B") return Direction::Backward;
  if (value == "L") return Direction::Left;
  if (value == "R") return Direction::Right;
  return Direction::Stop;
}

void handleCommand() {
  const Direction requestedDirection = parseDirection(server.arg("dir"));
  const int requestedSpeed = server.hasArg("speed")
    ? constrain(server.arg("speed").toInt(), 0, 255)
    : currentSpeed;

  lastCommandAt = millis();
  moveRobot(requestedDirection, static_cast<uint8_t>(requestedSpeed));

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", statusJson());
}

void handleStatus() {
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", statusJson());
}

void configureWebServer() {
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
  });

  server.on("/api/cmd", HTTP_GET, handleCommand);
  server.on("/api/status", HTTP_GET, handleStatus);

  server.onNotFound([]() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  server.begin();
}

// -------------------- Arduino --------------------
void setup() {
  Serial.begin(115200);

  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  const bool pwmAOk = ledcAttach(PIN_ENA, PWM_FREQUENCY, PWM_RESOLUTION);
  const bool pwmBOk = ledcAttach(PIN_ENB, PWM_FREQUENCY, PWM_RESOLUTION);
  if (!pwmAOk || !pwmBOk) {
    Serial.println("ERRO: nao foi possivel configurar o PWM.");
    while (true) delay(1000);
  }

  stopMotors();
  distanceCm = readDistanceCm();
  obstacleBlocked = distanceCm > 0 && distanceCm < STOP_DISTANCE_CM;

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(localIP, gateway, subnet);

  if (!WiFi.softAP(WIFI_NAME, WIFI_PASSWORD)) {
    Serial.println("ERRO: nao foi possivel criar a rede Wi-Fi.");
    while (true) delay(1000);
  }

  configureWebServer();

  Serial.println();
  Serial.println("Carrinho pronto.");
  Serial.print("Firmware: v");
  Serial.println(FIRMWARE_VERSION);
  Serial.print("Rede Wi-Fi: ");
  Serial.println(WIFI_NAME);
  Serial.print("Senha: ");
  Serial.println(WIFI_PASSWORD);
  Serial.print("Abra no celular: http://");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();

  const uint32_t now = millis();

  if (now - lastUltrasonicAt >= ULTRASONIC_INTERVAL_MS) {
    lastUltrasonicAt = now;
    distanceCm = readDistanceCm();
    obstacleBlocked = distanceCm > 0 && distanceCm < STOP_DISTANCE_CM;

    if (obstacleBlocked && currentDirection == Direction::Forward) {
      stopMotors();
    }
  }

  if (currentDirection != Direction::Stop && now - lastCommandAt > COMMAND_TIMEOUT_MS) {
    stopMotors();
  }
}
