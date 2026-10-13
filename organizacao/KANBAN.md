# Kanban do projeto

**Data da atualização:** 25/08/2026  
**Limite sugerido de trabalho em andamento:** até 3 tarefas simultâneas.

## Quadro atual

| Backlog | A fazer | Em andamento | Em teste | Concluído |
|---|---|---|---|---|
| BK-14 Medir peças | BK-05 Alimentação e GND | BK-04 Conferir componentes | BK-03 Firmware gravado no ESP32 | BK-01 Requisitos e croqui |
| BK-15 Modelar CAD | BK-06 Ligar ESP32-L298N |  |  | BK-02 Firmware e página Wi-Fi |
| BK-16 Imprimir corpos de prova | BK-07 Testar motores |  |  | BK-03 Upload no ESP32 validado |
| BK-17 Corrigir CAD | BK-08 Corrigir sentidos |  |  | Diagrama elétrico produzido |
| BK-18 Imprimir peças finais | BK-09 Testar página no celular |  |  |  |
| BK-19 Montar HC-SR04 | BK-10 Base provisória |  |  |  |
| BK-20 Testar anticolisão | BK-11 Integrar base e eletrônica |  |  |  |
| BK-21 Organizar cabos e bateria | BK-12 Validar MVP |  |  |  |
| BK-22 Executar testes finais | BK-13 Publicar Tarefa 17 |  |  |  |
| BK-23 Corrigir falhas |  |  |  |  |
| BK-24 Publicar versão final |  |  |  |  |
| BK-25 LEDs ou buzzer |  |  |  |  |

## Próximas movimentações

1. Ao finalizar **BK-04**, mover **BK-05** para “Em andamento”.
2. Após conferir alimentação e GND, executar **BK-06** e **BK-07**.
3. Se os dois motores responderem corretamente, mover **BK-09** para “Em teste”.
4. Construir a base provisória em paralelo com os testes elétricos.
5. Quando BK-08, BK-09 e BK-11 estiverem concluídos, iniciar **BK-12 - Validar MVP**.

## Regras do Kanban

- toda tarefa deve possuir responsável e critério de aceitação;
- tarefas bloqueadas devem registrar o motivo e a ação necessária;
- uma tarefa somente vai para “Concluído” depois da validação;
- o quadro deve ser atualizado sempre que houver mudança de estado;
- itens opcionais não podem ocupar o lugar de uma tarefa P0;
- cada atualização relevante deve ser registrada por commit no GitHub.

## Modelo para atualização semanal

| Data | Tarefa | Estado anterior | Novo estado | Responsável | Evidência/observação |
|---|---|---|---|---|---|
| DD/MM/AAAA | BK-XX | A fazer | Em andamento | Nome | Link do commit, foto ou resultado |

## Quadro CP2 — atualização de 13/10/2026

| Backlog | A fazer | Em andamento | Em teste | Concluído |
|---|---|---|---|---|
| Coleta física e CSV real | Importar no Edge Impulse | Firmware CP2 com ação | — | Documentação e roteiro; baseline sintético identificado |
| Biblioteca exportada | Treinar e registrar métricas | — | — | Histórico CP1 → CP2 organizado |
| Vídeo específico CP2 | Prints de inferência | — | — | — |

### Critério para mover tarefas

Uma tarefa CP2 só pode ir para **Concluído** quando tiver arquivo/print/commit ou
registro físico correspondente. A existência do sketch não vale como prova de upload
no ESP32; a existência de `model/model.h` também não vale como prova de export do
Edge Impulse.

