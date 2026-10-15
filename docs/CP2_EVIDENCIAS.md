# CP2 — Evidências visuais e vídeo

## Mídias já existentes

| Arquivo | O que comprova | O que não comprova |
|---|---|---|
| [`2026/2026-atual.mp4`](../2026/2026-atual.mp4) | carrinho montado, movimento e controle pelo celular | não mostra dataset, Edge Impulse, classe/probabilidade no ESP32 nem ação comprovadamente causada pelo modelo |
| [`2026/fotos/image.png`](../2026/fotos/image.png) | proposta CAD com sensores e cones de leitura | não comprova montagem física nem calibração |
| [`2026/fotos/image_1.png`](../2026/fotos/image_1.png) | bancada com ESP32, L298N, motores e sensor | não comprova coleta rotulada nem treinamento |
| [`docs/videos/MicrosoftTeams-video.mp4`](videos/MicrosoftTeams-video.mp4) | registro anterior de demonstração | não é evidência específica do CP2 |

O vídeo atual é uma boa evidência do CP1/evolução mecânica e do controle Wi-Fi. Para
o CP2, gravar uma nova versão curta com o Serial Monitor visível ou com uma tela
equivalente que mostre as leituras, a classe e a confiança enquanto o carrinho age.

## Roteiro mínimo do vídeo CP2

1. mostrar o carrinho e os HC-SR04 instalados;
2. mostrar rapidamente a montagem do divisor de tensão do ECHO;
3. mostrar o ESP32 ligado e o Serial Monitor em 115200;
4. deixar a frente livre e registrar `LIVRE` + avanço;
5. aproximar obstáculo à esquerda e registrar classe + giro para a direita;
6. repetir no centro e na direita;
7. manter a câmera enquadrando o carrinho durante a ação;
8. mencionar no início o nome do commit/firmware usado.

## Auditoria dos arquivos adicionados em 14/10/2026

As imagens recentes registradas no repositório mostram o carrinho físico e peças
CAD, mas não são prints do Edge Impulse. O pacote V3 extraído em
[`../data/CUBI04_Autonomo_V3/`](../data/CUBI04_Autonomo_V3/) preserva firmware e
um modelo local, porém seu próprio documento informa que as 6.000 amostras são
sintéticas. Ele não substitui o vídeo CP2, os prints do Edge Impulse, o dataset
real ou a biblioteca exportada.

## Convenção de nomes para novas evidências

```text
experimentos/cp2-tinyml/evidencias/
├── 01-dataset-importado.png
├── 02-divisao-dataset.png
├── 03-configuracao-impulse.png
├── 04-treinamento.png
├── 05-matriz-confusao.png
├── 06-inferencia-amostras.png
├── 07-export-arduino.png
├── 08-montagem-cp2.jpg
├── 09-serial-inferencia.jpg
└── video-cp2-tinyml.mp4
```

Não substituir as mídias antigas. A evolução CP1 → CP2 fica mais clara quando o
vídeo anterior e o vídeo TinyML têm nomes e funções distintas.
