import os
import re

def extrair_numeros(nome_arquivo):
    # Procura pelo padrão "row-{numero}-column-{numero}"
    match = re.search(r'row-(\d+)-column-(\d+)', nome_arquivo)
    if match:
        # Retorna uma tupla (linha, coluna) como inteiros para ordenação correta
        return int(match.group(1)), int(match.group(2))
    return 0, 0

def renomear_tiles():
    # Pega apenas arquivos png
    arquivos = [f for f in os.listdir() if f.endswith('.png')]
    
    # ORDENAÇÃO MÁGICA:
    # Ordena primeiro pelo número da linha, depois pelo número da coluna.
    # Isso garante que row-1-column-10 venha depois de row-1-column-9
    arquivos.sort(key=extrair_numeros)
    
    # Renomeia
    for i, arquivo_antigo in enumerate(arquivos):
        # Cria o novo nome: 001.png, 002.png (o :03d garante os 3 dígitos com zeros à esquerda)
        novo_nome = f"{i+1:03d}.png"
        
        print(f"Renomeando: {arquivo_antigo} -> {novo_nome}")
        os.rename(arquivo_antigo, novo_nome)

if __name__ == "__main__":
    # DICA DE SEGURANÇA: Faça um backup da pasta antes de rodar!
    renomear_tiles()