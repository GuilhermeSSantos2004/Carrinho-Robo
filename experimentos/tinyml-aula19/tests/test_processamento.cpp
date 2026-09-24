// Dados sintéticos exclusivos para testar o algoritmo; não são evidência física.
#include "../firmware/leitura_processada/processamento.h"
#include <initializer_list>
#include <cassert>
#include <cmath>

int main() {
  ProcessadorDistancia p;
  float f = -1;
  uint8_t label = 9;
  for (int i = 0; i < 4; ++i) assert(!p.adicionar(10, f, label));
  assert(p.adicionar(10, f, label) && fabs(f - 0.1f) < 1e-6 && label == 0);
  // Média de [60,10,10,10,10] = 20: fronteira pertence à classe longe.
  assert(p.adicionar(60, f, label) && fabs(f - 0.2f) < 1e-6 && label == 1);
  for (float invalido : {-1.0f, 1.0f, 401.0f, INFINITY, NAN}) {
    assert(!p.adicionar(invalido, f, label));
    for (int i = 0; i < 4; ++i) assert(!p.adicionar(400, f, label));
    assert(p.adicionar(400, f, label) && f == 1.0f && label == 1);
  }
  p.reiniciar();
  for (int i = 0; i < 4; ++i) assert(!p.adicionar(2, f, label));
  assert(p.adicionar(2, f, label) && fabs(f - 0.02f) < 1e-6 && label == 0);
}
