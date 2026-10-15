#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <math.h>
#include "model_auto_v3.h"

// ============================================================
// CUBI-04 AUTONOMO V3
//
// PRINCIPAIS AJUSTES:
//
// 1) Se HC-SR04 retornar sem eco:
//      interface mostra "--".
//
// 2) Sem eco + ultima leitura NAO perigosa:
//      auto considera 150 cm = livre.
//      pode acelerar.
//
// 3) Se ultima leitura estava vermelha/perigosa e depois vira "--":
//      durante alguns segundos continua considerando o obstaculo perto.
//      O carrinho continua tentando sair daquele ponto.
//
// 4) O modo automatico usa:
//      frente esquerda + frente centro + frente direita
//      para escolher o caminho.
//
// 5) O sensor traseiro entra nas decisoes de re.
//
// 6) MODO MANUAL:
//      NAO existe bloqueio por sensor.
//      NAO existe limitacao artificial de velocidade.
//      PWM manual configuravel de 0 ate 255.
//      Os sensores apenas geram ALERTA SONORO no navegador.
//
// 7) AUTOMATICO:
//      velocidade base + velocidade livre.
//      Se estiver muito livre / sem eco seguro, acelera.
//      Parede/canto -> re + giro + nova avaliacao.
//      Ate varias tentativas de escape.
//
// ATENCAO:
// O comportamento "-- = livre" foi implementado conforme solicitado.
// Um HC-SR04 tambem pode perder eco por angulo/material/falha.
// Teste primeiro com rodas suspensas e depois em baixa velocidade.
// ============================================================


// ============================================================
// WIFI
// ============================================================

const char* ssid = "tony";
const char* password = "stark369";

IPAddress local_IP(
  192,
  168,
  4,
  1
);

IPAddress gateway(
  192,
  168,
  4,
  1
);

IPAddress subnet(
  255,
  255,
  255,
  0
);

WebServer server(80);
DNSServer dnsServer;


// ============================================================
// L298N
// ============================================================

#define ENA 25
#define IN1 26
#define IN2 27

#define ENB 33
#define IN3 32
#define IN4 23


// ============================================================
// ULTRASSONICOS
// ============================================================

#define TRIG_ESQ 16
#define ECHO_ESQ 17

#define TRIG_CENTRO 18
#define ECHO_CENTRO 19

#define TRIG_DIR 4
#define ECHO_DIR 36

#define TRIG_TRAS 14
#define ECHO_TRAS 13


// ============================================================
// LDR
// ============================================================

#define LDR_PIN 34


// ============================================================
// CONFIGURACOES
// ============================================================

// Manual: sem limitacao artificial.
// PWM pode ir de 0 a 255.
int velocidadeManual = 100;

// Automatico.
int velocidadeAutoBase = 65;
int velocidadeAutoLivre = 110;

// Pode ser configurada no front de 8 a 40 cm.
float distanciaDesvioCm = 16.0f;

// Distancia absoluta usada pelas manobras automaticas.
const float DISTANCIA_EMERGENCIA_CM = 8.0f;

// Se a ultima leitura estava na faixa vermelha e depois vira "--",
// a memoria de perigo permanece por esse tempo.
const unsigned long MEMORIA_PERIGO_MS = 2500;

// Tempos de manobra.
unsigned long tempoReBaseMs = 420;
unsigned long tempoGiroBaseMs = 430;

// IA.
float confiancaMinima = 0.55f;

// Evita ficar eternamente tentando escapar.
int maxTentativasEscape = 6;


// ============================================================
// SENSOR COM MEMORIA
// ============================================================

struct SensorEstado {
  int trig;
  int echo;

  // O valor mostrado na interface.
  // -1 significa "--".
  float bruto;

  // Ultima leitura realmente valida.
  float ultimaValida;

  unsigned long ultimaValidaMs;

  // Enquanto ativo, um "--" continua sendo tratado
  // como obstaculo proximo.
  unsigned long perigoAteMs;
};


SensorEstado sensorEsq = {
  TRIG_ESQ,
  ECHO_ESQ,
  -1.0f,
  150.0f,
  0,
  0
};

SensorEstado sensorCentro = {
  TRIG_CENTRO,
  ECHO_CENTRO,
  -1.0f,
  150.0f,
  0,
  0
};

SensorEstado sensorDir = {
  TRIG_DIR,
  ECHO_DIR,
  -1.0f,
  150.0f,
  0,
  0
};

SensorEstado sensorTras = {
  TRIG_TRAS,
  ECHO_TRAS,
  -1.0f,
  150.0f,
  0,
  0
};


unsigned long ultimoCicloSensores = 0;


// ============================================================
// LDR
// ============================================================

int leituraLDR = 0;
int luminosidade = 0;


// ============================================================
// TINYML
// ============================================================

int classeML = 4;
float confiancaML = 0.0f;

float probabilidadesML[
  MODEL_OUTPUTS
] = {
  0,
  0,
  0,
  0,
  0
};


// ============================================================
// ESTADO DO CARRO
// ============================================================

enum EstadoAuto {
  AUTO_DESLIGADO,
  AUTO_DECIDINDO,
  AUTO_FRENTE,
  AUTO_RECUANDO,
  AUTO_GIRANDO_ESQ,
  AUTO_GIRANDO_DIR,
  AUTO_SEM_SAIDA
};


EstadoAuto estadoAuto =
  AUTO_DESLIGADO;

bool modoAutomatico =
  false;

String estadoMotor =
  "PARADO";

String motivoAuto =
  "DESLIGADO";

unsigned long fimManobraMs =
  0;

unsigned long caminhoLivreDesdeMs =
  0;

int tentativasEscape =
  0;

bool ultimaPreferenciaEsquerda =
  false;


// ============================================================
// FUNCOES DE SENSOR
// ============================================================

float limiarVermelho() {
  // Mesmo se o usuario colocar desvio em 8 cm,
  // o painel continua avisando a partir de pelo menos 12 cm.
  return max(
    distanciaDesvioCm,
    12.0f
  );
}


bool memoriaPerigoAtiva(
  const SensorEstado& sensor
) {
  return (
    sensor.perigoAteMs != 0
    &&
    (long)(
      sensor.perigoAteMs -
      millis()
    ) > 0
  );
}


float distanciaEfetiva(
  const SensorEstado& sensor
) {
  // Tem leitura real.
  if (
    sensor.bruto > 0
  ) {
    return min(
      sensor.bruto,
      150.0f
    );
  }

  // "--" logo depois de leitura perigosa:
  // continua lembrando o obstaculo.
  if (
    memoriaPerigoAtiva(
      sensor
    )
  ) {
    return min(
      sensor.ultimaValida,
      limiarVermelho()
    );
  }

  // "--" sem perigo recente:
  // conforme solicitado, considera LIVRE.
  return 150.0f;
}


bool semEcoLivre(
  const SensorEstado& sensor
) {
  return (
    sensor.bruto < 0
    &&
    !memoriaPerigoAtiva(
      sensor
    )
  );
}


float medirUmaVez(
  int trig,
  int echo
) {
  digitalWrite(
    trig,
    LOW
  );

  delayMicroseconds(
    3
  );

  digitalWrite(
    trig,
    HIGH
  );

  delayMicroseconds(
    10
  );

  digitalWrite(
    trig,
    LOW
  );


  unsigned long duracao =
    pulseIn(
      echo,
      HIGH,
      24000
    );


  if (
    duracao == 0
  ) {
    return -1.0f;
  }


  float distancia =
    duracao *
    0.0343f /
    2.0f;


  if (
    distancia < 2.0f
    ||
    distancia > 400.0f
  ) {
    return -1.0f;
  }


  return distancia;
}


