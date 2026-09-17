# Histórico do projeto CUBI-04 — modificações, erros e correções

Este documento consolida o processo real de desenvolvimento mecânico, eletrônico e de firmware do carrinho-robô. O objetivo é registrar não só o resultado final, mas também as dificuldades, hipóteses erradas, correções e decisões que levaram ao estado atual.

> Os horários exatos das conversas não foram preservados. As sessões abaixo estão organizadas pelas datas e pela ordem real das alterações conhecidas.

---

## Sessão 1 — 15/09/2026

Foco: suporte do motor (T03A) e presilha (T03B).

- **v3**: suporte com largura interna 22,4 mm, reforços estruturais e furos de 3,6 mm.
- **v4**: reforços removidos após teste/avaliação; janela frontal e rasgo de eixo ampliado.
- **v5**: tampa frontal passou para encaixe tipo rabo-de-andorinha e o rasgo da base foi estendido.
- **v6**: tampa frontal eliminada; frente aberta e presilha superior adotada como retenção do motor; furos ajustados para 3,46 mm.
- **v7 — versão adotada**: suporte T03A separado da presilha T03B; dois furos de 4,5 mm para fixação no chassi e buchas com rebaixo para porca.

### Dificuldades dessa etapa

1. Folga excessiva em versões iniciais.
2. Região frontal frágil durante o aperto.
3. Necessidade de permitir montagem/desmontagem rápida do motor.
4. Ajuste dos furos a partir dos parafusos realmente disponíveis.

---

## Sessão 2 — 17/09/2026 — mecânica

### Criação do T01, T02 e T04

A primeira arquitetura usava:

- base de 163 × 156 mm;
- piso de 4 mm;
- quatro pedestais para suportes;
- guia de bateria;
- apoio para ESP32;
- carcaça envolvendo os motores;
- tampa superior parafusada.

### Reposicionamento dos motores

A geometria foi alterada para deixar as rodas realmente para fora do chassi e alinhar a face do motor com a borda útil da estrutura.

### Porcas cativas

**Problema:** os furos de fixação chegavam ao chassi sem espaço físico suficiente para alojar a porca.

**Correção:** criação de rebaixo hexagonal de aproximadamente 6 mm entre-chaves e 2,6 mm de profundidade.

### Overhang e peças saindo da base

**Problema:** pedestal e um dos apoios do ESP32 ultrapassavam o retângulo real da base.

**Correção:** suportes foram recuados para dentro da área útil e os apoios internos foram reposicionados.

### Trava suporte ↔ chassi

Foram testadas abordagens diferentes:

1. furo/pino lateral — descartado porque o corte estava fora do material real;
2. trava vertical de cima para baixo — adotada;
3. ajuste fino das nervuras para garantir contato real com material sólido.

Esse processo mostrou que uma operação booleana aparentemente válida não garante que o corte tenha atingido a peça.

### Carcaça mais baixa

A carcaça foi reduzida para ficar poucos milímetros acima dos suportes. A trava passou a se relacionar diretamente com a carcaça.

Durante essa etapa ocorreram três erros importantes:

- pino projetado em região onde não existia parede real;
- conta de altura sem considerar corretamente a espessura do piso;
- pequena colisão residual por margem geométrica incorreta.

Todos foram corrigidos após conferência da geometria real.

### Ultrassônico na carcaça

O encaixe do HC-SR04 passou por várias revisões:

- tentativa inicial com dois furos assumidos;
- tentativa com rasgo único;
- retorno para dois furos;
- revisão do espaçamento real;
- tentativa de tubos salientes ao redor dos sensores;
- remoção dos tubos a pedido, mantendo acabamento mais raso;
- rebaixos internos para esconder cabeças de fixação.

A principal lição foi não congelar dimensões assumidas antes de validar a peça física.

### Tampa

A tampa foi reduzida de 3 mm para 2 mm para economizar material e tempo de impressão. No conceito mecânico atual ela não é peça obrigatória.

### T06 — pino/parafuso impresso

Foi criado um pino com cabeça e nervuras de atrito para substituir parte da ferragem metálica. O projeto usa dois pontos de fixação por motor, oito no total.

### Otimização do chassi

Para reduzir tempo e material:

- pedestais foram removidos;
- os furos passaram diretamente para o piso;
- dimensões externas foram reduzidas;
- borda/guia periférica foi removida;
- a carcaça precisou receber novas regiões de ligação para continuar sendo uma peça única.

### Redesign da carcaça

A carcaça deixou de envolver completamente os suportes dos motores. O conceito atual é uma carcaça baixa, com os suportes atravessando/ficando visíveis.

Ocorreram tentativas em que a geometria ficou dividida em vários componentes independentes. A correção foi criar material contínuo na parte inferior ligando frente e traseira sem colidir com suportes, bateria e ESP32.

### Suportes de impressão indesejados

O fatiador indicava aproximadamente 45 minutos adicionais de material de suporte em uma região de ponte.

Foram adicionadas e reposicionadas colunas de apoio. Posteriormente foi descoberta a causa raiz: a região bloqueada pelo suporte do motor era bem maior do que a estimada inicialmente.

A correção foi feita medindo a colisão real e criando faixas estruturais somente onde necessário.

### Otimização final da carcaça

A faixa estrutural larga foi reduzida na maior parte da peça e mantida mais larga apenas ao redor dos quatro suportes. O volume caiu aproximadamente de 68.126 mm³ para 62.822 mm³, cerca de 8% de redução, mantendo a peça conectada.

