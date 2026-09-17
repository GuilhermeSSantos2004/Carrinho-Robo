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

## Carrinho-robô finalizado

![Carrinho-robô CUBI-04 montado](docs/videos/Media.jpg)

**Protótipo físico montado**, com chassi e suportes impressos em 3D, quatro motores TT, rodas, HC-SR04, alimentação e integração eletrônica.

## 1. Objetivo e descrição

O CUBI-04 é um carrinho-robô desenvolvido para integrar projeto mecânico, fabricação digital, eletrônica, programação, sensores e comunicação sem fio. O chassi, a carenagem e os suportes foram desenvolvidos pela equipe para fabricação por impressão 3D. O controle é realizado pelo celular através de uma página web hospedada pelo próprio ESP32, sem necessidade de internet ou aplicativo externo.

A arquitetura atual utiliza quatro motores TT, ponte H L298N, ESP32, alimentação por baterias 18650, sensor ultrassônico HC-SR04 e LDR.

## 2. Principais funcionalidades

- quatro motores TT, dois por lado;
- movimentação para frente e para trás;
- curvas para esquerda e direita;
- parada;
- controle de velocidade por PWM;
- controle remoto por Wi-Fi;
- ESP32 cria sua própria rede local;
- interface web acessível pelo celular;
- HC-SR04 integrado como sensor de estacionamento de ré;
- bipes progressivos conforme o obstáculo se aproxima;
- bloqueio da ré a 5 cm ou menos;
- LDR no GPIO 34 para leitura de luminosidade;
- interface com modos AUTO, CLARO e ESCURO;
- chassi, carenagem e suportes próprios fabricados em impressão 3D.

## 3. Projeto mecânico e fabricação 3D

O projeto utiliza **chassi próprio modelado e impresso em 3D**. O desenvolvimento foi iterativo: medidas, encaixes, suportes, fixações, posição das rodas e carenagem foram alterados conforme os testes de montagem e fabricação.

### T01 — chassi/base final

![T01 chassi final no fatiador](docs/evidencias/cad_chassi_T01_final.jpg)

O T01 é a base estrutural atual. A versão final foi otimizada para aproximadamente **147 × 140 × 10 mm**, com pontos de fixação e área para os componentes.

### T02 — carenagem final

![T02 carenagem final no fatiador](docs/evidencias/cad_carenagem_T02_final.jpg)

A carenagem T02 foi reduzida e redesenhada para manter acesso aos componentes. As aberturas laterais são funcionais e precisam permanecer abertas para a passagem/encaixe dos suportes dos motores.

### Suportes dos quatro motores

![Suportes dos motores preparados para impressão](docs/evidencias/cad_suportes_motores_final.jpg)

Os quatro suportes foram preparados para fabricação por impressão 3D. O suporte passou por diversas versões até o conceito T03A/T03B v7.

### Peças da configuração atual

| Peça | Função / estado |
|---|---|
| T01 | base/chassi próprio otimizado |
| T02 | carenagem baixa com aberturas funcionais |
| T03A | suporte de motor v7 |
| T03B | presilha de retenção v7 |
| T04 | tampa de 2 mm, opcional no conceito atual |
| T05 | trava vertical suporte ↔ chassi |
| T06 | pino/parafuso impresso; dois por motor |

Os arquivos mecânicos, STL/STEP, fontes, versões e validações estão em [`cad/`](cad/). O pacote final verificado está documentado em [`cad/PACOTE_FINAL_VERIFICADO.md`](cad/PACOTE_FINAL_VERIFICADO.md).

## 4. Evolução do projeto mecânico

O repositório preserva as versões anteriores propositalmente para demonstrar o processo de desenvolvimento, e não apenas o resultado final.

### Proposta inicial

![Render do desenvolvimento mecânico](cad/versoes/v1/PREVIAS/CUBI_04_previa.png)

### Vista aberta da arquitetura inicial

![Vista aberta](cad/versoes/v1/PREVIAS/CUBI_04_aberto.png)

### Vista explodida

![Vista explodida](cad/versoes/v1/PREVIAS/CUBI_04_explodido.png)

### Evolução do suporte

![Evolução do suporte](cad/versoes/v3/preview_01_frontal.png)

Esses renders representam etapas do desenvolvimento e não devem ser confundidos com a geometria mecânica final mostrada nas imagens do T01, T02 e suportes acima.

## 5. Problemas e adaptações — 15/09/2026

