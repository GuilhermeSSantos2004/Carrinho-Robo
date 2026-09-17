#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

// =====================================================
// WIFI
// =====================================================

const char* ssid = "tony";
const char* password = "stark369";

IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);
DNSServer dnsServer;

// =====================================================
// L298N
// =====================================================

#define ENA 25
#define IN1 26
#define IN2 27

#define ENB 33
#define IN3 32
#define IN4 23

// =====================================================
// ULTRASSONICO
// =====================================================

#define TRIG 18
#define ECHO 19

// =====================================================
// LDR
// =====================================================

#define LDR_PIN 34

// =====================================================
// CONFIGURACOES
// =====================================================

int velocidade = 80;

// Bloqueia SOMENTE a re quando estiver a 5 cm ou menos
const float DISTANCIA_BLOQUEIO_RE = 5.0;

// Log do LDR no Serial Monitor a cada 5 segundos
const unsigned long INTERVALO_LOG_LDR = 5000;

String estado = "PARADO";
float distanciaAtual = -1;
int leituraLDR = 0;
int luminosidade = 0;

// =====================================================
// DISTANCIA
// =====================================================

float medirDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracao = pulseIn(ECHO, HIGH, 30000);
  if (duracao == 0) return -1;

  float distancia = duracao * 0.0343 / 2.0;
  if (distancia < 2 || distancia > 400) return -1;

  return distancia;
}

// =====================================================
// LUMINOSIDADE
// =====================================================

void lerLuminosidade() {
  long soma = 0;

  for (int i = 0; i < 10; i++) {
    soma += analogRead(LDR_PIN);
    delayMicroseconds(200);
  }

  leituraLDR = soma / 10;

  // Ligacao esperada:
  // 3V3 -> LDR -> ponto GPIO34 -> resistor -> GND
  // mais claro = ADC maior; mais escuro = ADC menor
  luminosidade = map(leituraLDR, 0, 4095, 0, 100);
  luminosidade = constrain(luminosidade, 0, 100);
}

// =====================================================
// PWM / MOTORES
// =====================================================

void aplicarVelocidade() {
  analogWrite(ENA, velocidade);
  analogWrite(ENB, velocidade);
}

void parar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  estado = "PARADO";
  Serial.println("[MOTOR] PARADO");
}

void frente() {
  // Frente sempre liberada. O ultrassonico atua como sensor de estacionamento da re.
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  aplicarVelocidade();
  estado = "FRENTE";
  Serial.println("[MOTOR] FRENTE");
}

void tras() {
  distanciaAtual = medirDistancia();

  if (distanciaAtual > 0 && distanciaAtual <= DISTANCIA_BLOQUEIO_RE) {
    parar();
    estado = "RE_BLOQUEADA";
    Serial.print("[SEGURANCA] RE BLOQUEADA: ");
    Serial.print(distanciaAtual, 1);
    Serial.println(" cm");
    return;
  }

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  aplicarVelocidade();
  estado = "TRAS";
  Serial.println("[MOTOR] TRAS");
}

void esquerda() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  aplicarVelocidade();
  estado = "ESQUERDA";
  Serial.println("[MOTOR] ESQUERDA");
}

void direita() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  aplicarVelocidade();
  estado = "DIREITA";
  Serial.println("[MOTOR] DIREITA");
}

// =====================================================
// PAGINA WEB
// =====================================================