void atualizarSensor(
  SensorEstado& sensor
) {
  float leitura =
    medirUmaVez(
      sensor.trig,
      sensor.echo
    );


  if (
    leitura > 0
  ) {
    sensor.bruto =
      leitura;

    sensor.ultimaValida =
      leitura;

    sensor.ultimaValidaMs =
      millis();


    if (
      leitura <=
      limiarVermelho()
    ) {
      sensor.perigoAteMs =
        millis() +
        MEMORIA_PERIGO_MS;
    }

    else {
      // Uma leitura valida e segura
      // confirma que o obstaculo saiu.
      sensor.perigoAteMs =
        0;
    }
  }

  else {
    // Interface deve mostrar "--".
    // Nao apagamos a memoria de perigo aqui.
    sensor.bruto =
      -1.0f;
  }
}


void atualizarUltrassonicos() {
  static byte sensorAtual =
    0;

  static unsigned long
    ultimaLeituraMs =
      0;


  if (
    millis() -
    ultimaLeituraMs <
    38
  ) {
    return;
  }


  ultimaLeituraMs =
    millis();


  switch (
    sensorAtual
  ) {
    case 0:
      atualizarSensor(
        sensorEsq
      );
      break;

    case 1:
      atualizarSensor(
        sensorCentro
      );
      break;

    case 2:
      atualizarSensor(
        sensorDir
      );
      break;

    case 3:
      atualizarSensor(
        sensorTras
      );

      ultimoCicloSensores =
        millis();

      break;
  }


  sensorAtual++;


  if (
    sensorAtual >= 4
  ) {
    sensorAtual =
      0;
  }
}


bool sensoresAtualizando() {
  return (
    ultimoCicloSensores > 0
    &&
    millis() -
    ultimoCicloSensores <
    1000
  );
}


// ============================================================
// LDR
// ============================================================

void atualizarLDR() {
  static unsigned long
    ultimoLDR =
      0;


  if (
    millis() -
    ultimoLDR <
    300
  ) {
    return;
  }


  ultimoLDR =
    millis();


  long soma =
    0;


  for (
    int i = 0;
    i < 5;
    i++
  ) {
    soma +=
      analogRead(
        LDR_PIN
      );
  }


  leituraLDR =
    soma /
    5;


  luminosidade =
    map(
      leituraLDR,
      0,
      4095,
      0,
      100
    );


  luminosidade =
    constrain(
      luminosidade,
      0,
      100
    );
}


// ============================================================
// INFERENCIA MLP
// ============================================================

void executarInferencia() {
  static unsigned long
    ultimaInferencia =
      0;


  if (
    millis() -
    ultimaInferencia <
    160
  ) {
    return;
  }


  ultimaInferencia =
    millis();


  float x[
    MODEL_INPUTS
  ] = {
    distanciaEfetiva(
      sensorEsq
    ) /
      MODEL_NORMALIZACAO_CM,

    distanciaEfetiva(
      sensorCentro
    ) /
      MODEL_NORMALIZACAO_CM,

    distanciaEfetiva(
      sensorDir
    ) /
      MODEL_NORMALIZACAO_CM,

    distanciaEfetiva(
      sensorTras
    ) /
      MODEL_NORMALIZACAO_CM
  };


  float h1[
    MODEL_H1
  ];


  for (
    int j = 0;
    j < MODEL_H1;
    j++
  ) {
    float soma =
      MODEL_B1[j];


    for (
      int i = 0;
      i < MODEL_INPUTS;
      i++
    ) {
      soma +=
        x[i] *
        MODEL_W1[i][j];
    }


    h1[j] =
      soma > 0
        ? soma
        : 0;
  }


  float h2[
    MODEL_H2
  ];


  for (
    int j = 0;
    j < MODEL_H2;
    j++
  ) {
    float soma =
      MODEL_B2[j];


    for (
      int i = 0;
      i < MODEL_H1;
      i++
    ) {
      soma +=
        h1[i] *
        MODEL_W2[i][j];
    }


    h2[j] =
      soma > 0
        ? soma
        : 0;
  }


  float logits[
    MODEL_OUTPUTS
  ];


  for (
    int j = 0;
    j < MODEL_OUTPUTS;
    j++
  ) {
    float soma =
      MODEL_B3[j];


    for (
      int i = 0;
      i < MODEL_H2;
      i++
    ) {
      soma +=
        h2[i] *
        MODEL_W3[i][j];
    }


    logits[j] =
      soma;
  }


  float maior =
    logits[0];


  for (
    int i = 1;
    i < MODEL_OUTPUTS;
    i++
  ) {
    if (
      logits[i] >
      maior
    ) {
      maior =
        logits[i];
    }
  }


  float somaExp =
    0.0f;


  for (
    int i = 0;
    i < MODEL_OUTPUTS;
    i++
  ) {
    probabilidadesML[i] =
      expf(
        logits[i] -
        maior
      );


    somaExp +=
      probabilidadesML[i];
  }


  classeML =
    0;

  confiancaML =
    0.0f;


  for (
    int i = 0;
    i < MODEL_OUTPUTS;
    i++
  ) {
    probabilidadesML[i] /=
      somaExp;


    if (
      probabilidadesML[i] >
      confiancaML
    ) {
      confiancaML =
        probabilidadesML[i];

      classeML =
        i;
    }
  }
}


// ============================================================
// MOTORES
// ============================================================

void aplicarPWM(
  int pwm
) {
  // 0..255 para permitir controle manual completo.
  pwm =
    constrain(
      pwm,
      0,
      255
    );


  analogWrite(
    ENA,
    pwm
  );


  analogWrite(
    ENB,
    pwm
  );
}


void pararMotores() {
  digitalWrite(
    IN1,
    LOW
  );

  digitalWrite(
    IN2,
    LOW
  );

  digitalWrite(
    IN3,
    LOW
  );

  digitalWrite(
    IN4,
    LOW
  );


  analogWrite(
    ENA,
    0
  );

  analogWrite(
    ENB,
    0
  );


  estadoMotor =
    "PARADO";
}


void motorFrente(
  int pwm
) {
  digitalWrite(
    IN1,
    HIGH
  );

  digitalWrite(
    IN2,
    LOW
  );

  digitalWrite(
    IN3,
    HIGH
  );

  digitalWrite(
    IN4,
    LOW
  );


  aplicarPWM(
    pwm
  );


  estadoMotor =
    "FRENTE";
}


void motorRe(
  int pwm
) {
  digitalWrite(
    IN1,
    LOW
  );

  digitalWrite(
    IN2,
    HIGH
  );

  digitalWrite(
    IN3,
    LOW
  );

  digitalWrite(
    IN4,
    HIGH
  );


  aplicarPWM(
    pwm
  );


  estadoMotor =
    "RE";
}


void motorEsquerda(
  int pwm
) {
  digitalWrite(
    IN1,
    LOW
  );

  digitalWrite(
    IN2,
    HIGH
  );

  digitalWrite(
    IN3,
    HIGH
  );

  digitalWrite(
    IN4,
    LOW
  );


  aplicarPWM(
    pwm
  );


  estadoMotor =
    "GIRO_ESQUERDA";
}


void motorDireita(
  int pwm
) {
  digitalWrite(
    IN1,
    HIGH
  );

  digitalWrite(
    IN2,
    LOW
  );

  digitalWrite(
    IN3,
    LOW
  );

  digitalWrite(
    IN4,
    HIGH
  );


  aplicarPWM(
    pwm
  );


  estadoMotor =
    "GIRO_DIREITA";
}


// ============================================================
// ANALISE DOS 3 SENSORES FRONTAIS
// ============================================================

float distanciaEsq() {
  return distanciaEfetiva(
    sensorEsq
  );
}


float distanciaCentro() {
  return distanciaEfetiva(
    sensorCentro
  );
}


float distanciaDir() {
  return distanciaEfetiva(
    sensorDir
  );
}


float distanciaTras() {
  return distanciaEfetiva(
    sensorTras
  );
}


float scoreFrente() {
  float e =
    distanciaEsq();

  float c =
    distanciaCentro();

  float d =
    distanciaDir();


  return (
    0.20f * e +
    0.60f * c +
    0.20f * d
  );
}


