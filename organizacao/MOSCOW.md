# Priorização MoSCoW

O método MoSCoW foi utilizado para controlar o escopo e evitar que funcionalidades adicionais prejudiquem a entrega do carrinho funcional.

## Must Have - obrigatório

| ID | Funcionalidade | Justificativa |
|---|---|---|
| M-01 | ESP32 como placa controladora | É o controlador definido pelo grupo e fornece Wi-Fi integrado |
| M-02 | Dois motores DC controlados pela L298N | Permitem tração diferencial e realização das curvas |
| M-03 | Avançar, recuar, esquerda, direita e parar | São os movimentos essenciais do carrinho |
| M-04 | Controle Wi-Fi pelo celular | É a interface principal definida para o projeto |
| M-05 | Chassi capaz de sustentar os componentes | Garante a montagem física e o deslocamento do conjunto |
| M-06 | Alimentação segura e GND comum | Evita danos aos módulos e funcionamento instável |
| M-07 | Parada automática por perda de comando | Reduz o risco de movimento descontrolado |
| M-08 | Sensor HC-SR04 com divisor no ECHO | Detecta obstáculos sem aplicar 5 V diretamente ao ESP32 |
| M-09 | Bloqueio do avanço diante de obstáculo | Evita colisões frontais |
| M-10 | Código e documentação publicados no GitHub | Compõem a entrega e permitem reprodução do projeto |

## Should Have - importante

| ID | Funcionalidade | Justificativa |
|---|---|---|
| S-01 | Chassi e carenagem definitivos impressos em 3D | Protegem e organizam os componentes |
| S-02 | Tampa removível | Facilita manutenção e troca da bateria |
| S-03 | Controle de velocidade por PWM | Melhora a condução e o alinhamento do carrinho |
| S-04 | Esteiras com ajuste de tensão | Evita que as esteiras escapem durante as curvas |
| S-05 | Chave geral liga/desliga | Permite desligamento rápido e seguro |
| S-06 | Indicador de distância na página | Ajuda a verificar o funcionamento do HC-SR04 |
| S-07 | Registro de fotos, vídeo e resultados | Demonstra a validação do produto |

## Could Have - interessante

| ID | Funcionalidade | Justificativa |
|---|---|---|
| C-01 | LEDs de estado | Indicam ligado, conectado e obstáculo detectado |
| C-02 | Buzzer | Pode emitir alerta de obstáculo ou bateria fraca |
| C-03 | Indicador de nível da bateria | Facilita o planejamento de recarga |
| C-04 | Modo semiautônomo simples | Permite parar e desviar de obstáculos |
| C-05 | Personalização estética da carenagem | Aproxima o resultado da referência visual |

## Won't Have - não terá nesta versão

| ID | Funcionalidade | Motivo |
|---|---|---|
| W-01 | Câmera e transmissão de vídeo | Aumentam consumo, custo e complexidade |
| W-02 | GPS | Não é necessário para testes em ambiente interno |
| W-03 | RFID | Não contribui para o objetivo principal do MVP |
| W-04 | Display OLED | A interface já será exibida no celular |
| W-05 | Aplicativo Android ou iOS nativo | A página hospedada pelo ESP32 atende ao controle básico |
| W-06 | Controle remoto pela internet | O projeto utilizará rede local do ESP32 |
| W-07 | Versão com quatro motores | O projeto adotará dois motores e tração diferencial |
| W-08 | Navegação autônoma avançada | Será avaliada somente após a conclusão da versão atual |

## Regra de mudança de escopo

Uma funcionalidade nova só poderá entrar no projeto se todos os itens **Must Have** estiverem concluídos, se não alterar a data final e se houver aprovação do grupo. Caso contrário, será registrada para uma versão futura.

