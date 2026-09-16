# MVP - Minimum Viable Product

## 1. Objetivo

Entregar a menor versão funcional do carrinho-robô capaz de se movimentar com segurança e ser controlada por um celular. O MVP comprovará a integração entre **ESP32**, **ponte H L298N**, **dois motores DC**, alimentação e interface web via Wi-Fi.

O chassi do MVP pode utilizar uma montagem provisória ou a primeira versão impressa em 3D. O acabamento definitivo da carenagem não é necessário para validar o funcionamento essencial.

## 2. Marcos e datas

| Marco | Data planejada | Condição para aprovação |
|---|---:|---|
| Protótipo eletrônico | 27/08/2026 | ESP32 aciona separadamente os dois motores pela L298N |
| MVP minimamente funcional | 29/08/2026 | Carrinho executa avançar, recuar, esquerda, direita e parar pelo celular |
| Documentação da Tarefa 17 | 01/09/2026 | MVP, MoSCoW, Backlog, Dependências e Kanban publicados |
| Produto final concluído | 30/09/2026 | Estrutura 3D definitiva, sensor ultrassônico, organização elétrica e testes aprovados |

## 3. Escopo do MVP

O MVP deverá possuir:

- ESP32 com firmware carregado;
- ponte H L298N ligada aos dois motores DC;
- alimentação compatível com os motores e GND comum;
- estrutura capaz de sustentar os componentes;
- rede Wi-Fi local criada pelo ESP32;
- página web acessível pelo celular em `http://192.168.4.1`;
- comandos de avançar, recuar, virar à esquerda, virar à direita e parar;
- parada automática quando o comando deixar de ser recebido;
- teste inicial realizado com as rodas ou esteiras suspensas.

## 4. Critérios de aceitação do MVP

| Código | Critério | Como validar | Resultado esperado |
|---|---|---|---|
| MVP-01 | Inicialização | Ligar o ESP32 e observar a rede Wi-Fi | Rede `Carrinho-ESP32` disponível em até 30 segundos |
| MVP-02 | Acesso pelo celular | Conectar à rede e abrir o endereço local | Página de controle carregada corretamente |
| MVP-03 | Motor esquerdo | Acionar somente o canal A da L298N | Apenas o motor esquerdo gira |
| MVP-04 | Motor direito | Acionar somente o canal B da L298N | Apenas o motor direito gira |
| MVP-05 | Movimentos | Acionar todos os botões da interface | Cinco comandos executados corretamente |
| MVP-06 | Parada | Soltar o botão ou tocar em parar | Motores param imediatamente |
| MVP-07 | Fail-safe | Interromper o envio de comandos | Carrinho para automaticamente em até 1 segundo |
| MVP-08 | Alimentação segura | Verificar o circuito antes do teste | Motores não são alimentados pelo ESP32 e todos os GNDs estão interligados |

## 5. Produto final

Após a validação do MVP, a versão final deverá acrescentar:

- chassi e carenagem definitivos impressos em 3D;
- tampa removível e fixações com parafusos;
- duas esteiras alinhadas e tensionadas;
- sensor HC-SR04 instalado na parte frontal;
- bloqueio do avanço quando houver obstáculo a menos de 20 cm;
- bateria fixada, chave geral e cabos protegidos;
- controle de velocidade por PWM;
- evidências de montagem e testes no GitHub.

## 6. Definição de pronto

O produto será considerado concluído quando:

1. todos os requisitos classificados como **Must Have** estiverem aprovados;
2. o carrinho operar por pelo menos 10 minutos sem reiniciar ou apresentar aquecimento anormal;
3. nenhum cabo tocar nas esteiras ou em partes móveis;
4. o sensor impedir uma colisão frontal durante o teste;
5. fotos, vídeo, código-fonte e resultados dos testes estiverem registrados no repositório;
6. o grupo realizar uma revisão final e não houver bloqueios críticos no Kanban.

## 7. Fora do MVP

Não fazem parte da primeira versão: câmera, GPS, RFID, display OLED, aplicativo nativo, navegação autônoma avançada e controle pela internet. Esses itens aumentariam o risco de *feature creep* antes da validação das funções essenciais.