float scoreEsquerda() {
  float e =
    distanciaEsq();

  float c =
    distanciaCentro();

  float d =
    distanciaDir();


  return (
    0.65f * e +
    0.25f * c +
    0.10f * d
  );
}


float scoreDireita() {
  float e =
    distanciaEsq();

  float c =
    distanciaCentro();

  float d =
    distanciaDir();


  return (
    0.10f * e +
    0.25f * c +
    0.65f * d
  );
}


bool escolherEsquerda() {
  float se =
    scoreEsquerda();

  float sd =
    scoreDireita();


  if (
    se >
    sd + 5.0f
  ) {
    ultimaPreferenciaEsquerda =
      true;

    return true;
  }


  if (
    sd >
    se + 5.0f
  ) {
    ultimaPreferenciaEsquerda =
      false;

    return false;
  }


  // Quase empatado:
  // alterna para nao entrar num loop sempre igual.
  ultimaPreferenciaEsquerda =
    !ultimaPreferenciaEsquerda;


  return
    ultimaPreferenciaEsquerda;
}


bool paredeFrontal() {
  float e =
    distanciaEsq();

  float c =
    distanciaCentro();

  float d =
    distanciaDir();


  float maior =
    max(
      c,
      max(
        e,
        d
      )
    );


  float menor =
    min(
      c,
      min(
        e,
        d
      )
    );


  return (
    c <
      distanciaDesvioCm +
      8.0f
    &&
    maior -
      menor <
      15.0f
  );
}


bool corredorEstreito() {
  float e =
    distanciaEsq();

  float c =
    distanciaCentro();

  float d =
    distanciaDir();


  return (
    e <
      distanciaDesvioCm
    &&
    d <
      distanciaDesvioCm
    &&
    c >
      distanciaDesvioCm *
      2.2f
  );
}


bool frenteEmergencia() {
  float e =
    distanciaEsq();

  float c =
    distanciaCentro();

  float d =
    distanciaDir();


  return (
    c <=
      DISTANCIA_EMERGENCIA_CM
    ||
    (
      e <=
        DISTANCIA_EMERGENCIA_CM
      &&
      d <=
        DISTANCIA_EMERGENCIA_CM
    )
  );
}


// ============================================================
// VELOCIDADE AUTOMATICA DINAMICA
// ============================================================

int velocidadeAutomaticaFrente() {
  float e =
    distanciaEsq();

  float c =
    distanciaCentro();

  float d =
    distanciaDir();


  int semEcoLivreFrente =
    0;


  if (
    semEcoLivre(
      sensorEsq
    )
  ) {
    semEcoLivreFrente++;
  }


  if (
    semEcoLivre(
      sensorCentro
    )
  ) {
    semEcoLivreFrente++;
  }


  if (
    semEcoLivre(
      sensorDir
    )
  ) {
    semEcoLivreFrente++;
  }


  // 2 ou 3 sensores sem eco, sem memoria vermelha:
  // considera area bem livre e pode acelerar.
  if (
    semEcoLivreFrente >= 2
    &&
    c >
      distanciaDesvioCm +
      8.0f
  ) {
    return
      velocidadeAutoLivre;
  }


  // Muito espaço.
  if (
    c >= 90.0f
    &&
    e >= 35.0f
    &&
    d >= 35.0f
  ) {
    return
      velocidadeAutoLivre;
  }


  // Espaço confortável.
  if (
    c >= 60.0f
  ) {
    return (
      velocidadeAutoBase +
      velocidadeAutoLivre
    ) /
    2;
  }


  // Próximo da região de decisão.
  if (
    c <=
      distanciaDesvioCm +
      10.0f
  ) {
    return max(
      35,
      velocidadeAutoBase -
      20
    );
  }


  return
    velocidadeAutoBase;
}


// ============================================================
// MANOBRAS AUTOMATICAS
// ============================================================

void iniciarGiro(
  bool esquerda
) {
  unsigned long extra =
    min(
      tentativasEscape *
      90,
      540
    );


  fimManobraMs =
    millis() +
    tempoGiroBaseMs +
    extra;


  if (
    esquerda
  ) {
    motorEsquerda(
      velocidadeAutoBase
    );

    estadoAuto =
      AUTO_GIRANDO_ESQ;

    motivoAuto =
      "GIRANDO_ESQUERDA";
  }

  else {
    motorDireita(
      velocidadeAutoBase
    );

    estadoAuto =
      AUTO_GIRANDO_DIR;

    motivoAuto =
      "GIRANDO_DIREITA";
  }
}


void pararSemSaida(
  const String& motivo
) {
  pararMotores();

  estadoAuto =
    AUTO_SEM_SAIDA;

  motivoAuto =
    motivo;
}


void iniciarRecuo() {
  float t =
    distanciaTras();


  // Se a ultima traseira estava vermelha e virou "--",
  // distanciaTras() continua retornando perigo.
  if (
    t <=
    DISTANCIA_EMERGENCIA_CM
  ) {
    // Nao consegue recuar.
    // Tenta pivotar usando os tres sensores frontais.
    if (
      max(
        distanciaEsq(),
        distanciaDir()
      ) >
      DISTANCIA_EMERGENCIA_CM +
      3.0f
    ) {
      tentativasEscape++;

      iniciarGiro(
        escolherEsquerda()
      );
    }

    else {
      pararSemSaida(
        "FRENTE_E_TRAS_BLOQUEADOS"
      );
    }

    return;
  }


  tentativasEscape++;


  if (
    tentativasEscape >
    maxTentativasEscape
  ) {
    pararSemSaida(
      "MAX_TENTATIVAS_ESCAPE"
    );

    return;
  }


  unsigned long extra =
    min(
      tentativasEscape *
      90,
      450
    );


  fimManobraMs =
    millis() +
    tempoReBaseMs +
    extra;


  motorRe(
    velocidadeAutoBase
  );


  estadoAuto =
    AUTO_RECUANDO;


  motivoAuto =
    "RECUANDO_PARA_ESCAPAR";
}


void ligarAutomatico() {
  pararMotores();

  modoAutomatico =
    true;

  estadoAuto =
    AUTO_DECIDINDO;

  motivoAuto =
    "INICIANDO";

  tentativasEscape =
    0;

  caminhoLivreDesdeMs =
    0;
}


void desligarAutomatico(
  const String& motivo
) {
  modoAutomatico =
    false;

  estadoAuto =
    AUTO_DESLIGADO;

  motivoAuto =
    motivo;

  pararMotores();
}


// ============================================================
// PLANEJADOR AUTONOMO
// ============================================================