String pagina() {
return R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<title>Carrinho ESP32</title>
<style>
*{box-sizing:border-box;touch-action:manipulation}
:root{--fundo:#101010;--card:#1d1d1d;--texto:#fff;--secundario:#aaa}
body{margin:0;padding:15px;background:var(--fundo);color:var(--texto);font-family:Arial,sans-serif;text-align:center;transition:background .4s,color .4s}
body.claro{--fundo:#eee;--card:#fff;--texto:#111;--secundario:#555}
body.escuro{--fundo:#101010;--card:#1d1d1d;--texto:#fff;--secundario:#aaa}
h1{margin:10px 0 5px}.online{display:inline-block;padding:8px 18px;border-radius:20px;background:#143d20;color:#56ff7c;font-weight:bold;margin:5px}
.luz{margin:10px auto;padding:12px;max-width:330px;background:var(--card);border-radius:15px}.periodo{font-size:24px;font-weight:bold}.luminosidade{color:var(--secundario);margin-top:5px}
.temas{margin:12px 0}.temaBotao{padding:9px 14px;margin:3px;border:0;border-radius:10px;background:#444;color:#fff;font-size:14px}.temaAtivo{background:#1565c0}
.distanciaBox{margin:15px auto;width:300px;max-width:95%;padding:15px;border-radius:20px;background:var(--card);border:4px solid #333;transition:.2s}.distanciaNumero{font-size:55px;font-weight:bold}.cm{color:var(--secundario)}
.atencao{border-color:#ff9800}.perigo{background:#c62828!important;border-color:#ff5252;color:#fff}.critico{background:#d50000!important;border-color:#fff;color:#fff;animation:piscar .15s infinite}
@keyframes piscar{0%{opacity:1}50%{opacity:.35}100%{opacity:1}}.alertaTexto{display:none;margin:10px auto;max-width:300px;padding:10px;border-radius:12px;background:#d50000;color:#fff;font-weight:bold}
.controle{display:grid;grid-template-columns:90px 90px 90px;grid-template-rows:90px 90px 90px;gap:10px;justify-content:center;margin-top:20px}.controle button{border:0;border-radius:20px;background:#333;color:#fff;font-size:32px;user-select:none}.controle button:active{background:#777;transform:scale(.94)}
.frente{grid-column:2;grid-row:1}.esquerda{grid-column:1;grid-row:2}.stop{grid-column:2;grid-row:2;background:#c62828!important;font-size:15px!important;font-weight:bold}.direita{grid-column:3;grid-row:2}.tras{grid-column:2;grid-row:3}
.som{margin:15px;padding:12px 20px;border:0;border-radius:12px;background:#1565c0;color:#fff;font-size:16px}.slider{width:300px;max-width:90%}
</style>
</head>
<body class="escuro">
<h1>Carrinho ESP32</h1>
<div class="online">● ONLINE</div>
<div class="luz"><div class="periodo" id="periodo">☀ DIA</div><div class="luminosidade">Luminosidade: <strong id="luzValor">--</strong>%<br><small>ADC bruto: <strong id="ldrRaw">--</strong></small></div></div>
<div class="temas"><button id="autoBtn" class="temaBotao temaAtivo" onclick="tema('auto')">AUTO</button><button id="claroBtn" class="temaBotao" onclick="tema('claro')">CLARO</button><button id="escuroBtn" class="temaBotao" onclick="tema('escuro')">ESCURO</button></div>
<div class="distanciaBox" id="distanciaBox"><div>DISTÂNCIA</div><div class="distanciaNumero" id="distancia">---</div><div class="cm">cm</div></div>
<div id="alerta" class="alertaTexto">OBSTÁCULO!</div>
<p>Estado: <strong id="estado">PARADO</strong></p>
<button id="somBotao" class="som" onclick="toggleSom()">🔇 ATIVAR ALERTA SONORO</button>
<div class="controle">
<button class="frente" onpointerdown="mover('frente')" onpointerup="parar()" onpointercancel="parar()">▲</button>
<button class="esquerda" onpointerdown="mover('esquerda')" onpointerup="parar()" onpointercancel="parar()">◀</button>
<button class="stop" onclick="parar()">PARAR</button>
<button class="direita" onpointerdown="mover('direita')" onpointerup="parar()" onpointercancel="parar()">▶</button>
<button class="tras" onpointerdown="mover('tras')" onpointerup="parar()" onpointercancel="parar()">▼</button>
</div>
<h3>Velocidade</h3>
<input id="slider" class="slider" type="range" min="40" max="140" value="80">
<p>PWM: <strong id="valor">80</strong>/255</p>
<script>
let modoTema="auto",somAtivo=false,audioCtx=null,beepTimer=null,osciladorContinuo=null,ultimaFaixaSom=-1,ultimoEstadoSom="";
function mover(d){fetch('/'+d)}function parar(){fetch('/parar')}
const slider=document.getElementById('slider');slider.addEventListener('input',function(){document.getElementById('valor').innerText=this.value;fetch('/velocidade?valor='+this.value)});
function toggleSom(){somAtivo=!somAtivo;if(somAtivo){if(!audioCtx)audioCtx=new(window.AudioContext||window.webkitAudioContext)();if(audioCtx.state==='suspended')audioCtx.resume();document.getElementById('somBotao').innerText='🔊 ALERTA SONORO ATIVO'}else{pararSom();document.getElementById('somBotao').innerText='🔇 ATIVAR ALERTA SONORO'}}
function beep(d=100){if(!somAtivo||!audioCtx)return;const o=audioCtx.createOscillator(),g=audioCtx.createGain();o.frequency.value=900;g.gain.value=.12;o.connect(g);g.connect(audioCtx.destination);o.start();setTimeout(()=>{try{o.stop()}catch(e){}},d)}
function iniciarSomContinuo(){if(!somAtivo||!audioCtx||osciladorContinuo)return;osciladorContinuo=audioCtx.createOscillator();const g=audioCtx.createGain();osciladorContinuo.frequency.value=1100;g.gain.value=.12;osciladorContinuo.connect(g);g.connect(audioCtx.destination);osciladorContinuo.start()}
function pararSom(){if(beepTimer){clearTimeout(beepTimer);beepTimer=null}if(osciladorContinuo){try{osciladorContinuo.stop()}catch(e){}osciladorContinuo=null}}
function atualizarSom(d){pararSom();const e=document.getElementById('estado').innerText;if(e!=='TRAS'&&e!=='RE_BLOQUEADA')return;if(!somAtivo||d<0||d>100)return;if(d<=5){iniciarSomContinuo();return}let i;if(d<=15)i=100;else if(d<=25)i=180;else if(d<=40)i=300;else if(d<=60)i=600;else i=1100;function ciclo(){if(!somAtivo)return;beep(80);beepTimer=setTimeout(ciclo,i)}ciclo()}
function tema(n){modoTema=n;['auto','claro','escuro'].forEach(x=>document.getElementById(x+'Btn').classList.remove('temaAtivo'));document.getElementById(n+'Btn').classList.add('temaAtivo');aplicarTema()}
function aplicarTema(){let c=false;if(modoTema==='claro')c=true;else if(modoTema==='auto')c=parseInt(document.getElementById('luzValor').innerText)>=45;document.body.classList.remove('claro','escuro');document.body.classList.add(c?'claro':'escuro')}
function faixaSom(d){if(d<0||d>100)return 0;if(d<=5)return 6;if(d<=15)return 5;if(d<=25)return 4;if(d<=40)return 3;if(d<=60)return 2;return 1}
function atualizarStatus(){fetch('/status').then(r=>r.json()).then(data=>{document.querySelector('.online').innerText='● ONLINE';document.getElementById('estado').innerText=data.estado;document.getElementById('valor').innerText=data.velocidade;const box=document.getElementById('distanciaBox'),dist=document.getElementById('distancia'),al=document.getElementById('alerta');box.classList.remove('atencao','perigo','critico');if(data.distancia<0){dist.innerText='---';al.style.display='none'}else{dist.innerText=data.distancia.toFixed(1);if(data.distancia<=100&&data.distancia>40)box.classList.add('atencao');if(data.distancia<=40&&data.distancia>5)box.classList.add('perigo');if(data.distancia<=5)box.classList.add('critico');if(data.distancia<=5){al.style.display='block';al.innerText='⚠ RE BLOQUEADA: 5 cm OU MENOS!'}else al.style.display='none'}const f=faixaSom(data.distancia);if(f!==ultimaFaixaSom||data.estado!==ultimoEstadoSom){ultimaFaixaSom=f;ultimoEstadoSom=data.estado;atualizarSom(data.distancia)}document.getElementById('luzValor').innerText=data.luz;document.getElementById('ldrRaw').innerText=data.ldr;const p=document.getElementById('periodo');if(data.luz>=65)p.innerText='☀ DIA';else if(data.luz>=30)p.innerText='🌤 MEIA-LUZ';else p.innerText='🌙 NOITE';if(modoTema==='auto')aplicarTema()}).catch(()=>document.querySelector('.online').innerText='● DESCONECTADO')}
setInterval(atualizarStatus,250);window.addEventListener('blur',parar);document.addEventListener('visibilitychange',()=>{if(document.hidden)parar()});document.addEventListener('contextmenu',e=>e.preventDefault());
</script>
</body>
</html>
)rawliteral";
}

// =====================================================
// SETUP
// =====================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LDR_PIN, INPUT);
  digitalWrite(TRIG, LOW);
  parar();

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  bool wifiOK = WiFi.softAP(ssid, password);
  Serial.println(wifiOK ? "[WIFI] OK" : "[WIFI] ERRO");

  dnsServer.start(53, "*", local_IP);

  server.on("/", [](){server.send(200,"text/html",pagina());});
  server.on("/frente", [](){frente();server.send(200,"text/plain","OK");});
  server.on("/tras", [](){tras();server.send(200,"text/plain","OK");});
  server.on("/esquerda", [](){esquerda();server.send(200,"text/plain","OK");});
  server.on("/direita", [](){direita();server.send(200,"text/plain","OK");});
  server.on("/parar", [](){parar();server.send(200,"text/plain","OK");});

  server.on("/velocidade", [](){
    if (server.hasArg("valor")) {
      velocidade = constrain(server.arg("valor").toInt(), 0, 255);
      if (estado != "PARADO" && estado != "RE_BLOQUEADA") aplicarVelocidade();
      Serial.print("[PWM] "); Serial.println(velocidade);
    }
    server.send(200,"text/plain",String(velocidade));
  });

  server.on("/status", [](){
    String json="{";
    json += "\"estado\":\"" + estado + "\",";
    json += "\"velocidade\":" + String(velocidade) + ",";
    json += "\"distancia\":" + String(distanciaAtual,1) + ",";
    json += "\"luz\":" + String(luminosidade) + ",";
    json += "\"ldr\":" + String(leituraLDR);
    json += "}";
    server.send(200,"application/json",json);
  });

  server.onNotFound([](){server.sendHeader("Location","http://192.168.4.1",true);server.send(302,"text/plain","");});
  server.begin();

  Serial.println("=============================");
  Serial.println("CARRINHO ESP32 PRONTO");
  Serial.println("WiFi: tony");
  Serial.println("Senha: stark369");
  Serial.println("IP: http://192.168.4.1");
  Serial.println("=============================");
}

// =====================================================
// LOOP
// =====================================================

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();

  static unsigned long ultimoSensor = 0;

  if (millis() - ultimoSensor >= 100) {
    ultimoSensor = millis();
    distanciaAtual = medirDistancia();

    // Protecao somente da re
    if (estado == "TRAS" && distanciaAtual > 0 && distanciaAtual <= DISTANCIA_BLOQUEIO_RE) {
      parar();
      estado = "RE_BLOQUEADA";
      Serial.print("[SEGURANCA] RE BLOQUEADA: ");
      Serial.print(distanciaAtual, 1);
      Serial.println(" cm");
    }

    lerLuminosidade();

    static unsigned long ultimoLogLDR = 0;
    if (millis() - ultimoLogLDR >= INTERVALO_LOG_LDR) {
      ultimoLogLDR = millis();
      Serial.print("[LDR] ADC=");
      Serial.print(leituraLDR);
      Serial.print(" | Luz=");
      Serial.print(luminosidade);
      Serial.print("% | Ambiente=");
      if (luminosidade >= 65) Serial.println("DIA");
      else if (luminosidade >= 30) Serial.println("MEIA-LUZ");
      else Serial.println("NOITE");
    }
  }
}
