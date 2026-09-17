# Check Point 01 — Validação da entrega

Este arquivo relaciona os critérios oficiais do Check Point 01 com a documentação atualmente disponível no repositório.

## Grupo

| Integrante | RM |
|---|---|
| Enricco Rossi de Souza Carvalho Miranda | RM551717 |
| Gabriel Marquez Trevisan | RM99227 |
| Guilherme Silva dos Santos | RM551168 |
| Danilo Urze Aldred | RM99465 |
| Laura Claro Mathias | RM98747 |

## 1. Carrinho-robô — 60%

| Critério | Peso | Evidência documental |
|---|---:|---|
| Chassi e projeto mecânico | 15% | Chassi próprio, CAD, arquivos-fonte, STEP/STL, renders, versões e histórico em `cad/` e `historico/`. |
| Movimentação e sistema elétrico | 15% | Firmware contém frente, ré, curvas, parada e PWM; arquitetura documenta ESP32, L298N, quatro motores e alimentação. A pontuação prática depende da demonstração do protótipo físico. |
| Controle remoto sem fio | 10% | Wi-Fi próprio criado pelo ESP32 e interface web local em `192.168.4.1`. |
| Sensor integrado | 10% | HC-SR04 integrado como sensor de estacionamento de ré; LDR integrado à interface. |
| Carenagem e acabamento | 10% | T02 e evolução da carenagem documentados em CAD e histórico. A avaliação de acabamento depende da peça física apresentada. |

## 2. Repositório GitHub — 40%

| Critério | Peso | Evidência no repositório |
|---|---:|---|
| README e apresentação | 5% | Nome, integrantes/RM, objetivo, funcionalidades, imagens de desenvolvimento, registro físico existente e organização do conteúdo no `README.md`. |
| Requisitos, planejamento e evolução | 7% | `organizacao/` contém backlog, MVP, MoSCoW, Kanban, dependências e custos; `historico/` registra decisões, erros e alterações. |
| Projeto mecânico e fabricação | 8% | `cad/` preserva fontes, STEP/STL, renders e diversas versões do desenvolvimento do chassi/suportes/carenagem. |
| Hardware e eletrônica | 5% | `hardware/` documenta componentes, ESP32, quatro motores TT, L298N, bateria 3S, sensores, Wi-Fi, pinagem e diagrama. |
| Software | 7% | `src/` contém firmware principal, versão v0.3 e explicação do funcionamento. |
| Testes e resultados | 4% | `historico/HISTORICO_PROJETO_CUBI04.md` registra problemas, testes, correções e decisões; README resume os principais resultados. |
| Evidências finais | 4% | `docs/EVIDENCIAS_CHECKPOINT.md` centraliza renders, registro físico, vídeo existente, software, hardware e instruções. Evidências físicas apresentadas devem corresponder ao protótipo real. |

## Evidências organizadas

A página [`docs/EVIDENCIAS_CHECKPOINT.md`](docs/EVIDENCIAS_CHECKPOINT.md) reúne os materiais já disponíveis e explica o papel de cada evidência. O README também incorpora imagens do processo diretamente na apresentação principal.

## Adaptação mecânica registrada em 15/09/2026

O histórico registra especificamente o problema de alinhamento do conjunto motor/roda e as adaptações realizadas no chassi e nos suportes: reposicionamento, centralização do eixo, redução de folga, reforço, rodas para fora do perímetro do chassi e preservação das aberturas da carenagem necessárias ao encaixe dos suportes.

## Conferência para apresentação física

A documentação foi organizada para cobrir todos os tópicos pedidos na rubrica. A nota dos critérios físicos continua dependendo do que o protótipo realmente demonstrar ao avaliador. Para tornar a apresentação inequívoca, a demonstração deve mostrar o carrinho montado, alimentação, frente/ré/curvas, controle pelo celular, sensor e carenagem. Uma foto ou vídeo só deve ser chamado de "final" quando representar a montagem efetivamente entregue.

## Roteiro curto para demonstração

1. mostrar o carrinho e a carenagem;
2. mostrar a alimentação e eletrônica;
3. conectar o celular à rede `tony` e abrir `192.168.4.1`;
4. demonstrar frente, ré, esquerda, direita e parada;
5. variar o PWM;
6. demonstrar o HC-SR04 durante a ré;
7. demonstrar o LDR alterando a leitura/interface;
8. mostrar rapidamente os componentes internos e finalizar com o carrinho completo.