void executarAutomatico() {
  if (
    !modoAutomatico
  ) {
    return;
  }


  if (
    !sensoresAtualizando()
  ) {
    pararSemSaida(
      "SENSORES_NAO_ATUALIZAM"
    );

    return;
  }


  float e =
    distanciaEsq();

  float c =
    distanciaCentro();

  float d =
    distanciaDir();

  float t =
    distanciaTras();


  // ----------------------------------------------------------
  // RE
  // ----------------------------------------------------------

  if (
    estadoAuto ==
    AUTO_RECUANDO
  ) {
    if (
      t <=
      DISTANCIA_EMERGENCIA_CM
    ) {
      pararMotores();

      iniciarGiro(
        escolherEsquerda()
      );

      motivoAuto =
        "TRASEIRA_PROXIMA_VIRANDO";

      return;
    }


    if (
      millis() >=
      fimManobraMs
    ) {
      pararMotores();

      // Depois da re, usa novamente os 3 sensores
      // para escolher a melhor direção.
      iniciarGiro(
        escolherEsquerda()
      );

      motivoAuto =
        "RE_FINALIZADA_ESCOLHENDO_LADO";
    }


    return;
  }


  // ----------------------------------------------------------
  // GIRO
  // ----------------------------------------------------------

  if (
    estadoAuto ==
    AUTO_GIRANDO_ESQ
  ) {
    if (
      e <=
      DISTANCIA_EMERGENCIA_CM
    ) {
      pararMotores();

      estadoAuto =
        AUTO_DECIDINDO;

      motivoAuto =
        "ESQUERDA_FICOU_PROXIMA";
    }

    else if (
      millis() >=
      fimManobraMs
    ) {
      pararMotores();

      estadoAuto =
        AUTO_DECIDINDO;

      motivoAuto =
        "REAVALIANDO_APOS_GIRO";
    }


    return;
  }


  if (
    estadoAuto ==
    AUTO_GIRANDO_DIR
  ) {
    if (
      d <=
      DISTANCIA_EMERGENCIA_CM
    ) {
      pararMotores();

      estadoAuto =
        AUTO_DECIDINDO;

      motivoAuto =
        "DIREITA_FICOU_PROXIMA";
    }

    else if (
      millis() >=
      fimManobraMs
    ) {
      pararMotores();

      estadoAuto =
        AUTO_DECIDINDO;

      motivoAuto =
        "REAVALIANDO_APOS_GIRO";
    }


    return;
  }


  // ----------------------------------------------------------
  // SEM SAIDA
  // ----------------------------------------------------------

  if (
    estadoAuto ==
    AUTO_SEM_SAIDA
  ) {
    // Se aparecer qualquer abertura nova, tenta novamente.
    if (
      c >
        distanciaDesvioCm +
        8.0f
      ||
      e >
        distanciaDesvioCm +
        8.0f
      ||
      d >
        distanciaDesvioCm +
        8.0f
      ||
      t >
        distanciaDesvioCm +
        8.0f
    ) {
      tentativasEscape =
        0;

      estadoAuto =
        AUTO_DECIDINDO;

      motivoAuto =
        "ABERTURA_DETECTADA";
    }


    return;
  }


  estadoAuto =
    AUTO_DECIDINDO;


  // ----------------------------------------------------------
  // 1. PERIGO MUITO PROXIMO
  // ----------------------------------------------------------

  if (
    frenteEmergencia()
  ) {
    pararMotores();

    motivoAuto =
      "FRENTE_MUITO_PROXIMA";

    iniciarRecuo();

    return;
  }


  // ----------------------------------------------------------
  // 2. PAREDE / CANTO SEM DIRECAO CLARA
  // ----------------------------------------------------------

  if (
    paredeFrontal()
  ) {
    pararMotores();

    motivoAuto =
      "PAREDE_FRONTAL";

    iniciarRecuo();

    return;
  }


  // ----------------------------------------------------------
  // 3. CORREDOR ESTREITO
  // ----------------------------------------------------------

  if (
    corredorEstreito()
  ) {
    motorFrente(
      max(
        35,
        velocidadeAutoBase -
        20
      )
    );

    estadoAuto =
      AUTO_FRENTE;

    motivoAuto =
      "CORREDOR_FRENTE_LENTA";

    return;
  }


  // ----------------------------------------------------------
  // 4. CENTRO BLOQUEADO
  //    Usa esquerda + centro + direita para escolher.
  // ----------------------------------------------------------

  if (
    c <=
    distanciaDesvioCm
  ) {
    bool esquerdaMelhor =
      escolherEsquerda();


    float melhorLado =
      esquerdaMelhor
        ? e
        : d;


    if (
      melhorLado >
      distanciaDesvioCm +
      4.0f
    ) {
      pararMotores();

      iniciarGiro(
        esquerdaMelhor
      );

      motivoAuto =
        esquerdaMelhor
          ? "CENTRO_BLOQUEADO_VAI_ESQ"
          : "CENTRO_BLOQUEADO_VAI_DIR";
    }

    else {
      pararMotores();

      motivoAuto =
        "CENTRO_SEM_SAIDA_LATERAL";

      iniciarRecuo();
    }


    return;
  }


  // ----------------------------------------------------------
  // 5. OBSTACULO LATERAL
  // ----------------------------------------------------------

  if (
    e <=
    distanciaDesvioCm
    &&
    d >
      distanciaDesvioCm +
      4.0f
  ) {
    pararMotores();

    iniciarGiro(
      false
    );

    motivoAuto =
      "OBSTACULO_ESQUERDA";

    return;
  }


  if (
    d <=
    distanciaDesvioCm
    &&
    e >
      distanciaDesvioCm +
      4.0f
  ) {
    pararMotores();

    iniciarGiro(
      true
    );

    motivoAuto =
      "OBSTACULO_DIREITA";

    return;
  }


  // ----------------------------------------------------------
  // 6. CAMINHO CLARAMENTE LIVRE
  //
  // "--" sem memória vermelha vira 150 cm.
  // Aqui ele pode acelerar.
  // ----------------------------------------------------------

  if (
    c >
      distanciaDesvioCm +
      3.0f
    &&
    e >
      DISTANCIA_EMERGENCIA_CM
    &&
    d >
      DISTANCIA_EMERGENCIA_CM
  ) {
    int pwm =
      velocidadeAutomaticaFrente();


    motorFrente(
      pwm
    );


    estadoAuto =
      AUTO_FRENTE;


    if (
      pwm >=
      velocidadeAutoLivre
    ) {
      motivoAuto =
        "LIVRE_ACELERANDO";
    }

    else {
      motivoAuto =
        "FRENTE_LIVRE";
    }


    if (
      c >
      distanciaDesvioCm +
      15.0f
    ) {
      if (
        caminhoLivreDesdeMs ==
        0
      ) {
        caminhoLivreDesdeMs =
          millis();
      }


      if (
        millis() -
        caminhoLivreDesdeMs >
        1800
      ) {
        tentativasEscape =
          0;
      }
    }

    else {
      caminhoLivreDesdeMs =
        0;
    }


    return;
  }


  // ----------------------------------------------------------
  // 7. CASO AMBIGUO -> TINYML
  // ----------------------------------------------------------

  if (
    confiancaML >=
    confiancaMinima
  ) {
    switch (
      classeML
    ) {
      case 0:
        motorFrente(
          max(
            35,
            velocidadeAutoBase -
            10
          )
        );

        estadoAuto =
          AUTO_FRENTE;

        motivoAuto =
          "ML_FRENTE";
        break;


      case 1:
        pararMotores();

        iniciarGiro(
          true
        );

        motivoAuto =
          "ML_VIRAR_ESQUERDA";
        break;


      case 2:
        pararMotores();

        iniciarGiro(
          false
        );

        motivoAuto =
          "ML_VIRAR_DIREITA";
        break;


      case 3:
        pararMotores();

        iniciarRecuo();

        motivoAuto =
          "ML_RE";
        break;


      case 4:
      default:
        pararMotores();

        if (
          t >
          distanciaDesvioCm +
          4.0f
        ) {
          iniciarRecuo();

          motivoAuto =
            "ML_PARAR_TENTANDO_ESCAPE";
        }

        else {
          iniciarGiro(
            escolherEsquerda()
          );

          motivoAuto =
            "ML_PARAR_TENTANDO_GIRO";
        }

        break;
    }


    return;
  }


  // ----------------------------------------------------------
  // 8. IA SEM CONFIANCA:
  //    usa score dos 3 sensores frontais.
  // ----------------------------------------------------------

  float sf =
    scoreFrente();

  float se =
    scoreEsquerda();

  float sd =
    scoreDireita();


  if (
    sf >= se
    &&
    sf >= sd
    &&
    c >
      distanciaDesvioCm
  ) {
    motorFrente(
      max(
        35,
        velocidadeAutoBase -
        15
      )
    );

    estadoAuto =
      AUTO_FRENTE;

    motivoAuto =
      "SCORE_FRENTE";
  }

  else if (
    se >= sd
  ) {
    pararMotores();

    iniciarGiro(
      true
    );

    motivoAuto =
      "SCORE_ESQUERDA";
  }

  else {
    pararMotores();

    iniciarGiro(
      false
    );

    motivoAuto =
      "SCORE_DIREITA";
  }
}


// ============================================================
// PAGINA WEB
// ============================================================

String pagina() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">

<head>

<meta charset="UTF-8">

<meta
  name="viewport"
  content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">

<title>CUBI-04 V3</title>

<style>

