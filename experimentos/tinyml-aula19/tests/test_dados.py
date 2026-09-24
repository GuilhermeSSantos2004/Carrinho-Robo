"""Casos sintéticos de teste; não são dados coletados do sensor."""
import contextlib
import io
import importlib.util
from pathlib import Path
import tempfile
import unittest

MODULE = Path(__file__).resolve().parents[1] / "ferramentas/dados.py"
spec = importlib.util.spec_from_file_location("dados", MODULE)
dados = importlib.util.module_from_spec(spec)
spec.loader.exec_module(dados)


class DadosTests(unittest.TestCase):
    def ler_texto(self, texto, tipo="processado", serial=True):
        with tempfile.TemporaryDirectory() as pasta:
            path = Path(pasta) / "serial.txt"
            path.write_text(texto, encoding="utf-8")
            return dados.ler(path, tipo, serial)

    def test_importa_timestamp_e_comentarios(self):
        linhas, comentarios, boot = self.ler_texto(
            "rst:0x1\n12:30:10.123 -> feature,label\n"
            "12:30:10.223 -> # AQUECIMENTO\n12:30:10.323 -> 0.100000,0\n0.200000,1\n")
        self.assertEqual(len(linhas), 2)
        self.assertEqual((comentarios, boot), (1, 1))

    def test_rejeita_dados_corrompidos(self):
        for linha in ["nan,0", "inf,1", "1.1,1", "0.2,0", "0.1,2", "0,1,0"]:
            with self.subTest(linha=linha), self.assertRaises(ValueError):
                self.ler_texto(linha)

    def test_nao_inventa_csv_vazio(self):
        with self.assertRaises(ValueError):
            self.ler_texto("feature,label\n# SEM_ECO\n")

    def test_bruto_preserva_timeout_e_fora_faixa(self):
        linhas, _, _ = self.ler_texto(
            "100,0,,SEM_ECO\n200,1000,17.150000,OK\n300,25000,428.750000,FORA_FAIXA\n", "bruto")
        self.assertEqual(len(linhas), 3)
        with self.assertRaises(ValueError):
            self.ler_texto("100,0,0,OK", "bruto")

    def test_arquivo_final_estrito(self):
        for texto in ["feature,label\n# comentario\n0.1,0", "0.1,0", "feature,label\nfeature,label\n0.1,0"]:
            with self.subTest(texto=texto), self.assertRaises(ValueError):
                self.ler_texto(texto, serial=False)

    def test_importacao_preserva_arquivo_existente(self):
        with tempfile.TemporaryDirectory() as pasta:
            entrada = Path(pasta) / "serial.txt"
            saida = Path(pasta) / "dataset.csv"
            entrada.write_text("feature,label\n0.100000,0\n0.300000,1\n")
            args = ["importar", str(entrada), "--tipo", "processado", "--saida", str(saida)]
            with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(dados.main(args), 0)
                original = saida.read_bytes()
                self.assertEqual(dados.main(args), 1)
                self.assertEqual(saida.read_bytes(), original)
            linhas, _, _ = dados.ler(saida, "processado")
            self.assertEqual(len(linhas), 2)


if __name__ == "__main__":
    unittest.main()
