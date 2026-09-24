#pragma once
#include <stdint.h>
#include <math.h>

// Logica independente do hardware, utilizada pelo sketch e pelos testes C++.
class ProcessadorDistancia {
 public:
  static constexpr uint8_t JANELA = 5;

  // Exige cinco leituras validas consecutivas; falhas descartam a janela inteira.
  bool adicionar(float cm, float &feature, uint8_t &label) {
    if (!isfinite(cm) || cm < 2.0f || cm > 400.0f) {
      reiniciar();
      return false;
    }
    valores[indice] = cm;
    indice = (indice + 1) % JANELA;
    if (quantidade < JANELA) ++quantidade;
    if (quantidade < JANELA) return false;
    float soma = 0.0f;
    for (uint8_t i = 0; i < JANELA; ++i) soma += valores[i];
    float media = soma / JANELA;
    // Faixa de trabalho didatica 0..100 cm, distinta da faixa fisica 2..400 cm.
    if (media > 100.0f) media = 100.0f;
    // Quantizar antes de rotular evita divergencia no limiar do CSV (6 casas).
    const uint32_t normalizada = static_cast<uint32_t>(lroundf(media * 10000.0f));
    feature = normalizada / 1000000.0f;
    label = normalizada < 200000UL ? 0 : 1;
    return true;
  }

  void reiniciar() { quantidade = 0; indice = 0; }

 private:
  float valores[JANELA] = {};
  uint8_t quantidade = 0;
  uint8_t indice = 0;
};
