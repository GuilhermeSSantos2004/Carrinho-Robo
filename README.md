# Carrinho-robô com ESP32 — CUBI-04

**Project-based Maker Lab — FIAP**  
**Grupo: Start-up One**

## Integrantes

| Nome | RM |
|---|---|
| Enricco Rossi de Souza Carvalho Miranda | RM551717 |
| Gabriel Marquez Trevisan | RM99227 |
| Guilherme Silva dos Santos | RM551168 |
| Danilo Urze Aldred | RM99465 |
| Laura Claro Mathias | RM98747 |

## 1. Objetivo e descrição

O CUBI-04 é um carrinho-robô desenvolvido para integrar projeto mecânico, fabricação digital, eletrônica, programação, sensores e comunicação sem fio. O chassi e a carenagem foram desenvolvidos pela equipe para fabricação por impressão 3D. O controle é realizado pelo celular através de uma página web hospedada pelo próprio ESP32, sem necessidade de internet ou aplicativo externo.

O conceito inicial passou por diversas revisões. A arquitetura atual utiliza quatro motores TT, ponte H L298N, ESP32, alimentação por baterias 18650, sensor ultrassônico HC-SR04 e LDR.

## 2. Visão do projeto e evolução visual

### Proposta mecânica / render

![Render do desenvolvimento mecânico](cad/versoes/v1/PREVIAS/CUBI_04_previa.png)

### Vista aberta

![Vista aberta do projeto](cad/versoes/v1/PREVIAS/CUBI_04_aberto.png)

### Vista explodida

![Vista explodida](cad/versoes/v1/PREVIAS/CUBI_04_explodido.png)

### Evolução do suporte do motor

![Evolução do suporte do motor](cad/versoes/v3/preview_01_frontal.png)

### Registro físico disponível no repositório

![Registro físico do projeto](docs/videos/Media.jpg)

> O repositório preserva renders e registros de diferentes etapas para demonstrar o processo de desenvolvimento. As versões anteriores não devem ser confundidas com a geometria mecânica final.

## 3. Principais funcionalidades

- quatro motores TT, dois por lado;
- movimentação para frente e para trás;
- curvas para esquerda e direita;
- parada;
- controle de velocidade por PWM;
- ponte H L298N;
- controle remoto por Wi-Fi;
- ESP32 cria sua própria rede local;
- interface web acessível pelo celular;
- HC-SR04 integrado como sensor de estacionamento de ré;
- bipes progressivos conforme o obstáculo se aproxima;
- bloqueio da ré a 5 cm ou menos;
- LDR no GPIO 34 para leitura de luminosidade;
- interface com modos AUTO, CLARO e ESCURO;
- chassi, carenagem e suportes desenvolvidos para fabricação 3D.

## 4. Chassi, projeto mecânico e fabricação 3D

O projeto utiliza chassi próprio. O processo não foi feito em uma única versão: medidas, encaixes, suportes e carenagem foram alterados conforme os testes de montagem e impressão.

### Peças da configuração atual

| Peça | Função / estado |
|---|---|
| T01 — base/chassi | base própria otimizada, aproximadamente 147 × 140 × 10 mm |
| T02 — carenagem | carenagem baixa, aproximadamente 140 × 133 × 24 mm, com aberturas funcionais |
| T03A — suporte de motor | suporte v7 |
| T03B — presilha | retenção do motor v7 |
| T04 — tampa | 2 mm; opcional no conceito atual |
| T05 — trava | trava vertical suporte ↔ chassi |
| T06 — pino/parafuso impresso | dois por motor, oito previstos |

Os arquivos e versões mecânicas estão em [`cad/`](cad/). O repositório contém versões anteriores, arquivos STL, arquivos-fonte/STEP, scripts de geração e validação e renders. A evolução existente em `cad/versoes/` é mantida propositalmente como evidência do processo, em vez de mostrar somente o resultado final.

## 5. Problemas mecânicos e adaptações

### 15/09/2026 — suporte, chassi e alinhamento das rodas

