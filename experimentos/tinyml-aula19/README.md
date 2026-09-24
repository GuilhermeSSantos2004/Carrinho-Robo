# Aula 19 — Práticas iniciais com TinyML

**CUBI-04 · Start-up One · Experimento em fase de teste**

Esta pasta atende à preparação da atividade da profª Dra. Gedeane G. S. Kenshima, disciplina Project-based Maker Lab. A referência é o PDF **Aula 19 - Práticas iniciais com TinyML.pdf**, especialmente as páginas 8, 10–14. O firmware principal do carrinho continua em [`../../src/`](../../src/); estes sketches são independentes e não implementam controle remoto.

> **Situação:** códigos e roteiro preparados. Coleta física, mínimos/máximos observados, avaliação de ruído e CSV real ainda pendentes. Não há modelo de ML treinado: a aula pede engenharia do dado e rótulos definidos por limiares.

## O que a atividade pede

| Requisito da aula | Material | Situação |
|---|---|---|
| Escolher um sensor | HC-SR04 já documentado no carrinho | Definido; conferir módulo físico |
| Tipo da variável, valores típicos, mínimo e máximo | [RELATORIO.md](RELATORIO.md) | Especificação preenchida; observações pendentes |
| Código antes do pré-processamento | [leitura_bruta.ino](firmware/leitura_bruta/leitura_bruta.ino) | Implementado |
| Código com pelo menos 2 pré-processamentos e limiares | [leitura_processada.ino](firmware/leitura_processada/leitura_processada.ino) e [processamento.h](firmware/leitura_processada/processamento.h) | Implementado: 3 operações |
| Tipo de problema, classes e critério de rótulo | Classificação binária perto/longe | Definido |
| CSV `feature,label` com dados do sensor | `dados/dataset.csv`, gerado após coleta | **Pendente de medição real** |
| Caracterizar estabilidade e ruído | Coleta bruta com alvo imóvel e análise | **Pendente de medição real** |

## 1. Preparar o circuito

Use o **ESP32 DevKit V1 / ESP32 clássico** documentado no repositório. Se a sua placa for ESP32-S3 ou outro modelo, confirme a identificação e a pinagem antes de gravar: este exemplo não foi configurado para ela.

**Desconecte a bateria dos motores e alimente o ESP32 por USB.** Os sketches também colocam os pinos da L298N em LOW, mas a desconexão física evita acionamento durante reinício/upload. O upload substitui o programa que está na memória do ESP32; os arquivos do firmware original permanecem no GitHub.

| HC-SR04 | Ligação |
|---|---|
| VCC | 5 V da placa alimentada por USB, conforme pinagem da placa |
| GND | GND do ESP32 |
| TRIG | GPIO 18 |
| ECHO | GPIO 19 **através do divisor resistivo** |

