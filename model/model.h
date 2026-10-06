#ifndef CUBI04_TINYML_MODEL_H
#define CUBI04_TINYML_MODEL_H

// =====================================================
// CUBI-04 - Tarefa 21 TinyML
// Modelo: regressao logistica multinomial (softmax)
// Entradas: distancia esquerda, centro e direita
// Normalizacao: distancia_cm / 150.0
//
// IMPORTANTE:
// Este modelo foi treinado com dataset SINTETICO REALISTA,
// criado para representar leituras do HC-SR04 com ruido.
// Nao deve ser apresentado como dataset fisicamente medido.
// =====================================================

#define MODEL_NUM_FEATURES 3
#define MODEL_NUM_CLASSES 4
#define MODEL_NORMALIZATION_CM 150.0f

// Mapeamento das classes:
// 0 = LIVRE
// 1 = OBSTACULO_ESQUERDA
// 2 = OBSTACULO_CENTRO
// 3 = OBSTACULO_DIREITA

static const char* MODEL_LABELS[MODEL_NUM_CLASSES] = {
  "LIVRE",
  "OBSTACULO_ESQUERDA",
  "OBSTACULO_CENTRO",
  "OBSTACULO_DIREITA"
};

// Pesos treinados offline a partir de docs/dataset.csv.
// Ordem das features:
// [esquerda_cm/150, centro_cm/150, direita_cm/150]

static const float MODEL_WEIGHTS[MODEL_NUM_CLASSES][MODEL_NUM_FEATURES] = {
  { 16.30608587f,  16.00131349f,  19.94098471f },
  {-49.17189272f,  14.96672547f,  18.27607089f },
  { 19.35833406f, -47.74578342f,  17.55409859f },
  { 13.50747279f,  16.77774446f, -55.77115419f }
};

static const float MODEL_BIAS[MODEL_NUM_CLASSES] = {
  -11.18865040f,
    3.51562765f,
    2.38724123f,
    5.28578152f
};

#endif
