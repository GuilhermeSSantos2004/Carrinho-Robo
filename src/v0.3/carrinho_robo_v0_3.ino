#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

const char* ssid = "tony";
const char* password = "stark369";
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);
DNSServer dnsServer;

#define ENA 25
#define IN1 26
#define IN2 27
#define ENB 33
#define IN3 32
#define IN4 23
#define TRIG 18
#define ECHO 19
#define LDR_PIN 34

int velocidade = 80;
const float DISTANCIA_BLOQUEIO_RE = 5.0;
const unsigned long INTERVALO_LOG_LDR = 5000;
String estado = "PARADO";
float distanciaAtual = -1;
int leituraLDR = 0;
int luminosidade = 0;

float medirDistancia() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duracao = pulseIn(ECHO, HIGH, 30000);
  if (duracao == 0) return -1;
  float distancia = duracao * 0.0343 / 2.0;
  if (distancia < 2 || distancia > 400) return -1;
  return distancia;
}

void lerLuminosidade() {
  long soma = 0;
  for (int i = 0; i < 10; i++) { soma += analogRead(LDR_PIN); delayMicroseconds(200); }
  leituraLDR = soma / 10;
  luminosidade = constrain(map(leituraLDR, 0, 4095, 0, 100), 0, 100);
}

void aplicarVelocidade() { analogWrite(ENA, velocidade); analogWrite(ENB, velocidade); }
void parar() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0); analogWrite(ENB, 0); estado = "PARADO"; Serial.println("[MOTOR] PARADO");
}
void frente() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  aplicarVelocidade(); estado = "FRENTE"; Serial.println("[MOTOR] FRENTE");
}
void tras() {
  distanciaAtual = medirDistancia();
  if (distanciaAtual > 0 && distanciaAtual <= DISTANCIA_BLOQUEIO_RE) {
    parar(); estado = "RE_BLOQUEADA";
    Serial.print("[SEGURANCA] RE BLOQUEADA: "); Serial.print(distanciaAtual, 1); Serial.println(" cm"); return;
  }
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  aplicarVelocidade(); estado = "TRAS"; Serial.println("[MOTOR] TRAS");
}
void esquerda() { digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); aplicarVelocidade(); estado="ESQUERDA"; }
void direita() { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); aplicarVelocidade(); estado="DIREITA"; }