Durante a montagem foi identificado que o posicionamento inicialmente previsto não deixava o conjunto motor/roda corretamente alinhado com o chassi. Em determinadas versões, havia folga lateral no suporte e a posição do motor deixava a roda distante ou inadequadamente posicionada em relação à borda.

As adaptações realizadas incluíram reposicionamento dos suportes, centralização do eixo, ajuste à largura real de aproximadamente 22 mm do motor TT, reforço das laterais e região frontal e manutenção das rodas para fora do perímetro útil do chassi. As aberturas laterais da carenagem também precisaram permanecer abertas, pois os suportes dos motores atravessam/ocupam essas regiões durante a montagem.

A versão do suporte evoluiu até o conceito T03A/T03B v7, separando suporte e presilha e permitindo melhor montagem e manutenção. As travas inferiores e os pontos de fixação também passaram a fazer parte do sistema mecânico.

### Outras correções realizadas

- folga excessiva nas primeiras versões do suporte;
- região frontal do suporte frágil durante o aperto;
- cortes CAD realizados fora do material real;
- peças aparentemente válidas contendo corpos desconectados;
- pedestais e apoios ultrapassando a área real da base;
- necessidade de porcas cativas para a fixação;
- revisão do encaixe do HC-SR04;
- redução da tampa de 3 mm para 2 mm;
- remoção de pedestais para reduzir material e tempo;
- redesign da carenagem para não envolver completamente os suportes;
- aproximadamente 45 minutos extras de suporte detectados no fatiador;
- criação/reposicionamento de apoios para melhorar a impressão;
- redução aproximada de 8% no volume da carenagem em uma das etapas de otimização.

O relato detalhado está em [`historico/HISTORICO_PROJETO_CUBI04.md`](historico/HISTORICO_PROJETO_CUBI04.md).

## 6. Hardware e eletrônica

### Componentes principais

- ESP32 DevKit V1 / NodeMCU-ESP32;
- L298N — ponte H dupla;
- 4 × motores TT amarelos 3–6 V, redução 48:1;
- suporte para 3 × células 18650 em série;
- HC-SR04;
- LDR;
- resistores para divisor do ECHO e divisor do LDR;
- fios/conectores;
- rodas, fixadores e peças impressas em 3D.

A lista detalhada está em [`hardware/componentes/README.md`](hardware/componentes/README.md).

### Diagrama de conexões

![Diagrama elétrico](hardware/arquitetura/diagrama-ligacoes.svg)

Documentação elétrica completa: [`hardware/arquitetura/README.md`](hardware/arquitetura/README.md).

### L298N ↔ ESP32

| L298N | ESP32 | Função |
|---|---:|---|
| ENA | GPIO 25 | PWM canal A |
| IN1 | GPIO 26 | direção A |
| IN2 | GPIO 27 | direção A |
| ENB | GPIO 33 | PWM canal B |
| IN3 | GPIO 32 | direção B |
| IN4 | GPIO 23 | direção B |
| GND | GND | referência comum |

Os jumpers ENA e ENB são removidos para permitir o controle por PWM.

## 7. Alimentação

O protótipo utiliza suporte com três células 18650 em série, resultando em aproximadamente 11,1 V nominais e até 12,6 V quando totalmente carregadas. O pack alimenta a entrada de potência da L298N. Durante os testes, o ESP32 é alimentado separadamente por USB/power bank, mantendo GND comum entre os circuitos.

**Limitação conhecida:** os motores TT utilizados são especificados para 3–6 V. A tensão do pack 3S é superior a essa faixa. O PWM baixo foi utilizado como limitação durante testes, mas isso não equivale a um regulador de tensão. A limitação permanece documentada como parte das decisões e riscos do protótipo.

## 8. Controle remoto sem fio

O ESP32 cria uma rede Wi-Fi própria. Portanto, o carrinho não depende de roteador ou acesso à internet.