*{
  box-sizing:border-box;
  -webkit-user-select:none;
  user-select:none;
  -webkit-touch-callout:none;
  -webkit-tap-highlight-color:transparent
}

body{
  margin:0;
  padding:14px;
  background:#101010;
  color:#fff;
  font-family:Arial,sans-serif;
  text-align:center
}

h1{
  margin:4px 0
}

.sub{
  color:#aaa;
  margin-bottom:8px
}

.card{
  max-width:470px;
  margin:10px auto;
  padding:14px;
  background:#1d1d1d;
  border-radius:17px
}

.online{
  display:inline-block;
  margin:5px;
  padding:7px 14px;
  border-radius:20px;
  background:#143d20;
  color:#56ff7c;
  font-weight:bold
}

.grid{
  display:grid;
  grid-template-columns:1fr 1fr;
  gap:8px
}

.sensor{
  min-height:105px;
  padding:10px;
  border-radius:13px;
  background:#292929;
  border:2px solid transparent
}

.sensor b{
  display:block;
  font-size:27px;
  margin:5px 0
}

.sensor small{
  display:block;
  color:#aaa
}

.sensor.perigo{
  background:#7b1b1b;
  border-color:#ff5252
}

.sensor.atencao{
  background:#614213;
  border-color:#ff9800
}

.sensor.semeco{
  background:#173d25;
  border-color:#3cbf69
}

.ml{
  padding:12px;
  background:#132a43;
  border-radius:13px
}

.big{
  font-size:21px;
  font-weight:bold;
  margin:6px 0
}

.row{
  display:flex;
  justify-content:space-between;
  align-items:center;
  gap:10px;
  margin:8px 0
}

input[type=range]{
  width:100%
}

.auto{
  width:100%;
  padding:15px;
  border:0;
  border-radius:14px;
  background:#2e7d32;
  color:#fff;
  font-size:17px;
  font-weight:bold
}

.auto.on{
  background:#c62828
}

.emerg{
  width:100%;
  margin-top:8px;
  padding:14px;
  border:0;
  border-radius:14px;
  background:#b71c1c;
  color:#fff;
  font-size:16px;
  font-weight:bold
}

.sound{
  width:100%;
  margin-top:8px;
  padding:12px;
  border:0;
  border-radius:12px;
  background:#1565c0;
  color:#fff;
  font-size:15px;
  font-weight:bold
}

.info{
  color:#aaa;
  font-size:12px;
  margin-top:7px
}

.controle{
  display:grid;
  grid-template-columns:88px 88px 88px;
  grid-template-rows:88px 88px 88px;
  gap:9px;
  justify-content:center;
  touch-action:none;
  margin-top:10px
}

.controle button{
  border:0;
  border-radius:19px;
  background:#333;
  color:#fff;
  font-size:31px;
  touch-action:none
}

.f{
  grid-column:2;
  grid-row:1
}

.e{
  grid-column:1;
  grid-row:2
}

.p{
  grid-column:2;
  grid-row:2;
  background:#c62828!important;
  font-size:13px!important;
  font-weight:bold
}

.d{
  grid-column:3;
  grid-row:2
}

.r{
  grid-column:2;
  grid-row:3
}

</style>

</head>


<body>

<h1>CUBI-04 V3</h1>

<div class="sub">
TinyML + planejamento autonomo
</div>

<div
  id="online"
  class="online">

● ONLINE

</div>


<div class="card">

  <button
    id="btnAuto"
    class="auto"
    onclick="toggleAuto()">

    LIGAR AUTOMATICO

  </button>


  <button
    class="emerg"
    onclick="emergencia()">

    PARADA DE EMERGENCIA

  </button>


  <button
    id="btnSom"
    class="sound"
    onclick="toggleSom()">

    🔇 ATIVAR SOM

  </button>


  <div class="info">
    No modo manual, sensores apenas avisam por som.
    Eles nao bloqueiam os comandos.
  </div>

</div>


<div class="card">

  <div class="ml">

    <div>
      TinyML
    </div>

    <div
      id="classe"
      class="big">

      --

    </div>

    <div>
      Confianca:
      <b id="confianca">
        --
      </b>%
    </div>

  </div>


  <div style="margin-top:10px">
    Planejador:
    <b id="motivo">--</b>
  </div>

  <div>
    Motor:
    <b id="motor">PARADO</b>
  </div>

  <div>
    Tentativas:
    <b id="tentativas">0</b>
  </div>

</div>


<div class="card">

  <div class="grid">

    <div
      id="boxEsq"
      class="sensor">

      Frente esquerda

      <b id="esq">
        --
      </b>

      <small id="infoEsq">
        --
      </small>

    </div>


    <div
      id="boxCentro"
      class="sensor">

      Frente centro

      <b id="centro">
        --
      </b>

      <small id="infoCentro">
        --
      </small>

    </div>


    <div
      id="boxDir"
      class="sensor">

      Frente direita

      <b id="dir">
        --
      </b>

      <small id="infoDir">
        --
      </small>

    </div>


    <div
      id="boxTras"
      class="sensor">

      Traseiro

      <b id="tras">
        --
      </b>

      <small id="infoTras">
        --
      </small>

    </div>

  </div>

</div>


<div class="card">

  <div class="big">
    AUTOMATICO
  </div>


  <div class="row">
    <span>Velocidade base</span>
    <b id="baseTxt">65</b>
  </div>

  <input
    id="base"
    type="range"
    min="35"
    max="160"
    value="65">


  <div class="row">
    <span>Velocidade livre</span>
    <b id="livreTxt">110</b>
  </div>

  <input
    id="livre"
    type="range"
    min="50"
    max="220"
    value="110">


  <div class="row">
    <span>Distancia de desvio</span>
    <b>
      <span id="distTxt">16</span>
      cm
    </b>
  </div>

  <input
    id="dist"
    type="range"
    min="8"
    max="40"
    value="16">


  <div class="row">
    <span>Tempo de re</span>
    <b>
      <span id="reTxt">420</span>
      ms
    </b>
  </div>

  <input
    id="tempoRe"
    type="range"
    min="200"
    max="1200"
    step="20"
    value="420">


  <div class="row">
    <span>Tempo de giro</span>
    <b>
      <span id="giroTxt">430</span>
      ms
    </b>
  </div>

  <input
    id="tempoGiro"
    type="range"
    min="200"
    max="1400"
    step="20"
    value="430">


  <div class="row">
    <span>Confianca minima</span>
    <b>
      <span id="confTxt">55</span>%
    </b>
  </div>

  <input
    id="confMin"
    type="range"
    min="35"
    max="90"
    value="55">

</div>


<div class="card">

  <div class="big">
    MANUAL
  </div>

  <div class="row">
    <span>Velocidade manual</span>
    <b>
      <span id="manualTxt">100</span>
      /255
    </b>
  </div>

  <input
    id="manualVel"
    type="range"
    min="0"
    max="255"
    value="100">


  <div class="info">
    Sem bloqueio por distancia.
    Ao tocar em um comando, o som e ativado automaticamente
    pelo navegador quando permitido.
  </div>


  <div class="controle">

    <button
      id="bf"
      class="f">
      ▲
    </button>

    <button
      id="be"
      class="e">
      ◀
    </button>

    <button
      id="bp"
      class="p">
      PARAR
    </button>

    <button
      id="bd"
      class="d">
      ▶
    </button>

    <button
      id="br"
      class="r">
      ▼
    </button>

  </div>

</div>


<script>

"use strict";

let autoLigado=false;

let somAtivo=false;
let audioCtx=null;
let timerSom=null;
let intervaloSomAtual=0;


// ------------------------------------------------------------
// Evita selecao/copia acidental no celular.
// ------------------------------------------------------------

document.addEventListener(
  "contextmenu",
  e=>e.preventDefault()
);

document.addEventListener(
  "selectstart",
  e=>e.preventDefault()
);

document.addEventListener(
  "dragstart",
  e=>e.preventDefault()
);


// ------------------------------------------------------------
// HTTP
// ------------------------------------------------------------