String pagina() {
return R"rawliteral(
<!DOCTYPE html><html lang="pt-BR"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no"><title>Carrinho ESP32</title>
<style>*{box-sizing:border-box;touch-action:manipulation}:root{--fundo:#101010;--card:#1d1d1d;--texto:#fff;--secundario:#aaa}body{margin:0;padding:15px;background:var(--fundo);color:var(--texto);font-family:Arial,sans-serif;text-align:center;transition:.4s}body.claro{--fundo:#eee;--card:#fff;--texto:#111;--secundario:#555}body.escuro{--fundo:#101010;--card:#1d1d1d;--texto:#fff;--secundario:#aaa}.online{display:inline-block;padding:8px 18px;border-radius:20px;background:#143d20;color:#56ff7c;font-weight:bold}.luz,.distanciaBox{margin:12px auto;padding:14px;max-width:330px;background:var(--card);border-radius:16px}.distanciaNumero{font-size:55px;font-weight:bold}.temaBotao,.som{padding:10px 14px;margin:4px;border:0;border-radius:10px;background:#444;color:#fff}.temaAtivo,.som{background:#1565c0}.controle{display:grid;grid-template-columns:90px 90px 90px;grid-template-rows:90px 90px 90px;gap:10px;justify-content:center}.controle button{border:0;border-radius:20px;background:#333;color:#fff;font-size:32px}.frente{grid-column:2}.esquerda{grid-column:1;grid-row:2}.stop{grid-column:2;grid-row:2;background:#c62828!important;font-size:14px!important}.direita{grid-column:3;grid-row:2}.tras{grid-column:2;grid-row:3}.atencao{border:4px solid #ff9800}.perigo{background:#c62828!important}.critico{background:#d50000!important;animation:piscar .15s infinite}@keyframes piscar{50%{opacity:.35}}</style></head><body class="escuro">
<h1>Carrinho ESP32</h1><div class="online">● ONLINE</div><div class="luz"><div id="periodo">☀ DIA</div>Luminosidade: <strong id="luzValor">--</strong>%<br><small>ADC: <strong id="ldrRaw">--</strong></small></div>
<div><button id="autoBtn" class="temaBotao temaAtivo" onclick="tema('auto')">AUTO</button><button id="claroBtn" class="temaBotao" onclick="tema('claro')">CLARO</button><button id="escuroBtn" class="temaBotao" onclick="tema('escuro')">ESCURO</button></div>
<div class="distanciaBox" id="distanciaBox">DISTÂNCIA<div class="distanciaNumero" id="distancia">---</div>cm</div><div id="alerta"></div><p>Estado: <strong id="estado">PARADO</strong></p><button id="somBotao" class="som" onclick="toggleSom()">🔇 ATIVAR ALERTA SONORO</button>
<div class="controle"><button class="frente" onpointerdown="mover('frente')" onpointerup="parar()">▲</button><button class="esquerda" onpointerdown="mover('esquerda')" onpointerup="parar()">◀</button><button class="stop" onclick="parar()">PARAR</button><button class="direita" onpointerdown="mover('direita')" onpointerup="parar()">▶</button><button class="tras" onpointerdown="mover('tras')" onpointerup="parar()">▼</button></div><h3>Velocidade</h3><input id="slider" type="range" min="40" max="140" value="80"><p>PWM: <strong id="valor">80</strong>/255</p>
<script>let modoTema='auto',somAtivo=false,audioCtx=null,beepTimer=null,osciladorContinuo=null,ultimaFaixaSom=-1,ultimoEstadoSom='';function mover(d){fetch('/'+d)}function parar(){fetch('/parar')}slider.oninput=function(){valor.innerText=this.value;fetch('/velocidade?valor='+this.value)};function toggleSom(){somAtivo=!somAtivo;if(somAtivo){if(!audioCtx)audioCtx=new(window.AudioContext||window.webkitAudioContext)();audioCtx.resume();somBotao.innerText='🔊 ALERTA SONORO ATIVO'}else{pararSom();somBotao.innerText='🔇 ATIVAR ALERTA SONORO'}}function beep(d=80){if(!somAtivo||!audioCtx)return;let o=audioCtx.createOscillator(),g=audioCtx.createGain();o.frequency.value=900;g.gain.value=.12;o.connect(g);g.connect(audioCtx.destination);o.start();setTimeout(()=>o.stop(),d)}function pararSom(){if(beepTimer){clearTimeout(beepTimer);beepTimer=null}if(osciladorContinuo){try{osciladorContinuo.stop()}catch(e){}osciladorContinuo=null}}function atualizarSom(d){pararSom();let e=estado.innerText;if(e!=='TRAS'&&e!=='RE_BLOQUEADA'||!somAtivo||d<0||d>100)return;if(d<=5){osciladorContinuo=audioCtx.createOscillator();let g=audioCtx.createGain();osciladorContinuo.frequency.value=1100;g.gain.value=.12;osciladorContinuo.connect(g);g.connect(audioCtx.destination);osciladorContinuo.start();return}let i=d<=15?100:d<=25?180:d<=40?300:d<=60?600:1100;function c(){if(!somAtivo)return;beep();beepTimer=setTimeout(c,i)}c()}function tema(n){modoTema=n;['auto','claro','escuro'].forEach(x=>document.getElementById(x+'Btn').classList.remove('temaAtivo'));document.getElementById(n+'Btn').classList.add('temaAtivo');aplicarTema()}function aplicarTema(){let c=modoTema==='claro'||(modoTema==='auto'&&parseInt(luzValor.innerText)>=45);document.body.className=c?'claro':'escuro'}function faixa(d){return d<0||d>100?0:d<=5?6:d<=15?5:d<=25?4:d<=40?3:d<=60?2:1}setInterval(()=>fetch('/status').then(r=>r.json()).then(d=>{estado.innerText=d.estado;valor.innerText=d.velocidade;distancia.innerText=d.distancia<0?'---':d.distancia.toFixed(1);distanciaBox.className='distanciaBox '+(d.distancia<=5?'critico':d.distancia<=40?'perigo':d.distancia<=100?'atencao':'');alerta.innerText=d.distancia>0&&d.distancia<=5?'⚠ RÉ BLOQUEADA: 5 cm OU MENOS!':'';let f=faixa(d.distancia);if(f!==ultimaFaixaSom||d.estado!==ultimoEstadoSom){ultimaFaixaSom=f;ultimoEstadoSom=d.estado;atualizarSom(d.distancia)}luzValor.innerText=d.luz;ldrRaw.innerText=d.ldr;periodo.innerText=d.luz>=65?'☀ DIA':d.luz>=30?'🌤 MEIA-LUZ':'🌙 NOITE';if(modoTema==='auto')aplicarTema()}),250);window.onblur=parar;document.onvisibilitychange=()=>{if(document.hidden)parar()}</script></body></html>
)rawliteral";
}