| Item | Valor |
|---|---|
| SSID | `tony` |
| Senha | `stark369` |
| IP do controle | `192.168.4.1` |
| Serial | `115200 baud` |

A página web permite comandar frente, ré, esquerda, direita e parada, além de alterar o PWM.

## 9. Sensores integrados

### HC-SR04 — sensor de estacionamento de ré

| HC-SR04 | ESP32 |
|---|---:|
| VCC | 5 V |
| GND | GND comum |
| TRIG | GPIO 18 |
| ECHO | GPIO 19 através de divisor resistivo |

O sensor teve sua função alterada durante o desenvolvimento. Inicialmente foi experimentado como bloqueio frontal em aproximadamente 20 cm. Na versão atual ele auxilia a ré: os bipes do celular ficam progressivamente mais rápidos com a aproximação e a ré é interrompida a 5 cm ou menos.

Divisor do ECHO utilizado na documentação atual:

```text
ECHO ── 1 kΩ ──┬── GPIO 19
                │
               1 kΩ
                │
               1 kΩ
                │
               GND
```

### LDR

O LDR está ligado ao GPIO 34 e fornece uma leitura de luminosidade para a interface. No modo AUTO, essa leitura altera o tema do painel. O firmware também apresenta ADC bruto e percentual para facilitar calibração e diagnóstico.

```text
3V3 ── LDR ──┬── GPIO 34
              │
           resistor
              │
             GND
```

## 10. Software

O firmware principal está em [`src/codigo.ino`](src/codigo.ino). A versão funcional congelada está em [`src/v0.3/carrinho_robo_v0_3.ino`](src/v0.3/carrinho_robo_v0_3.ino).

O software integra:

- geração do Access Point Wi-Fi;
- servidor HTTP local;
- DNS/captive portal;
- comandos de movimento;
- controle PWM dos dois lados do carrinho;
- leitura do HC-SR04;
- proteção/bloqueio da ré na faixa crítica;
- alerta sonoro no navegador;
- leitura do LDR;
- interface responsiva para celular;
- logs de diagnóstico no Serial Monitor.

Mais detalhes em [`src/README.md`](src/README.md).

## 11. Requisitos, planejamento e evolução

Os materiais de planejamento foram preservados em [`organizacao/`](organizacao/), incluindo:

- [`BACKLOG.md`](organizacao/BACKLOG.md);
- [`MVP.md`](organizacao/MVP.md);
- [`MOSCOW.md`](organizacao/MOSCOW.md);
- [`KANBAN.md`](organizacao/KANBAN.md);
- [`DEPENDENCIAS.md`](organizacao/DEPENDENCIAS.md);
- planilha de custos.

Além disso, [`historico/HISTORICO_PROJETO_CUBI04.md`](historico/HISTORICO_PROJETO_CUBI04.md) consolida decisões, erros, correções e mudanças mecânicas/eletrônicas/software.

## 12. Testes, problemas e resultados

O projeto foi desenvolvido de forma iterativa. Foram realizados testes de reconhecimento do ESP32, upload de firmware, rede Wi-Fi local, controle web, PWM e leituras dos sensores, além de verificações mecânicas de encaixe e fabricação.

Problemas registrados durante o desenvolvimento incluem driver CP2102 ausente no Windows, folgas no suporte, quebra/fragilidade de regiões do suporte, desalinhamento da roda, colisões geométricas, suporte excessivo no fatiador, alterações no encaixe do ultrassônico e necessidade de tornar a leitura do LDR observável para calibração.

Esses problemas não foram removidos do histórico: foram mantidos como evidência da evolução do projeto.

## 13. Evidências

### Imagens/renders de desenvolvimento

