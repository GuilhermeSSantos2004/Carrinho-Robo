#!/usr/bin/env python3
"""Importa Serial Monitor e analisa medições. Somente biblioteca padrão Python."""
import argparse
import csv
import math
from pathlib import Path
import re
import statistics
import sys

CABECALHOS = {
    "bruto": ["tempo_ms", "duracao_us", "distancia_cm", "status"],
    "processado": ["feature", "label"],
}


def validar(linha, tipo):
    if len(linha) != len(CABECALHOS[tipo]):
        raise ValueError("quantidade incorreta de colunas")
    if tipo == "processado":
        feature = float(linha[0])
        if not math.isfinite(feature) or not 0 <= feature <= 1:
            raise ValueError("feature deve ser finita entre 0 e 1")
        if linha[1] not in ("0", "1"):
            raise ValueError("label deve ser 0 ou 1")
        if int(linha[1]) != (0 if feature < 0.2 else 1):
            raise ValueError("label inconsistente com limiar 0.2")
        return
    tempo, pulso = int(linha[0]), int(linha[1])
    if tempo < 0 or pulso < 0:
        raise ValueError("tempo e pulso devem ser não negativos")
    if linha[3] == "SEM_ECO":
        if pulso != 0 or linha[2] != "":
            raise ValueError("SEM_ECO exige pulso 0 e distância vazia")
        return
    cm = float(linha[2])
    if pulso == 0 or not math.isfinite(cm) or cm <= 0:
        raise ValueError("distância ou pulso inválidos")
    esperado = "OK" if 2 <= cm <= 400 else "FORA_FAIXA"
    if linha[3] != esperado:
        raise ValueError("status incompatível com distância")
    if not math.isclose(cm, pulso * 0.0343 / 2, abs_tol=0.001):
        raise ValueError("distância incompatível com pulso")


def ler(caminho, tipo, serial=False):
    linhas, comentarios, ignoradas = [], 0, 0
    cabecalho_visto = False
    with Path(caminho).open(encoding="utf-8-sig") as arquivo:
        for numero, original in enumerate(arquivo, 1):
            texto = original.strip()
            if serial:
                texto = re.sub(r"^\d{1,2}:\d{2}:\d{2}(?:\.\d+)?\s*->\s*", "", texto)
            if not texto:
                continue
            if texto.startswith("#"):
                if not serial:
                    raise ValueError(f"linha {numero}: comentário não pertence ao CSV final")
                comentarios += 1
                continue
            linha = next(csv.reader([texto]))
            if not serial and not cabecalho_visto and linha != CABECALHOS[tipo]:
                raise ValueError(f"linha {numero}: cabeçalho obrigatório ausente ou incorreto")
            if linha == CABECALHOS[tipo]:
                if not serial and cabecalho_visto:
                    raise ValueError(f"linha {numero}: cabeçalho repetido no CSV final")
                cabecalho_visto = True
                continue
            try:
                validar(linha, tipo)
            except (ValueError, OverflowError) as erro:
                # Ignorar boot do ESP32 só na importação; nunca esconder linhas numéricas ruins.
                if serial and not re.match(r"^[+\-\d.]", texto) and "," not in texto:
                    ignoradas += 1
                    continue
                raise ValueError(f"linha {numero}: {erro}: {texto}") from erro
            linhas.append(linha)
    if not linhas:
        raise ValueError("nenhuma medição válida encontrada; arquivo vazio não é uma coleta")
    return linhas, comentarios, ignoradas


def resumo(linhas, tipo):
    print(f"Registros: {len(linhas)}")
    if tipo == "processado":
        features = [float(x[0]) for x in linhas]
        for label, nome in (("0", "perto"), ("1", "longe")):
            n = sum(x[1] == label for x in linhas)
            print(f"Classe {label} ({nome}): {n}")
            if n == 0:
                print("ATENÇÃO: falta coletar esta classe.")
        print(f"Feature: mínimo={min(features):.6f}; máximo={max(features):.6f}")
        print("Consistência numérica verificada; origem física depende do registro da coleta.")
        return
    for status in ("OK", "SEM_ECO", "FORA_FAIXA"):
        print(f"{status}: {sum(x[3] == status for x in linhas)}")
    valores = [float(x[2]) for x in linhas if x[3] == "OK"]
    if not valores:
        raise ValueError("coleta sem distâncias dentro da faixa do sensor")
    print(f"Mínimo observado válido: {min(valores):.6f} cm")
    print(f"Máximo observado válido: {max(valores):.6f} cm")
    print(f"Média: {statistics.mean(valores):.6f} cm")
    print(f"Amplitude: {max(valores)-min(valores):.6f} cm")
    if len(valores) > 1:
        print(f"Desvio padrão amostral: {statistics.stdev(valores):.6f} cm")
    print("Avalie ruído/estabilidade apenas em coleta com alvo parado; movimento altera estas estatísticas.")


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("acao", choices=["importar", "analisar"])
    parser.add_argument("entrada", type=Path)
    parser.add_argument("--tipo", choices=CABECALHOS, required=True)
    parser.add_argument("--saida", type=Path, help="CSV novo; obrigatório ao importar")
    args = parser.parse_args(argv)
    if args.acao == "importar" and args.saida is None:
        parser.error("importar exige --saida")
    if args.acao == "analisar" and args.saida is not None:
        parser.error("--saida só é usado com importar")
    try:
        linhas, comentarios, ignoradas = ler(args.entrada, args.tipo, args.acao == "importar")
        resumo(linhas, args.tipo)
        if args.acao == "importar":
            # Escrita exclusiva protege uma coleta já salva.
            with args.saida.open("x", encoding="utf-8", newline="") as arquivo:
                writer = csv.writer(arquivo)
                writer.writerow(CABECALHOS[args.tipo])
                writer.writerows(linhas)
            print(f"Salvo: {args.saida}")
            print(f"Comentários removidos: {comentarios}; linhas de boot ignoradas: {ignoradas}")
        return 0
    except (OSError, ValueError, csv.Error) as erro:
        print(f"ERRO: {erro}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
