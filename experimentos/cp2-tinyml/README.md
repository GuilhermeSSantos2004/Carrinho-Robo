# Experimento CP2 — coleta real e integração TinyML

Esta pasta contém os materiais específicos do Check Point 02. O firmware de coleta
gera linhas com três distâncias e um label definido pelo cenário físico. O firmware
final de referência está em [`../../src/tinyml_carrinho_cp2.ino`](../../src/tinyml_carrinho_cp2.ino).

## Ordem de uso

1. Ajustar `CLASSE_COLETA` em `firmware/coleta_dataset_3sensores.ino`.
2. Gravar o sketch no ESP32 sem a bateria dos motores conectada.
3. Salvar uma sessão bruta por classe em `dados/`.
4. Preencher [`dados/REGISTRO_COLETA.md`](dados/REGISTRO_COLETA.md).
5. Consolidar o CSV real conforme [`../../docs/CP2_COLETA_DATASET.md`](../../docs/CP2_COLETA_DATASET.md).
6. Importar e treinar no Edge Impulse.
7. Guardar prints em `evidencias/` e a biblioteca exportada em `../../model/edge-impulse/`.

Não colocar dados sintéticos nesta pasta com nome de coleta real. Se um teste de
software for necessário, manter o arquivo explicitamente marcado como `sintetico`.