---

## Sessão 2 — 17/09/2026 — eletrônica e firmware

### ESP32 e comunicação USB

A placa ESP32 usa interface USB-UART CP2102. Após instalação do driver Silicon Labs CP210x, a placa passou a ser reconhecida corretamente pelo Windows e os primeiros testes de firmware foram executados.

### Wi-Fi local

O ESP32 passou a criar sua própria rede, sem roteador e sem internet:

- SSID: `tony`;
- senha: `stark369`;
- IP: `192.168.4.1`;
- servidor HTTP local;
- DNS/captive portal para facilitar abertura do painel.

### Quatro motores e L298N

O projeto passou a utilizar quatro motores TT, dois por lado, ligados aos dois canais do L298N.

Pinagem atual:

| Função | GPIO |
|---|---:|
| ENA / PWM | 25 |
| IN1 | 26 |
| IN2 | 27 |
| ENB / PWM | 33 |
| IN3 | 32 |
| IN4 | 23 |

Os jumpers ENA/ENB foram removidos e o controle de velocidade passou a ser feito por PWM.

### Alimentação

A alimentação usada no protótipo mudou para três células 18650 em série:

- 11,1 V nominal;
- até 12,6 V carregadas;
- pack alimenta a entrada de potência da L298N;
- ESP32 permanece alimentado por USB/power bank durante os testes;
- GND compartilhado entre os circuitos.

**Risco identificado:** os motores TT são 3–6 V e a tensão do pack 3S é superior à especificação deles. O PWM baixo foi usado apenas como limitação de teste e não substitui uma fonte adequada para os motores.

### HC-SR04

O sensor utiliza:

- TRIG no GPIO 18;
- ECHO no GPIO 19;
- divisor de tensão construído com três resistores de 1 kΩ: 1 kΩ no ramo superior e 2 kΩ no ramo para GND.

A função do ultrassônico também mudou durante o desenvolvimento.

**Versão anterior:** bloquear avanço a aproximadamente 20 cm.

**Versão atual:** atuar como sensor de estacionamento para a ré.

- frente permanece liberada;
- a ré funciona normalmente acima de 5 cm;
- a 5 cm ou menos, a ré é bloqueada;
- o celular emite bipes cada vez mais rápidos conforme o obstáculo se aproxima;
- na faixa crítica, o alerta se torna contínuo.

### LDR

Foi adicionado um LDR ao GPIO 34 para adaptar a interface à iluminação do ambiente.

O painel possui três modos:

- `AUTO`;
- `CLARO`;
- `ESCURO`.

No modo automático, a luminosidade altera o tema. O firmware também mostra o ADC bruto para permitir calibração do sensor e grava logs no Serial Monitor aproximadamente a cada 5 segundos.

Ligação prevista:

```text
3V3 ── LDR ──┬── GPIO 34
              │
           resistor
              │
             GND
```

### Interface web

A interface atual mostra:

- estado do carrinho;
- distância do ultrassônico;
- PWM atual;
- luminosidade em percentual;
- ADC bruto do LDR;
- indicação de dia/meia-luz/noite;
- seleção de tema automático/claro/escuro;
- botão para ativar áudio no navegador;
- comandos direcionais para toque no celular.

---

## Dificuldades e ajustes mais relevantes

1. **Dimensões assumidas do CAD causaram retrabalho.** Sempre que possível, passaram a ser usadas medições ou testes contra a geometria real.
2. **Malha válida não significa corte válido.** Houve cortes fora da peça que não mudavam o sólido.
3. **Uma malha fechada pode conter vários corpos soltos.** A validação passou a incluir contagem de componentes conectados.
4. **O fatiador revelou problemas que o CAD sozinho não mostrava**, principalmente pontes sem apoio e tempo excessivo de suporte.
5. **A carcaça precisava conciliar motores, bateria, ESP32 e impressão rápida**, o que levou ao conceito baixo e aberto atual.
6. **O ultrassônico mudou de função conforme o uso real do carrinho:** saiu de bloqueio frontal para auxílio de ré.
7. **O LDR precisava ser observável para calibração.** Por isso foram adicionados ADC bruto e logs periódicos.
8. **A alimentação disponível não é ideal para os motores 3–6 V.** Essa limitação permanece documentada para evitar tratar PWM como regulador de tensão.

---

## Estado atual das peças

| Peça | Arquivo | Estado |
|---|---|---|
| T01 | `T01_base_chassi_v1.stl` | chassi/base atual otimizado |
| T02 | `T02_carcaca_v1.stl` | carcaça baixa atual, com aberturas para suportes e sensor |
| T03A | `T03A_suporte_motor_aberto_v7.stl` | suporte v7 |
| T03B | `T03B_presilha_v7.stl` | presilha v7 |
| T04 | `T04_tampa_v1.stl` | tampa 2 mm, não obrigatória no conceito atual |
| T05 | `T05_trava_suporte_v1.stl` | trava vertical |
| T06 | `T06_parafuso_impresso_v1.stl` | pino/parafuso impresso; 8 unidades previstas |

## Estado atual do firmware

A versão funcional atual é documentada como **v0.3** e inclui Wi-Fi local, PWM, quatro motores, HC-SR04 de ré, LDR, temas de interface e logs de diagnóstico.