| Evidência | Arquivo |
|---|---|
| render geral da primeira arquitetura | [`CUBI_04_previa.png`](cad/versoes/v1/PREVIAS/CUBI_04_previa.png) |
| montagem aberta | [`CUBI_04_aberto.png`](cad/versoes/v1/PREVIAS/CUBI_04_aberto.png) |
| vista explodida | [`CUBI_04_explodido.png`](cad/versoes/v1/PREVIAS/CUBI_04_explodido.png) |
| vista traseira | [`CUBI_04_traseira.png`](cad/versoes/v1/PREVIAS/CUBI_04_traseira.png) |
| suporte v2 | [`T03_motor_v2_previa.png`](cad/versoes/v2/PREVIA/T03_motor_v2_previa.png) |
| suporte v3 | [`preview_01_frontal.png`](cad/versoes/v3/preview_01_frontal.png) |
| registro físico disponível | [`Media.jpg`](docs/videos/Media.jpg) |

### Vídeo disponível

[`MicrosoftTeams-video.mp4`](docs/videos/MicrosoftTeams-video.mp4)

O vídeo acima é mantido como mídia de evidência existente no repositório. Para a avaliação presencial/final, a demonstração deve permitir ao avaliador observar claramente as funções que forem apresentadas no protótipo real.

## 14. Como utilizar

1. conferir toda a fiação com a alimentação desligada;
2. deixar as rodas suspensas no primeiro teste após qualquer alteração elétrica;
3. alimentar o ESP32 por USB/power bank;
4. abrir `src/codigo.ino` na Arduino IDE;
5. selecionar `ESP32 Dev Module` ou `DOIT ESP32 DEVKIT V1` e a porta CP210x;
6. realizar o upload;
7. abrir o Serial Monitor em 115200 baud;
8. conectar o celular à rede Wi-Fi `tony`, senha `stark369`;
9. acessar `192.168.4.1`;
10. ativar o áudio no painel para permitir os alertas do sensor de ré;
11. iniciar com PWM baixo;
12. testar parada, frente, ré e curvas;
13. durante a ré, aproximar um obstáculo do HC-SR04 e observar o alerta progressivo e o bloqueio próximo de 5 cm;
14. alterar a iluminação sobre o LDR e observar a leitura/tema da interface.

## 15. Organização do repositório

```text
Carrinho-Robo/
├── README.md                         # apresentação principal
├── CHECKPOINT_01_VALIDACAO.md        # checklist contra a rubrica
├── cad/                              # projeto mecânico
│   ├── STL/                          # documentação dos STLs atuais
│   ├── fonte-modelo/                 # fontes de modelagem
│   └── versoes/                      # histórico mecânico e arquivos de versões
├── docs/                             # croqui e mídias
│   └── videos/                       # foto e vídeo existentes
├── hardware/
│   ├── arquitetura/                  # esquema e pinagem
│   └── componentes/                  # lista de componentes
├── historico/                        # problemas, decisões e correções
├── organizacao/                      # backlog, MVP, planejamento e custos
└── src/                              # firmware e versões
```

## 16. Relação direta com os critérios do Check Point 01

| Critério | Evidência no projeto |
|---|---|
| Chassi e projeto mecânico | CAD próprio, versões, fontes, STL, renders e histórico em `cad/` |
| Movimentação e sistema elétrico | quatro motores, L298N, PWM, comandos direcionais e alimentação documentados |
| Controle remoto sem fio | Wi-Fi próprio do ESP32 + interface web |
| Sensor integrado | HC-SR04 de ré e LDR integrados ao firmware |
| Carenagem e acabamento | T02 e evolução da carenagem documentados no CAD/histórico |
| README e apresentação | este documento contém equipe, objetivo, funções, imagens e organização |
| Requisitos, planejamento e evolução | `organizacao/` + `historico/` |
| Projeto mecânico e fabricação | `cad/`, arquivos-fonte, STEP/STL, renders e versões |
| Hardware e eletrônica | `hardware/`, componentes e diagrama de conexões |
| Software | `src/`, firmware atual e versões |
| Testes e resultados | histórico de testes, dificuldades e correções |
| Evidências | imagens/renders, `Media.jpg`, vídeo e instruções de utilização |

Consulte também [`CHECKPOINT_01_VALIDACAO.md`](CHECKPOINT_01_VALIDACAO.md) para a conferência específica da rubrica.
