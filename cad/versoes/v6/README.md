# T03 v6 — suporte aberto recebido

Referência direta da integração v7. STL original preservado, peça única, sem tampa. Fonte CadQuery e STEP desta revisão não acompanharam o STL recebido. Os parâmetros e o relatório foram informados pelo usuário no chat; a tabela abaixo transcreve as dimensões relevantes.

| Parâmetro | mm | Origem |
|---|---:|---|
| Carcaça amarela, Y | 22,0 | medida confirmada pelo usuário |
| Alojamento interno | 22,4 | folga nominal de 0,2 por lado |
| Comprimento, Z | 70 | anúncio |
| Profundidade, X | 18,5 | presumida na v6 |
| Eixo | 5,35 | anúncio |
| Eixo livre | 10 | presumido |
| Centro do eixo sobre fundo da caixa | 9,5 | presumido |
| Base | 7 | projeto |
| Paredes | 5,5 | projeto |
| Altura de paredes sobre a base | 72 | projeto |
| Envelope total STL | 26,5 × 33,4 × 79 | geometria |
| Janela traseira inferior | 16 × 16 | projeto |
| Janela superior | 12 de largura, Z=38…72 | projeto |
| Rasgo inferior | X=5,5…23 | relatório recebido |

O usuário pediu outra extensão de 4 mm no rasgo. Segundo o relatório recebido só restavam 3,5 mm até a parede traseira; a alteração parou em X=5,5 para não cortar essa parede.

**Limitação:** o motor entra por +X e a v6 isolada não tem retenção positiva nessa direção. O atrito lateral não garante que ele fique preso durante o movimento. Fios e conectores não devem segurar o motor.

Na v7 a cavidade foi preservada, foi adicionada uma orelha estrutural para o chassi e a carroceria recebeu batentes. Use quatro `T03_suporte_v6_integrado.stl` da pasta v7 na montagem. O STL desta pasta não tem essa orelha.

O volume informado da v6 é cerca de 30.906 mm³ (~38,3 g se PLA maciço). Essa massa é somente do suporte isolado e não é consumo do fatiador. Não existe furo estrutural novo neste STL original; os furos novos da integração usam 3,46 mm.
