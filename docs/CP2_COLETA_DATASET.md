# CP2 — Coleta do dataset real

O CSV do CP2 precisa vir do sensor do carrinho. O arquivo [`dataset.csv`](dataset.csv)
já existente no repositório é **sintético** e pertence à Tarefa 21; ele não deve ser
apresentado como coleta real.

## Sensor e entradas

São usados três HC-SR04 frontais:

| Entrada | Sensor | TRIG | ECHO |
|---|---|---:|---:|
| `esquerda_cm` | frontal esquerdo | 19 | 23 |
| `centro_cm` | frontal central | 5 | 18 |
| `direita_cm` | frontal direito | 4 | 13 |

Cada ECHO precisa passar pelo divisor de tensão antes de chegar ao ESP32. A tabela
acima é o perfil CP2 descrito na especificação de 2026; se a montagem física usar
outra pinagem, corrigir o sketch e registrar a alteração antes da coleta.

## Classes

Use o mesmo cenário físico em todas as sessões, movendo um obstáculo grande e plano
para a região correspondente. Não rotule a partir de uma regra inventada depois da
medição.

| Label | Classe | Cenário para coletar |
|---:|---|---|
| 0 | `LIVRE` | sem obstáculo próximo nos três sensores |
| 1 | `OBSTACULO_ESQUERDA` | obstáculo predominante diante do sensor esquerdo |
| 2 | `OBSTACULO_CENTRO` | obstáculo predominante diante do sensor central |
| 3 | `OBSTACULO_DIREITA` | obstáculo predominante diante do sensor direito |

O sketch usa `CLASSE_COLETA` no topo. Faça uma sessão por classe e altere somente
essa constante entre sessões. A alteração deve aparecer em um commit ou no registro
da coleta.

## Procedimento reproduzível

1. Desligue a bateria dos motores; para a coleta, alimente o ESP32 por USB/power bank.
2. Confira GND comum, alimentação de 5 V e os três divisores de ECHO.
3. Abra [`coleta_dataset_3sensores.ino`](../experimentos/cp2-tinyml/firmware/coleta_dataset_3sensores.ino), ajuste `CLASSE_COLETA` e faça upload.
4. Abra o Serial Monitor em 115200 baud e salve a saída bruta da sessão, incluindo o cabeçalho.
5. Faça pelo menos quatro sessões separadas, uma por classe, mantendo o obstáculo parado por alguns segundos e variando levemente a distância entre as repetições.
6. Pare a gravação antes de mudar a classe e guarde o arquivo com nome explícito, por exemplo `classe-0-livre.txt`.
7. Preencha [`experimentos/cp2-tinyml/dados/REGISTRO_COLETA.md`](../experimentos/cp2-tinyml/dados/REGISTRO_COLETA.md).
8. Remova linhas de diagnóstico somente ao gerar o CSV final; nunca apague ou edite os TXT originais.

Para conferir o arquivo final sem alterá-lo, use:

```powershell
python experimentos/cp2-tinyml/ferramentas/validar_dataset.py caminho/para/dataset_cp2.csv
```

O validador verifica cabeçalho, quatro colunas, faixa de distância, labels e
contagem por classe. Ele não consegue provar que as linhas vieram do sensor; por
isso os TXT, fotos e registro da coleta continuam obrigatórios.

## Formato obrigatório

```csv
esquerda_cm,centro_cm,direita_cm,label
82.4,78.1,90.0,0
18.2,76.5,88.7,1
```

O CSV final deve conter as três distâncias em centímetros e um label inteiro de 0 a
3. A quantidade por classe deve ser informada no relatório; não declarar um número
antes de contar as linhas reais.

## Conferência antes do Edge Impulse

- [ ] cada linha tem quatro colunas;
- [ ] não existem `SEM_ECO`, vazios, `NaN` ou valores negativos;
- [ ] os quatro labels aparecem;
- [ ] as classes estão minimamente balanceadas;
- [ ] a sessão e a classe de cada grupo de linhas são rastreáveis;
- [ ] o TXT bruto e as condições da coleta foram preservados;
- [ ] o conjunto de teste não é apenas uma cópia de linhas consecutivas do treino.

## Registro de resultado

Preencher depois da coleta:

| Classe | Sessões | Amostras | Mínimo centro (cm) | Máximo centro (cm) | Observações |
|---|---:|---:|---:|---:|---|
| LIVRE | [PREENCHER] | [PREENCHER] | [PREENCHER] | [PREENCHER] | [PREENCHER] |
| OBSTACULO_ESQUERDA | [PREENCHER] | [PREENCHER] | [PREENCHER] | [PREENCHER] | [PREENCHER] |
| OBSTACULO_CENTRO | [PREENCHER] | [PREENCHER] | [PREENCHER] | [PREENCHER] | [PREENCHER] |
| OBSTACULO_DIREITA | [PREENCHER] | [PREENCHER] | [PREENCHER] | [PREENCHER] | [PREENCHER] |