Durante a montagem foi identificado que o posicionamento inicialmente previsto para os suportes não deixava o conjunto motor/roda corretamente alinhado com o chassi. Em algumas versões havia folga lateral e a roda não ocupava a posição desejada para girar livremente fora da estrutura.

As adaptações realizadas incluíram:

- reposicionamento dos suportes em relação às bordas do T01;
- centralização do eixo do motor;
- ajuste à largura real de aproximadamente 22 mm do motor TT;
- redução das folgas laterais;
- reforço das laterais e da região frontal do suporte;
- rodas mantidas para fora do perímetro útil do chassi;
- revisão das aberturas laterais do T02;
- preservação de acesso aos pontos de fixação;
- utilização das travas inferiores e parafusos/porcas no sistema de fixação.

Outros problemas encontrados incluíram cortes CAD fora do material real, corpos desconectados, porcas sem espaço suficiente, alterações no encaixe do HC-SR04 e aproximadamente 45 minutos adicionais de suporte detectados no fatiador. A carenagem também passou por otimização de volume de aproximadamente 8% em uma das etapas.

O histórico completo está em [`historico/HISTORICO_PROJETO_CUBI04.md`](historico/HISTORICO_PROJETO_CUBI04.md).

## 6. Hardware e eletrônica

### Componentes principais

- ESP32 DevKit V1 / NodeMCU-ESP32;
- ponte H L298N;
- 4 × motores TT amarelos 3–6 V, redução 48:1;
- 3 × células 18650 em série no suporte utilizado;
- HC-SR04;
- LDR;
- resistores para divisores;
- fios/conectores;
- rodas, fixadores e peças impressas em 3D.

Lista detalhada: [`hardware/componentes/README.md`](hardware/componentes/README.md).

### Diagrama das conexões

![Diagrama elétrico](hardware/arquitetura/diagrama-ligacoes.svg)

Documentação completa: [`hardware/arquitetura/README.md`](hardware/arquitetura/README.md).

### ESP32 ↔ L298N

| L298N | ESP32 | Função |
|---|---:|---|
| ENA | GPIO 25 | PWM canal A |
| IN1 | GPIO 26 | direção A |
| IN2 | GPIO 27 | direção A |
| ENB | GPIO 33 | PWM canal B |
| IN3 | GPIO 32 | direção B |
| IN4 | GPIO 23 | direção B |
| GND | GND | referência comum |

Os jumpers ENA/ENB são removidos para permitir o PWM.

## 7. Alimentação

O suporte utiliza três células 18650 em série: aproximadamente **11,1 V nominais e até 12,6 V carregadas**. O pack alimenta a entrada de potência da L298N. Durante os testes, o ESP32 pode ser alimentado separadamente por USB/power bank, mantendo GND comum.

**Limitação conhecida:** os motores TT utilizados são especificados para 3–6 V e o pack 3S possui tensão superior. O PWM baixo foi usado como limitação de teste, mas não substitui um regulador de tensão adequado. Essa limitação foi mantida documentada como parte das decisões do protótipo.

## 8. Controle remoto sem fio

O ESP32 cria sua própria rede Wi-Fi, sem necessidade de roteador ou internet.

| Item | Valor |
|---|---|
| SSID | `tony` |
| Senha | `stark369` |
| IP | `192.168.4.1` |
| Serial | `115200 baud` |

O painel web permite comandar frente, ré, esquerda, direita, parada e velocidade PWM.

## 9. Sensores integrados

### HC-SR04 — estacionamento de ré

| HC-SR04 | ESP32 |
|---|---:|
| VCC | 5 V |
| GND | GND comum |
| TRIG | GPIO 18 |
| ECHO | GPIO 19 através de divisor resistivo |

Na versão atual, o HC-SR04 funciona como sensor de estacionamento de ré. O celular produz bipes progressivamente mais rápidos conforme o obstáculo se aproxima e, a **5 cm ou menos**, o firmware bloqueia/interrompe a ré.

### LDR

O LDR está ligado ao **GPIO 34**. Sua leitura é apresentada no painel e, no modo AUTO, é utilizada para adaptar o tema da interface conforme a luminosidade. O firmware também exibe ADC bruto para calibração.

## 10. Software

Firmware principal: [`src/codigo.ino`](src/codigo.ino).  
Versão funcional preservada: [`src/v0.3/carrinho_robo_v0_3.ino`](src/v0.3/carrinho_robo_v0_3.ino).

O software integra Wi-Fi Access Point, servidor HTTP, DNS/captive portal, comandos direcionais, PWM, HC-SR04, bloqueio de ré, alertas sonoros no navegador, LDR, interface para celular e logs de diagnóstico.

