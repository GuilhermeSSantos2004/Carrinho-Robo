# Validação — 24/09/2026

## Executado neste ambiente

- Leitura das instruções do PDF e conferência dos requisitos da página 14.
- Pinagem conferida com `hardware/arquitetura/README.md` e `src/codigo.ino` do repositório.
- **6 testes Python aprovados:** importação com timestamps/comentários, rejeição de valores corrompidos, rejeição de coleta vazia, preservação de falhas brutas, validação estrita do CSV final e proteção de arquivo existente na importação.
- **Teste C++ aprovado**, compilado com `g++ -std=c++11 -Wall -Wextra -Werror`: aquecimento da janela, média móvel, fronteira de 20 cm, saturação em 100 cm, reinício após falhas, limites físicos e rejeição de NaN/infinito.
- Dados sintéticos usados somente nos testes de software, não apresentados como medições reais.

Comandos para repetir a partir desta pasta:

```bash
python3 -m unittest discover -s tests -v
g++ -std=c++11 -Wall -Wextra -Werror tests/test_processamento.cpp -o /tmp/test-tinyml
/tmp/test-tinyml
```

## Limitações e pendências

- **Compilação dos sketches para ESP32 ainda não validada.** Arduino CLI foi obtido, mas a instalação do core não pôde baixar os índices: `network is unreachable`. O teste C++ usa a lógica real de `processamento.h`, mas não substitui a compilação Arduino nem valida as APIs/pinos na placa.
- Upload, comunicação serial, montagem elétrica e execução física não testados neste ambiente.
- CSV do sensor, mínimo/máximo observado, ruído e estabilidade ainda dependem da coleta física.
- A validação do CSV verifica formato e coerência dos rótulos; não prova que os dados vieram do sensor. Guardar TXT original, fotos e registro de coleta.

Na Arduino IDE, conferir o modelo da placa, selecionar o core ESP32 instalado e compilar os dois sketches. Enviar o texto completo de qualquer erro e registrar a versão do core usada.