function req(url){
  return fetch(
    url,
    {
      cache:"no-store"
    }
  );
}


// ------------------------------------------------------------
// SOM
// ------------------------------------------------------------

function garantirAudio(){
  if(!audioCtx){
    audioCtx=
      new(
        window.AudioContext ||
        window.webkitAudioContext
      )();
  }

  if(
    audioCtx.state==="suspended"
  ){
    audioCtx.resume();
  }
}


function toggleSom(){
  garantirAudio();

  somAtivo=
    !somAtivo;

  document
    .getElementById("btnSom")
    .innerText=
      somAtivo
        ? "🔊 SOM ATIVO"
        : "🔇 ATIVAR SOM";

  if(!somAtivo){
    pararSom();
  }
}


function ativarSomPorGesto(){
  garantirAudio();

  somAtivo=true;

  document
    .getElementById("btnSom")
    .innerText=
      "🔊 SOM ATIVO";
}


function beep(){
  if(
    !somAtivo ||
    !audioCtx
  ){
    return;
  }

  const o=
    audioCtx.createOscillator();

  const g=
    audioCtx.createGain();

  o.frequency.value=980;
  g.gain.value=0.10;

  o.connect(g);
  g.connect(
    audioCtx.destination
  );

  o.start();

  setTimeout(
    ()=>{
      try{
        o.stop();
      }catch(_){}
    },
    75
  );
}


function pararSom(){
  if(timerSom){
    clearTimeout(
      timerSom
    );

    timerSom=null;
  }

  intervaloSomAtual=0;
}


function iniciarCicloSom(
  intervalo
){
  if(
    !somAtivo
  ){
    pararSom();
    return;
  }

  if(
    intervaloSomAtual===
    intervalo
    &&
    timerSom
  ){
    return;
  }

  pararSom();

  intervaloSomAtual=
    intervalo;

  function ciclo(){
    if(
      !somAtivo ||
      intervaloSomAtual===0
    ){
      return;
    }

    beep();

    timerSom=
      setTimeout(
        ciclo,
        intervaloSomAtual
      );
  }

  ciclo();
}


function atualizarSom(
  x
){
  if(
    !somAtivo
  ){
    pararSom();
    return;
  }

  let distancia=999;

  if(
    x.motor==="FRENTE"
  ){
    distancia=
      Math.min(
        x.efEsq,
        x.efCentro,
        x.efDir
      );
  }

  else if(
    x.motor==="RE"
  ){
    distancia=
      x.efTras;
  }

  else if(
    x.motor==="GIRO_ESQUERDA"
  ){
    distancia=
      Math.min(
        x.efEsq,
        x.efCentro
      );
  }

  else if(
    x.motor==="GIRO_DIREITA"
  ){
    distancia=
      Math.min(
        x.efDir,
        x.efCentro
      );
  }

  else{
    pararSom();
    return;
  }


  let intervalo=0;

  if(distancia<=8){
    intervalo=110;
  }

  else if(distancia<=12){
    intervalo=180;
  }

  else if(distancia<=18){
    intervalo=300;
  }

  else if(distancia<=30){
    intervalo=600;
  }

  else{
    pararSom();
    return;
  }

  iniciarCicloSom(
    intervalo
  );
}


// ------------------------------------------------------------
// AUTO
// ------------------------------------------------------------

function toggleAuto(){
  req(
    "/auto?on="+
    (
      autoLigado
        ? 0
        : 1
    )
  );
}


function emergencia(){
  req(
    "/emergencia"
  );
}


// ------------------------------------------------------------
// CONFIG
// ------------------------------------------------------------

const base=
  document.getElementById("base");

const livre=
  document.getElementById("livre");

const dist=
  document.getElementById("dist");

const tempoRe=
  document.getElementById("tempoRe");

const tempoGiro=
  document.getElementById("tempoGiro");

const confMin=
  document.getElementById("confMin");

const manualVel=
  document.getElementById("manualVel");


function enviarConfig(){
  req(
    "/config?base="+
    base.value+
    "&livre="+
    livre.value+
    "&dist="+
    dist.value+
    "&re="+
    tempoRe.value+
    "&giro="+
    tempoGiro.value+
    "&conf="+
    confMin.value+
    "&manual="+
    manualVel.value
  );
}


base.addEventListener(
  "input",
  ()=>{
    document
      .getElementById("baseTxt")
      .innerText=
        base.value;
  }
);

livre.addEventListener(
  "input",
  ()=>{
    document
      .getElementById("livreTxt")
      .innerText=
        livre.value;
  }
);

dist.addEventListener(
  "input",
  ()=>{
    document
      .getElementById("distTxt")
      .innerText=
        dist.value;
  }
);

tempoRe.addEventListener(
  "input",
  ()=>{
    document
      .getElementById("reTxt")
      .innerText=
        tempoRe.value;
  }
);

tempoGiro.addEventListener(
  "input",
  ()=>{
    document
      .getElementById("giroTxt")
      .innerText=
        tempoGiro.value;
  }
);

confMin.addEventListener(
  "input",
  ()=>{
    document
      .getElementById("confTxt")
      .innerText=
        confMin.value;
  }
);

manualVel.addEventListener(
  "input",
  ()=>{
    document
      .getElementById("manualTxt")
      .innerText=
        manualVel.value;
  }
);


[
  base,
  livre,
  dist,
  tempoRe,
  tempoGiro,
  confMin,
  manualVel
].forEach(
  elemento=>{
    elemento.addEventListener(
      "change",
      enviarConfig
    );
  }
);


// ------------------------------------------------------------
// MANUAL
// ------------------------------------------------------------

function configurarBotao(
  id,
  direcao
){
  const botao=
    document.getElementById(id);


  function iniciar(e){
    e.preventDefault();

    // Gesto do usuario libera audio no navegador.
    ativarSomPorGesto();

    try{
      botao.setPointerCapture(
        e.pointerId
      );
    }catch(_){}

    req(
      "/manual?dir="+
      direcao
    );
  }


  function finalizar(e){
    if(e){
      e.preventDefault();
    }

    req(
      "/manual?dir=parar"
    );
  }


  botao.addEventListener(
    "pointerdown",
    iniciar,
    {
      passive:false
    }
  );

  botao.addEventListener(
    "pointerup",
    finalizar,
    {
      passive:false
    }
  );

  botao.addEventListener(
    "pointercancel",
    finalizar,
    {
      passive:false
    }
  );
}


configurarBotao(
  "bf",
  "frente"
);

configurarBotao(
  "be",
  "esquerda"
);

configurarBotao(
  "bd",
  "direita"
);

configurarBotao(
  "br",
  "re"
);


document
  .getElementById("bp")
  .addEventListener(
    "pointerdown",
    e=>{
      e.preventDefault();

      req(
        "/manual?dir=parar"
      );
    },
    {
      passive:false
    }
  );


// ------------------------------------------------------------
// SENSORES
// ------------------------------------------------------------

function textoDistancia(
  bruto
){
  bruto=
    Number(bruto);

  if(
    bruto < 0
  ){
    return "--";
  }

  return bruto.toFixed(1);
}


function atualizarSensorUI(
  boxId,
  valorId,
  infoId,
  bruto,
  efetiva,
  memoria,
  limiteVermelho
){
  const box=
    document.getElementById(boxId);

  box.classList.remove(
    "perigo",
    "atencao",
    "semeco"
  );


  document
    .getElementById(valorId)
    .innerText=
      textoDistancia(
        bruto
      );


  const info=
    document.getElementById(infoId);


  if(
    Number(bruto) < 0
  ){
    if(memoria){
      box.classList.add(
        "perigo"
      );

      info.innerText=
        "SEM ECO + PERIGO MEMORIZADO";
    }

    else{
      box.classList.add(
        "semeco"
      );

      info.innerText=
        "SEM ECO = LIVRE";
    }

    return;
  }


  if(
    Number(bruto) <=
    Number(limiteVermelho)
  ){
    box.classList.add(
      "perigo"
    );

    info.innerText=
      "MUITO PROXIMO";
  }

  else if(
    Number(bruto) <=
    Number(limiteVermelho)+
    12
  ){
    box.classList.add(
      "atencao"
    );

    info.innerText=
      "ATENCAO";
  }

  else{
    info.innerText=
      "LIVRE";
  }
}