void setup(){
  Serial.begin(115200); delay(1000);
  pinMode(ENA,OUTPUT);pinMode(IN1,OUTPUT);pinMode(IN2,OUTPUT);pinMode(ENB,OUTPUT);pinMode(IN3,OUTPUT);pinMode(IN4,OUTPUT);pinMode(TRIG,OUTPUT);pinMode(ECHO,INPUT);pinMode(LDR_PIN,INPUT);digitalWrite(TRIG,LOW);parar();
  WiFi.mode(WIFI_AP);WiFi.softAPConfig(local_IP,gateway,subnet);WiFi.softAP(ssid,password);dnsServer.start(53,"*",local_IP);
  server.on("/",[](){server.send(200,"text/html",pagina());});server.on("/frente",[](){frente();server.send(200,"text/plain","OK");});server.on("/tras",[](){tras();server.send(200,"text/plain","OK");});server.on("/esquerda",[](){esquerda();server.send(200,"text/plain","OK");});server.on("/direita",[](){direita();server.send(200,"text/plain","OK");});server.on("/parar",[](){parar();server.send(200,"text/plain","OK");});server.on("/velocidade",[](){if(server.hasArg("valor")){velocidade=constrain(server.arg("valor").toInt(),0,255);if(estado!="PARADO"&&estado!="RE_BLOQUEADA")aplicarVelocidade();}server.send(200,"text/plain",String(velocidade));});server.on("/status",[](){String j="{\"estado\":\""+estado+"\",\"velocidade\":"+String(velocidade)+",\"distancia\":"+String(distanciaAtual,1)+",\"luz\":"+String(luminosidade)+",\"ldr\":"+String(leituraLDR)+"}";server.send(200,"application/json",j);});server.onNotFound([](){server.sendHeader("Location","http://192.168.4.1",true);server.send(302,"text/plain","");});server.begin();
  Serial.println("CARRINHO ESP32 PRONTO | WiFi: tony | IP: http://192.168.4.1");
}
void loop(){
  dnsServer.processNextRequest();server.handleClient();static unsigned long ultimoSensor=0;if(millis()-ultimoSensor>=100){ultimoSensor=millis();distanciaAtual=medirDistancia();if(estado=="TRAS"&&distanciaAtual>0&&distanciaAtual<=DISTANCIA_BLOQUEIO_RE){parar();estado="RE_BLOQUEADA";Serial.print("[SEGURANCA] RE BLOQUEADA: ");Serial.print(distanciaAtual,1);Serial.println(" cm");}lerLuminosidade();static unsigned long ultimoLogLDR=0;if(millis()-ultimoLogLDR>=INTERVALO_LOG_LDR){ultimoLogLDR=millis();Serial.print("[LDR] ADC=");Serial.print(leituraLDR);Serial.print(" | Luz=");Serial.print(luminosidade);Serial.print("% | Ambiente=");if(luminosidade>=65)Serial.println("DIA");else if(luminosidade>=30)Serial.println("MEIA-LUZ");else Serial.println("NOITE");}}
}
