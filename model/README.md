# Modelos TinyML

## `model.h`

`model.h` é um classificador local de referência, com regressão logística
multinomial e Softmax. Ele foi treinado com o arquivo `docs/dataset.csv`, que contém
dados **sintéticos** da Tarefa 21.

Ele é útil para validar a lógica de leitura, normalização, classificação e ação no
ESP32, mas **não é a biblioteca exportada pelo Edge Impulse** e não deve ser usado
como prova de coleta real no CP2.

## Biblioteca do CP2

A biblioteca gerada pelo projeto real do Edge Impulse deve ser copiada para
[`edge-impulse/`](edge-impulse/) depois do export. Junto dela, registrar:

- ZIP original ou nome do pacote;
- header principal;
- versão do export;
- projeto/Project ID;
- placa e versão do core ESP32 usados no upload.

O firmware CP2 foi escrito com uma função de inferência isolada para que a troca do
modelo local pelo header exportado seja rastreável. Enquanto o diretório estiver sem
a biblioteca real, a integração deve ser descrita como **baseline local preparada**.