// ------------------------------------------------------------
// STATUS
// ------------------------------------------------------------

function atualizar(){
  req(
    "/status"
  )

  .then(
    r=>r.json()
  )

  .then(
    x=>{
      document
        .getElementById("online")
        .innerText=
          "● ONLINE";


      autoLigado=
        x.auto;


      const btn=
        document.getElementById("btnAuto");


      if(
        autoLigado
      ){
        btn.innerText=
          "DESLIGAR AUTOMATICO";

        btn.className=
          "auto on";
      }

      else{
        btn.innerText=
          "LIGAR AUTOMATICO";

        btn.className=
          "auto";
      }


      document
        .getElementById("classe")
        .innerText=
          x.classe;


      document
        .getElementById("confianca")
        .innerText=
          Number(
            x.confianca
          ).toFixed(1);


      document
        .getElementById("motivo")
        .innerText=
          x.motivo;


      document
        .getElementById("motor")
        .innerText=
          x.motor;


      document
        .getElementById("tentativas")
        .innerText=
          x.tentativas;


      atualizarSensorUI(
        "boxEsq",
        "esq",
        "infoEsq",
        x.esq,
        x.efEsq,
        x.memEsq,
        x.vermelho
      );


      atualizarSensorUI(
        "boxCentro",
        "centro",
        "infoCentro",
        x.centro,
        x.efCentro,
        x.memCentro,
        x.vermelho
      );


      atualizarSensorUI(
        "boxDir",
        "dir",
        "infoDir",
        x.dir,
        x.efDir,
        x.memDir,
        x.vermelho
      );


      atualizarSensorUI(
        "boxTras",
        "tras",
        "infoTras",
        x.tras,
        x.efTras,
        x.memTras,
        x.vermelho
      );


      base.value=
        x.base;

      livre.value=
        x.livre;

      dist.value=
        x.dist;

      tempoRe.value=
        x.tempoRe;

      tempoGiro.value=
        x.tempoGiro;

      confMin.value=
        x.confMin;

      manualVel.value=
        x.manual;


      document
        .getElementById("baseTxt")
        .innerText=
          x.base;

      document
        .getElementById("livreTxt")
        .innerText=
          x.livre;

      document
        .getElementById("distTxt")
        .innerText=
          x.dist;

      document
        .getElementById("reTxt")
        .innerText=
          x.tempoRe;

      document
        .getElementById("giroTxt")
        .innerText=
          x.tempoGiro;

      document
        .getElementById("confTxt")
        .innerText=
          x.confMin;

      document
        .getElementById("manualTxt")
        .innerText=
          x.manual;


      atualizarSom(
        x
      );
    }
  )

  .catch(
    ()=>{
      document
        .getElementById("online")
        .innerText=
          "● DESCONECTADO";
    }
  );
}


setInterval(
  atualizar,
  250
);

</script>

</body>

</html>
)rawliteral";
}


// ============================================================
// JSON
// ============================================================

void enviarStatus() {
  String json =
    "{";


  json +=
    "\"auto\":" +
    String(
      modoAutomatico
        ? "true"
        : "false"
    ) +
    ",";


  json +=
    "\"classe\":\"" +
    String(
      MODEL_ACTIONS[
        classeML
      ]
    ) +
    "\",";


  json +=
    "\"confianca\":" +
    String(
      confiancaML *
      100.0f,
      1
    ) +
    ",";


  json +=
    "\"motivo\":\"" +
    motivoAuto +
    "\",";


  json +=
    "\"motor\":\"" +
    estadoMotor +
    "\",";


  json +=
    "\"tentativas\":" +
    String(
      tentativasEscape
    ) +
    ",";


  // Brutos: -1 aparece como "--" no front.
  json +=
    "\"esq\":" +
    String(
      sensorEsq.bruto,
      1
    ) +
    ",";

  json +=
    "\"centro\":" +
    String(
      sensorCentro.bruto,
      1
    ) +
    ",";

  json +=
    "\"dir\":" +
    String(
      sensorDir.bruto,
      1
    ) +
    ",";

  json +=
    "\"tras\":" +
    String(
      sensorTras.bruto,
      1
    ) +
    ",";


  // Efetivos usados pelo planejador/IA/som.
  json +=
    "\"efEsq\":" +
    String(
      distanciaEsq(),
      1
    ) +
    ",";

  json +=
    "\"efCentro\":" +
    String(
      distanciaCentro(),
      1
    ) +
    ",";

  json +=
    "\"efDir\":" +
    String(
      distanciaDir(),
      1
    ) +
    ",";

  json +=
    "\"efTras\":" +
    String(
      distanciaTras(),
      1
    ) +
    ",";


  json +=
    "\"memEsq\":" +
    String(
      memoriaPerigoAtiva(
        sensorEsq
      )
        ? "true"
        : "false"
    ) +
    ",";

  json +=
    "\"memCentro\":" +
    String(
      memoriaPerigoAtiva(
        sensorCentro
      )
        ? "true"
        : "false"
    ) +
    ",";

  json +=
    "\"memDir\":" +
    String(
      memoriaPerigoAtiva(
        sensorDir
      )
        ? "true"
        : "false"
    ) +
    ",";

  json +=
    "\"memTras\":" +
    String(
      memoriaPerigoAtiva(
        sensorTras
      )
        ? "true"
        : "false"
    ) +
    ",";


  json +=
    "\"vermelho\":" +
    String(
      limiarVermelho(),
      0
    ) +
    ",";


  json +=
    "\"base\":" +
    String(
      velocidadeAutoBase
    ) +
    ",";

  json +=
    "\"livre\":" +
    String(
      velocidadeAutoLivre
    ) +
    ",";

  json +=
    "\"dist\":" +
    String(
      distanciaDesvioCm,
      0
    ) +
    ",";

  json +=
    "\"tempoRe\":" +
    String(
      tempoReBaseMs
    ) +
    ",";

  json +=
    "\"tempoGiro\":" +
    String(
      tempoGiroBaseMs
    ) +
    ",";

  json +=
    "\"confMin\":" +
    String(
      confiancaMinima *
      100.0f,
      0
    ) +
    ",";

  json +=
    "\"manual\":" +
    String(
      velocidadeManual
    );


  json +=
    "}";


  server.send(
    200,
    "application/json",
    json
  );
}


// ============================================================
// ROTAS
// ============================================================

