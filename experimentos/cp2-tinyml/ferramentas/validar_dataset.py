#!/usr/bin/env python3
"""Valida o CSV real do CP2 sem alterar o arquivo."""

import argparse
import csv
import math
from collections import Counter
from pathlib import Path

CABECALHO = ["esquerda_cm", "centro_cm", "direita_cm", "label"]
LABELS = {
    0: "LIVRE",
    1: "OBSTACULO_ESQUERDA",
    2: "OBSTACULO_CENTRO",
    3: "OBSTACULO_DIREITA",
}


def carregar(caminho: Path):
    with caminho.open(encoding="utf-8-sig", newline="") as arquivo:
        leitor = csv.reader(arquivo)
        try:
            cabecalho = next(leitor)
        except StopIteration as erro:
            raise ValueError("CSV vazio") from erro
        if cabecalho != CABECALHO:
            raise ValueError(f"cabeçalho esperado: {','.join(CABECALHO)}")

        linhas = []
        for numero, linha in enumerate(leitor, 2):
            if not linha or all(not celula.strip() for celula in linha):
                continue
            if len(linha) != 4:
                raise ValueError(f"linha {numero}: esperado 4 colunas")
            try:
                distancias = [float(valor) for valor in linha[:3]]
                label = int(linha[3])
            except ValueError as erro:
                raise ValueError(f"linha {numero}: número inválido") from erro
            if not all(math.isfinite(valor) and 0 < valor <= 400 for valor in distancias):
                raise ValueError(f"linha {numero}: distância fora de 0–400 cm")
            if label not in LABELS:
                raise ValueError(f"linha {numero}: label deve ser 0, 1, 2 ou 3")
            linhas.append((distancias, label))
    if not linhas:
        raise ValueError("nenhuma amostra válida")
    return linhas


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("csv", type=Path)
    args = parser.parse_args()
    try:
        linhas = carregar(args.csv)
    except (OSError, ValueError) as erro:
        parser.error(str(erro))

    contagem = Counter(label for _, label in linhas)
    print(f"Amostras válidas: {len(linhas)}")
    for label, nome in LABELS.items():
        print(f"{label} ({nome}): {contagem[label]}")
    faltantes = [str(label) for label in LABELS if not contagem[label]]
    if faltantes:
        print(f"ATENÇÃO: labels sem amostras: {', '.join(faltantes)}")
        return 1
    print("Formato, faixa e labels conferidos; origem física depende do registro da coleta.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