Documentação: [`src/README.md`](src/README.md).

## 11. Requisitos, planejamento e evolução

Os materiais produzidos durante o desenvolvimento estão em [`organizacao/`](organizacao/):

- [`BACKLOG.md`](organizacao/BACKLOG.md);
- [`MVP.md`](organizacao/MVP.md);
- [`MOSCOW.md`](organizacao/MOSCOW.md);
- [`KANBAN.md`](organizacao/KANBAN.md);
- [`DEPENDENCIAS.md`](organizacao/DEPENDENCIAS.md);
- planilha de custos.

As decisões, dificuldades e mudanças estão consolidadas no histórico do projeto.

## 12. Testes e resultados

Foram registrados testes de reconhecimento do ESP32/CP2102, upload de firmware, Wi-Fi local, controle web, PWM, sensores, encaixe mecânico e fabricação. Entre os problemas encontrados estão folgas nos suportes, fragilidade em versões anteriores, desalinhamento das rodas, colisões geométricas, suporte excessivo no fatiador e alterações no encaixe do ultrassônico.

As correções foram preservadas no GitHub para demonstrar a evolução real do projeto.

## 13. Evidências finais

### Foto do carrinho montado

![Carrinho final](docs/videos/Media.jpg)

### Fabricação/modelagem final

![T01 final](docs/evidencias/cad_chassi_T01_final.jpg)

![T02 final](docs/evidencias/cad_carenagem_T02_final.jpg)

![Suportes finais](docs/evidencias/cad_suportes_motores_final.jpg)

### Vídeos

- [Vídeo de evidência em `docs/videos`](docs/videos/MicrosoftTeams-video.mp4)
- [Vídeo final adicionado ao repositório](WhatsApp%20Video%202026-09-17%20at%207.53.06%20PM.mp4)

A página [`docs/EVIDENCIAS_CHECKPOINT.md`](docs/EVIDENCIAS_CHECKPOINT.md) centraliza as evidências da entrega.

## 14. Como utilizar

1. conferir a fiação com o circuito desligado;
2. alimentar o ESP32 por USB/power bank;
3. abrir `src/codigo.ino` na Arduino IDE;
4. selecionar `ESP32 Dev Module` ou `DOIT ESP32 DEVKIT V1`;
5. realizar o upload;
6. abrir o Serial Monitor em 115200 baud;
7. conectar o celular à rede `tony`, senha `stark369`;
8. acessar `192.168.4.1`;
9. ativar o áudio do navegador;
10. iniciar com PWM baixo;
11. testar parada, frente, ré e curvas;
12. durante a ré, aproximar um obstáculo do HC-SR04 e observar o alerta/bloqueio próximo de 5 cm;
13. variar a iluminação do LDR e observar a interface.

## 15. Organização do repositório

```text
Carrinho-Robo/
├── README.md
├── CHECKPOINT_01_VALIDACAO.md
├── cad/
│   ├── STL/
│   ├── fonte-modelo/
│   └── versoes/
├── docs/
│   ├── evidencias/
│   └── videos/
├── hardware/
│   ├── arquitetura/
│   └── componentes/
├── historico/
├── organizacao/
└── src/
```

## 16. Relação com os critérios do Check Point 01

| Critério | Evidência |
|---|---|
| Chassi e projeto mecânico | chassi próprio, CAD, STL/STEP, versões, renders, fabricação e histórico |
| Movimentação e sistema elétrico | quatro motores, L298N, PWM, alimentação e comandos direcionais |
| Controle remoto sem fio | Wi-Fi próprio do ESP32 + interface web |
| Sensor integrado | HC-SR04 de ré + LDR integrados ao firmware |
| Carenagem e acabamento | T02 final e evolução documentada |
| README e apresentação | equipe/RMs, objetivo, funções, foto final e organização |
| Requisitos, planejamento e evolução | `organizacao/` + `historico/` |
| Projeto mecânico e fabricação | `cad/`, STL/STEP, fontes, renders e imagens do fatiador |
| Hardware e eletrônica | componentes, pinagem e diagrama em `hardware/` |
| Software | firmware completo e versão v0.3 em `src/` |
| Testes e resultados | histórico de problemas, testes e correções |
| Evidências finais | foto real, imagens de fabricação, vídeos e instruções |

Consulte também [`CHECKPOINT_01_VALIDACAO.md`](CHECKPOINT_01_VALIDACAO.md).