void configurarRotas() {
  server.on(
    "/",
    []() {
      server.send(
        200,
        "text/html",
        pagina()
      );
    }
  );


  server.on(
    "/auto",
    []() {
      if (
        server.hasArg(
          "on"
        )
      ) {
        if (
          server
            .arg("on")
            .toInt() ==
          1
        ) {
          ligarAutomatico();
        }

        else {
          desligarAutomatico(
            "DESLIGADO_PELO_USUARIO"
          );
        }
      }


      server.send(
        200,
        "text/plain",
        "OK"
      );
    }
  );


  server.on(
    "/emergencia",
    []() {
      desligarAutomatico(
        "PARADA_EMERGENCIA"
      );


      server.send(
        200,
        "text/plain",
        "PARADO"
      );
    }
  );


  server.on(
    "/config",
    []() {
      if (
        server.hasArg(
          "base"
        )
      ) {
        velocidadeAutoBase =
          constrain(
            server
              .arg("base")
              .toInt(),
            35,
            160
          );
      }


      if (
        server.hasArg(
          "livre"
        )
      ) {
        velocidadeAutoLivre =
          constrain(
            server
              .arg("livre")
              .toInt(),
            50,
            220
          );
      }


      if (
        server.hasArg(
          "dist"
        )
      ) {
        distanciaDesvioCm =
          constrain(
            server
              .arg("dist")
              .toFloat(),
            8.0f,
            40.0f
          );
      }


      if (
        server.hasArg(
          "re"
        )
      ) {
        tempoReBaseMs =
          constrain(
            server
              .arg("re")
              .toInt(),
            200,
            1200
          );
      }


      if (
        server.hasArg(
          "giro"
        )
      ) {
        tempoGiroBaseMs =
          constrain(
            server
              .arg("giro")
              .toInt(),
            200,
            1400
          );
      }


      if (
        server.hasArg(
          "conf"
        )
      ) {
        int valor =
          constrain(
            server
              .arg("conf")
              .toInt(),
            35,
            90
          );


        confiancaMinima =
          valor /
          100.0f;
      }


      if (
        server.hasArg(
          "manual"
        )
      ) {
        velocidadeManual =
          constrain(
            server
              .arg("manual")
              .toInt(),
            0,
            255
          );
      }


      server.send(
        200,
        "text/plain",
        "OK"
      );
    }
  );


  // ----------------------------------------------------------
  // MANUAL:
  // NENHUM sensor bloqueia movimento.
  // ----------------------------------------------------------

  server.on(
    "/manual",
    []() {
      modoAutomatico =
        false;

      estadoAuto =
        AUTO_DESLIGADO;

      tentativasEscape =
        0;

      motivoAuto =
        "MANUAL";


      String direcao =
        server.hasArg("dir")
          ? server.arg("dir")
          : "parar";


      if (
        direcao ==
        "frente"
      ) {
        motorFrente(
          velocidadeManual
        );
      }

      else if (
        direcao ==
        "re"
      ) {
        motorRe(
          velocidadeManual
        );
      }

      else if (
        direcao ==
        "esquerda"
      ) {
        motorEsquerda(
          velocidadeManual
        );
      }

      else if (
        direcao ==
        "direita"
      ) {
        motorDireita(
          velocidadeManual
        );
      }

      else {
        pararMotores();

        motivoAuto =
          "MANUAL_PARADO";
      }


      server.send(
        200,
        "text/plain",
        "OK"
      );
    }
  );


  server.on(
    "/status",
    enviarStatus
  );


  server.onNotFound(
    []() {
      server.sendHeader(
        "Location",
        "http://192.168.4.1",
        true
      );


      server.send(
        302,
        "text/plain",
        ""
      );
    }
  );
}


// ============================================================
// SETUP
// ============================================================

void setup() {
  Serial.begin(
    115200
  );

  delay(
    900
  );


  // Motores.
  pinMode(
    ENA,
    OUTPUT
  );

  pinMode(
    IN1,
    OUTPUT
  );

  pinMode(
    IN2,
    OUTPUT
  );

  pinMode(
    ENB,
    OUTPUT
  );

  pinMode(
    IN3,
    OUTPUT
  );

  pinMode(
    IN4,
    OUTPUT
  );


  pararMotores();


  // Sensores.
  pinMode(
    TRIG_ESQ,
    OUTPUT
  );

  pinMode(
    ECHO_ESQ,
    INPUT
  );

  pinMode(
    TRIG_CENTRO,
    OUTPUT
  );

  pinMode(
    ECHO_CENTRO,
    INPUT
  );

  pinMode(
    TRIG_DIR,
    OUTPUT
  );

  pinMode(
    ECHO_DIR,
    INPUT
  );

  pinMode(
    TRIG_TRAS,
    OUTPUT
  );

  pinMode(
    ECHO_TRAS,
    INPUT
  );


  digitalWrite(
    TRIG_ESQ,
    LOW
  );

  digitalWrite(
    TRIG_CENTRO,
    LOW
  );

  digitalWrite(
    TRIG_DIR,
    LOW
  );

  digitalWrite(
    TRIG_TRAS,
    LOW
  );


  pinMode(
    LDR_PIN,
    INPUT
  );


  // Wi-Fi.
  WiFi.mode(
    WIFI_AP
  );


  WiFi.softAPConfig(
    local_IP,
    gateway,
    subnet
  );


  bool wifiOK =
    WiFi.softAP(
      ssid,
      password
    );


  dnsServer.start(
    53,
    "*",
    local_IP
  );


  configurarRotas();


  server.begin();


  Serial.println();
  Serial.println(
    "========================================"
  );

  Serial.println(
    " CUBI-04 AUTONOMO V3"
  );

  Serial.println(
    "========================================"
  );


  Serial.println(
    wifiOK
      ? "[OK] WiFi"
      : "[ERRO] WiFi"
  );


  Serial.println(
    "[OK] Modelo V3 carregado"
  );


  Serial.println(
    "[OK] -- sem perigo = livre"
  );


  Serial.println(
    "[OK] -- apos vermelho = perigo memorizado"
  );


  Serial.println(
    "[OK] Manual sem bloqueio de sensores"
  );


  Serial.println(
    "[OK] Alerta sonoro no navegador"
  );


  Serial.println();
  Serial.println(
    "WiFi: tony"
  );

  Serial.println(
    "Senha: stark369"
  );

  Serial.println(
    "Web: http://192.168.4.1"
  );


  Serial.println();
  Serial.println(
    "Teste primeiro com rodas suspensas."
  );

  Serial.println(
    "========================================"
  );
}


// ============================================================
// LOOP
// ============================================================

void loop() {
  dnsServer
    .processNextRequest();


  server
    .handleClient();


  atualizarUltrassonicos();

  atualizarLDR();

  executarInferencia();


  // No manual NAO existe nenhuma rotina
  // automatica de bloqueio.
  if (
    modoAutomatico
  ) {
    executarAutomatico();
  }


  static unsigned long
    ultimoLog =
      0;


  if (
    millis() -
    ultimoLog >=
    900
  ) {
    ultimoLog =
      millis();


    Serial.println();
    Serial.println(
      "---------- V3 ----------"
    );


    Serial.print(
      "RAW ESQ: "
    );

    if (
      sensorEsq.bruto < 0
    ) {
      Serial.print(
        "--"
      );
    }

    else {
      Serial.print(
        sensorEsq.bruto,
        1
      );
    }


    Serial.print(
      " | CENTRO: "
    );

    if (
      sensorCentro.bruto < 0
    ) {
      Serial.print(
        "--"
      );
    }

    else {
      Serial.print(
        sensorCentro.bruto,
        1
      );
    }


    Serial.print(
      " | DIR: "
    );

    if (
      sensorDir.bruto < 0
    ) {
      Serial.print(
        "--"
      );
    }

    else {
      Serial.print(
        sensorDir.bruto,
        1
      );
    }


    Serial.print(
      " | TRAS: "
    );

    if (
      sensorTras.bruto < 0
    ) {
      Serial.println(
        "--"
      );
    }

    else {
      Serial.println(
        sensorTras.bruto,
        1
      );
    }


    Serial.print(
      "EFETIVAS: "
    );

    Serial.print(
      distanciaEsq(),
      1
    );

    Serial.print(
      " / "
    );

    Serial.print(
      distanciaCentro(),
      1
    );

    Serial.print(
      " / "
    );

    Serial.print(
      distanciaDir(),
      1
    );

    Serial.print(
      " / "
    );

    Serial.println(
      distanciaTras(),
      1
    );


    Serial.print(
      "ML: "
    );

    Serial.print(
      MODEL_ACTIONS[
        classeML
      ]
    );

    Serial.print(
      " | "
    );

    Serial.print(
      confiancaML *
      100.0f,
      1
    );

    Serial.println(
      "%"
    );


    Serial.print(
      "MODO: "
    );

    Serial.print(
      modoAutomatico
        ? "AUTO"
        : "MANUAL"
    );

    Serial.print(
      " | MOTOR: "
    );

    Serial.println(
      estadoMotor
    );


    Serial.print(
      "PLANO: "
    );

    Serial.print(
      motivoAuto
    );

    Serial.print(
      " | TENTATIVAS: "
    );

    Serial.println(
      tentativasEscape
    );


    Serial.println(
      "------------------------"
    );
  }
}
