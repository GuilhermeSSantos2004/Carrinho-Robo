# Check Point 01 — Checklist de validação

Este arquivo relaciona os critérios oficiais do Check Point 01 com as evidências existentes no repositório e destaca o que ainda precisa ser registrado fisicamente antes da entrega.

## Grupo

| Integrante | RM |
|---|---|
| Enricco Rossi de Souza Carvalho Miranda | RM551717 |
| Gabriel Marquez Trevisan | RM99227 |
| Guilherme Silva dos Santos | RM551168 |
| Danilo Urze Aldred | RM99465 |
| Laura Claro Mathias | RM98747 |

## 1. Carrinho-robô — 60%

| Critério | Peso | Situação documental | Evidência / ação |
|---|---:|---|---|
| Chassi e projeto mecânico | 15% | Documentado | CAD próprio, fontes, STLs, versões, renders e histórico em `cad/` e `historico/`. A avaliação dos 15% depende do chassi próprio estar efetivamente impresso/montado. |
| Movimentação e sistema elétrico | 15% | Implementado no firmware; precisa evidência final | Código contempla frente, ré, curvas, parada e PWM. Registrar vídeo do carrinho montado funcionando por bateria e demonstrando estabilidade. |
| Controle remoto sem fio | 10% | Implementado | ESP32 cria Wi-Fi próprio e página web local em `192.168.4.1`. O vídeo final deve mostrar o celular comandando o carrinho. |
| Sensor integrado | 10% | Implementado | HC-SR04 integrado como sensor de estacionamento de ré, com bipes progressivos e bloqueio da ré a 5 cm ou menos. LDR também está integrado à interface. Demonstrar ao menos um sensor no vídeo final. |
| Carenagem e acabamento | 10% | Projeto documentado; precisa evidência final | T02/carenagem está documentada. Registrar fotos da carenagem instalada, fixação, acabamento e acesso aos componentes. |

## 2. Repositório GitHub — 40%

| Critério | Peso | Situação | Evidência / ação |
|---|---:|---|---|
| README e apresentação | 5% | Parcial | README possui nome, grupo, objetivo, descrição, funcionalidades e organização. **Falta colocar no README uma foto identificada como carrinho finalizado.** |
| Requisitos, planejamento e evolução | 7% | Atendido | `organizacao/` contém backlog, MVP, MoSCoW, Kanban, dependências e planejamento; `historico/` registra decisões e evolução. |
| Projeto mecânico e fabricação | 8% | Quase completo | Existem arquivos-fonte, STEP/STL, renders e versões em `cad/`. **Faltam fotos/registros claros da fabricação/impressão das peças finais e da montagem física final.** |
| Hardware e eletrônica | 5% | Parcial | Componentes, ESP32, motores, L298N, alimentação, sensores, Wi-Fi e diagrama estão documentados. **Faltam fotos claras da montagem elétrica real.** |
| Software | 7% | Atendido | Código-fonte em `src/`, versão congelada, documentação de PWM, motores, Wi-Fi, HC-SR04, LDR e interface web. |
| Testes e resultados | 4% | Parcial | Histórico registra problemas e correções. **Adicionar evidência do teste final integrado e seu resultado.** |
| Evidências finais | 4% | Parcial | Já existem mídia e vídeo em `docs/videos/`, mas a entrega deve deixar inequívoco que mostram a versão final. **Adicionar/renomear evidências finais e documentar o que cada foto/vídeo demonstra.** |

## Evidências finais que ainda devem ser produzidas

Para fechar os itens que dependem do protótipo físico, registrar:

1. foto geral do carrinho completamente montado, com chassi e carenagem;
2. foto da parte interna mostrando ESP32, L298N, bateria, fiação e sensores;
3. foto ou pequeno registro do processo de impressão/fabricação das peças finais;
4. vídeo mostrando frente, ré, curva para os dois lados e parada;
5. no mesmo vídeo ou em outro, mostrar o celular conectado ao Wi-Fi do ESP32 e comandando o carrinho;
6. demonstração do HC-SR04 durante a ré, incluindo os bipes e, se possível, o bloqueio próximo de 5 cm;
7. demonstração do LDR alterando a indicação/tema da interface;
8. registrar no README a foto final e links para os vídeos finais.

## Roteiro recomendado para o vídeo final

1. mostrar rapidamente o carrinho montado;
2. mostrar a alimentação por bateria;
3. mostrar o celular conectado à rede do ESP32 e abrir `192.168.4.1`;
4. executar frente, ré, esquerda e direita;
5. variar o PWM pelo controle;
6. aproximar um obstáculo durante a ré para demonstrar o HC-SR04 e os bipes;
7. cobrir/iluminar o LDR para demonstrar a mudança de luminosidade/tema;
8. finalizar mostrando a carenagem e a montagem interna.

## Observação

O repositório contém grande parte da documentação exigida. Os pontos ainda pendentes são principalmente **evidências físicas finais**. Não é correto marcar esses itens como concluídos antes de existirem fotos/vídeos reais da versão que será apresentada. Após adicionar as evidências, atualizar este checklist e o README com os caminhos definitivos.
