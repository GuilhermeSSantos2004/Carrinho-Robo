# Backlog do projeto

## Legenda

- **P0:** indispensável para o MVP;
- **P1:** necessário para o produto final;
- **P2:** melhoria opcional;
- **Estados:** A fazer, Em andamento, Em teste, Bloqueado ou Concluído.

## Backlog priorizado

| ID | Prioridade | Tarefa | Responsável | Dependência | Prazo | Critério de aceitação | Estado em 25/08/2026 |
|---|---|---|---|---|---:|---|---|
| BK-01 | P0 | Consolidar requisitos e croqui | Laura | - | 25/08 | Documentação revisada no GitHub | Concluído |
| BK-02 | P0 | Desenvolver firmware e página Wi-Fi | Guilherme | BK-01 | 25/08 | Código compila e cria a rede local | Concluído |
| BK-03 | P0 | Gravar firmware no ESP32 | Guilherme | BK-02 | 25/08 | Upload finalizado e hash verificado | Concluído |
| BK-04 | P0 | Conferir L298N, motores, bateria e HC-SR04 | Gabriel | BK-01 | 26/08 | Componentes identificados e sem dano visível | Em andamento |
| BK-05 | P0 | Montar alimentação e GND comum | Gabriel | BK-04 | 26/08 | Tensões conferidas antes de ligar os módulos | A fazer |
| BK-06 | P0 | Ligar ESP32 à L298N | Gabriel | BK-05 | 27/08 | GPIOs conectados conforme o firmware | A fazer |
| BK-07 | P0 | Testar cada motor separadamente | Danilo | BK-06 | 27/08 | Cada canal gira apenas o motor correspondente | A fazer |
| BK-08 | P0 | Corrigir sentido dos motores | Guilherme | BK-07 | 27/08 | Comandos produzem o sentido esperado | A fazer |
| BK-09 | P0 | Testar página pelo celular | Guilherme | BK-03, BK-07 | 28/08 | Cinco comandos funcionam pelo Wi-Fi | A fazer |
| BK-10 | P0 | Produzir base provisória do MVP | Enricco | BK-04 | 28/08 | Componentes ficam firmes e rodas giram livres | A fazer |
| BK-11 | P0 | Integrar eletrônica à base provisória | Danilo | BK-05, BK-10 | 29/08 | Carrinho montado sem cabos nas partes móveis | A fazer |
| BK-12 | P0 | Validar MVP integrado | Todos | BK-08, BK-09, BK-11 | 29/08 | Todos os critérios MVP-01 a MVP-08 aprovados | A fazer |
| BK-13 | P0 | Publicar documentação da Tarefa 17 | Laura | BK-12 | 01/09 | Cinco entregáveis disponíveis em `/organizacao` | A fazer |
| BK-14 | P1 | Medir componentes com paquímetro | Enricco | BK-04 | 03/09 | Medidas reais registradas em tabela | A fazer |
| BK-15 | P1 | Modelar chassi e carenagem em CAD | Enricco | BK-14 | 10/09 | Modelo contém suportes, furos e folgas | A fazer |
| BK-16 | P1 | Imprimir corpos de prova | Enricco | BK-15 | 12/09 | Furos, motor e tampa testados | A fazer |
| BK-17 | P1 | Corrigir CAD após teste de encaixe | Enricco | BK-16 | 15/09 | Peças encaixam sem força ou folga excessiva | A fazer |
| BK-18 | P1 | Imprimir peças definitivas | Danilo | BK-17 | 20/09 | Base, tampa e suportes sem defeitos críticos | A fazer |
| BK-19 | P1 | Montar sensor com divisor de tensão | Gabriel | BK-05, BK-18 | 22/09 | ECHO chega ao ESP32 com aproximadamente 3,3 V | A fazer |
| BK-20 | P1 | Validar parada diante de obstáculo | Guilherme | BK-19 | 24/09 | Avanço bloqueado abaixo de 20 cm | A fazer |
| BK-21 | P1 | Organizar cabos, bateria e chave geral | Danilo | BK-18, BK-19 | 25/09 | Cabos protegidos e bateria firmemente presa | A fazer |
| BK-22 | P1 | Executar plano completo de testes | Laura | BK-20, BK-21 | 27/09 | Resultados registrados com evidências | A fazer |
| BK-23 | P1 | Corrigir falhas encontradas | Responsável do item | BK-22 | 29/09 | Nenhuma falha crítica aberta | A fazer |
| BK-24 | P1 | Publicar versão final | Todos | BK-23 | 30/09 | Código, CAD/STL, fotos, vídeo e relatório no GitHub | A fazer |
| BK-25 | P2 | Implementar LEDs ou buzzer | Gabriel | BK-24 | Após 30/09 | Não pode atrasar os itens P0 ou P1 | A fazer |

## Distribuição de responsabilidades

| Integrante | Responsabilidade principal |
|---|---|
| Enricco Rossi | Medições, CAD, chassi e carenagem 3D |
| Gabriel Marquez | Alimentação, ponte H L298N, sensor e conferência elétrica |
| Guilherme Silva | Firmware do ESP32, controle Wi-Fi e integração de software |
| Danilo Urze | Montagem, impressão, organização física e testes dos motores |
| Laura Claro | Requisitos, Kanban, registros, evidências e revisão da documentação |

Todos os integrantes participam da validação do MVP e da revisão final.
