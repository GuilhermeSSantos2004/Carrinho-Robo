# Dados e firmware V3

Esta pasta contém o pacote `CUBI04_Autonomo_V3` extraído do ZIP recebido em
14/10/2026.

## Conteúdo

| Arquivo | Descrição | Situação para o CP2 |
|---|---|---|
| `dataset_autonomo_real.csv` | 6.000 amostras coletadas pelo grupo com o carrinho e o ESP32 | **Dataset real da coleta V3** |
| `model_auto_v3.h` | Modelo MLP local, com 4 entradas e 5 saídas | Baseline local; não é exportação Edge Impulse |
| `CUBI04_Autonomo_V3.ino` | Firmware autônomo com inferência local e controle dos motores | Código de referência para integração |
| `LEIA_PRIMEIRO_V3.txt` | Instruções e resultados declarados do pacote | Documento de origem |

O arquivo `LEIA_PRIMEIRO_V3.txt` registra a coleta pelo grupo com sensores HC-SR04
e ESP32, além da acurácia de 95,92% no teste. O nome anterior do arquivo foi
corrigido para `dataset_autonomo_real.csv`.

## Diferença em relação ao protocolo CP2

O protocolo documentado em [`../docs/CP2_COLETA_DATASET.md`](../docs/CP2_COLETA_DATASET.md)
usa três sensores frontais e quatro classes. Este pacote V3 usa quatro entradas
(`esquerda_cm`, `centro_cm`, `direita_cm`, `tras_cm`) e cinco classes
(`FRENTE`, `VIRAR_ESQUERDA`, `VIRAR_DIREITA`, `RE`, `PARAR`). Portanto, ele deve
ser mantido como uma versão/autônoma de referência até que a equipe registre
qual configuração foi realmente treinada no Edge Impulse.

Para comprovar o CP2 ainda são necessários, nesta pasta ou em seus diretórios
documentados: CSV/TXT bruto medido pelo HC-SR04, prints do projeto Edge Impulse,
biblioteca exportada pelo Edge Impulse e evidência do upload/teste no ESP32.