Divisor já documentado no carrinho: ECHO → resistor de 1 kΩ → ponto conectado ao GPIO 19; desse ponto até GND, dois resistores de 1 kΩ em série. Nunca aplique o ECHO de 5 V diretamente ao GPIO. Veja a [arquitetura existente](../../hardware/arquitetura/README.md#hc-sr04--esp32).

## 2. Ler o sensor sem filtragem

1. Baixe/atualize o repositório e abra `firmware/leitura_bruta/leitura_bruta.ino` na Arduino IDE. Mantenha o nome da pasta igual ao sketch.
2. Instale o pacote **esp32 by Espressif Systems** no gerenciador de placas, se necessário. Selecione **ESP32 Dev Module** para a placa clássica documentada, e a porta COM correspondente.
3. Compile e faça upload. Se ficar preso em `Connecting...`, mantenha BOOT pressionado no início da conexão e solte quando começar a gravação.
4. Abra o Serial Monitor em **115200 baud**, preferencialmente com timestamps desativados. Pressione EN/reset com o monitor aberto para ver o cabeçalho.
5. Observe por alguns minutos. Use uma superfície plana de frente para o sensor, evitando mão inclinada e materiais absorventes.
6. Faça uma coleta com o alvo **parado a cerca de 30 cm durante 60 segundos**. Salve as linhas em `dados/serial_bruto_parado.txt`.
7. Faça outra coleta por cerca de 2 minutos, movendo o alvo entre aproximadamente 5 e 100 cm. Salve em `dados/serial_bruto_variado.txt`. Esses tempos são uma proposta de protocolo do grupo; o PDF não fixa um número de amostras.

Saída: `tempo_ms,duracao_us,distancia_cm,status`. A distância é calculada por `duracao_us × 0.0343 / 2`. O pulso original também é preservado. Não há média, normalização nem corte de distância neste sketch. `SEM_ECO` fica com distância vazia; `FORA_FAIXA` preserva o valor medido. Apenas valores `OK` entram nas estatísticas válidas.

## 3. Ler com pré-processamento

Abra `firmware/leitura_processada/leitura_processada.ino`, mantendo `processamento.h` na mesma pasta, e grave na placa. Abra o Serial Monitor em 115200 baud e reinicie a placa.

Pipeline implementado:

1. Medir aproximadamente a cada **100 ms**; timeout de eco em 30 ms.
2. Rejeitar ausência de eco e distância fora de **2–400 cm**. Uma falha reinicia a janela; não é transformada em zero nem em uma classe.
3. **Média móvel das últimas 5 leituras válidas consecutivas.** Só emitir dados quando a janela estiver completa. As cinco leituras cobrem aproximadamente 400 ms entre primeira e última.
4. **Limitação da média à faixa de trabalho de 0–100 cm.** Como só entram distâncias válidas positivas, basta limitar o teto. Distâncias maiores que 100 cm saturam em 100 cm; não significam medição exata de 100 cm.
5. **Normalização:** `feature = media_limitada_cm / 100`. Arredondar para seis casas antes de definir o rótulo.
6. Classificar a feature final, conforme a tabela.

| Label | Classe | Critério exato do CSV | Equivalência em cm |
|---|---|---|---|
| 0 | Perto | `feature < 0.200000` | média menor que aproximadamente 20 cm |
| 1 | Longe | `feature >= 0.200000` | média a partir de aproximadamente 20 cm |

O arredondamento de seis casas faz parte do critério na fronteira. O limiar de 20 cm é uma escolha **didática deste experimento**, independente do bloqueio de ré do firmware principal. São regras de rotulagem, não inferência de um modelo treinado.

Mantenha o objeto em diversas posições de cada classe: por exemplo, 5, 10 e 15 cm para perto; 25, 40, 60 e 90 cm para longe. Colete cerca de 60 segundos por classe, variando a posição a cada 10 segundos. Ao mudar a posição, aguarde cerca de 1 segundo antes de copiar um novo trecho, para a média se estabilizar. Procure números semelhantes de amostras por classe. Teste separadamente a região de 20 cm para observar oscilações de rótulo.

Copie as linhas para `dados/serial_processado.txt`. O cabeçalho será `feature,label`; as mensagens que começam com `#` são diagnósticos e não pertencem ao CSV final. Preserve o TXT original, incluindo falhas, como evidência.

## 4. Gerar CSV e conferir os dados

Abra o terminal dentro de `experimentos/tinyml-aula19`. Requer **Python 3.9 ou superior**, sem instalar bibliotecas. No Windows use `py`; no Linux/macOS substitua por `python3`.

```powershell
py ferramentas/dados.py importar dados/serial_bruto_parado.txt --tipo bruto --saida dados/bruto_parado.csv
py ferramentas/dados.py importar dados/serial_bruto_variado.txt --tipo bruto --saida dados/bruto_variado.csv
py ferramentas/dados.py importar dados/serial_processado.txt --tipo processado --saida dados/dataset.csv
py ferramentas/dados.py analisar dados/dataset.csv --tipo processado
```

A ferramenta remove cabeçalhos repetidos, comentários e timestamps usuais do Arduino IDE (`HH:MM:SS.mmm ->`). Verifica números finitos, faixa da feature, classes e coerência dos rótulos. Recusa linhas numéricas corrompidas e não sobrescreve arquivos existentes; para repetir, use um novo nome de saída. Não insere nem inventa amostras.

Na coleta bruta, imprime mínimo, máximo, média, amplitude, desvio padrão e contagens de falhas. Use o arquivo com alvo parado para falar de **ruído e estabilidade**. A variação da coleta com movimento não é uma medida de ruído do sensor.

Você também pode montar o CSV manualmente conforme a aula: cabeçalho `feature,label`, uma medição por linha, ponto decimal e vírgula separando as duas colunas. Não inclua timestamps nem diagnósticos. Não substitua valores ausentes por zero. Evite o Excel alterar o separador para ponto e vírgula.

## 5. O que enviar para concluir

- Foto legível da placa (nome do módulo) e das conexões do HC-SR04/divisor.
- `serial_bruto_parado.txt` e `serial_bruto_variado.txt`, ou os CSVs correspondentes.
- `serial_processado.txt` e, se conseguir gerar, `dataset.csv`.
- Distância aproximada do alvo parado, duração de cada coleta e observações sobre variações/falhas.
- Foto/print do Serial Monitor em cada sketch. Se ocorrer erro, envie o texto completo da compilação/upload.

Com esses arquivos, é possível preencher os resultados reais no [relatório](RELATORIO.md), verificar as duas classes e preparar a entrega. Não entregar apenas a estrutura da pasta como se a prática física já estivesse concluída.

## Limitações e próximos passos

A média reduz oscilações rápidas, mas gera atraso; perto do limiar as classes podem alternar. Temperatura, ângulo, tamanho e material do alvo afetam a medição. Não implementamos histerese para manter o critério de rótulo explícito da aula. Se houver futuro treinamento, separar treino/teste por sessão de coleta, pois janelas consecutivas compartilham dados. Um modelo treinado com rótulos desta regra aprenderá a reproduzir a regra; isso não demonstra capacidade de evitar colisões.

Para voltar ao carrinho, abra uma cópia de `src/codigo.ino` em uma pasta chamada `codigo`, compile e faça upload do firmware original; teste com rodas suspensas antes de recolocar em uso.

## Validação de software

Veja [VALIDACAO.md](VALIDACAO.md). Os testes utilizam entradas sintéticas isoladas e **não constituem CSV de entrega**.

## Referências

- Aula 19 — Práticas iniciais com TinyML, PDF fornecido pelo usuário, pp. 8 e 10–14.
- [Datasheet HC-SR04 (ElecFreaks, hospedado pela SparkFun)](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf): faixa nominal, alimentação e temporização.
- [Referência Arduino: pulseIn](https://docs.arduino.cc/language-reference/en/functions/advanced-io/pulseIn/).
- [Pinagem e circuito atuais do CUBI-04](../../hardware/arquitetura/README.md).
