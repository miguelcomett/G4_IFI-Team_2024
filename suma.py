import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv("s.csv")
energias = df["fSp"]
     # Elimina espacios o saltos de línea

# Definir los bins para la agrupación: desde 0 hasta 150 keV, con tamaño de bin de 1 keV
bins = np.arange(0, 151, 2)  # 150 bins, de 0 a 150 keV

# Crear el histograma
conteos, edges = np.histogram(energias, bins=bins)

# Normalizar los conteos
conteos_normalizados = conteos / np.sum(conteos)

# Guardar los datos en un archivo .txt
with open("fSpectrum.txt", "w") as f:
    for i in range(len(conteos_normalizados)):
        energia_central = (edges[i] + edges[i+1]) / 2  # Centro del bin
        f.write(f"{energia_central}\t{conteos_normalizados[i]}\n")

# Graficar el espectro
plt.step(edges[:-1], conteos_normalizados, where='mid')
plt.xlabel("Energía (keV)")
plt.ylabel("Intensidad normalizada")
plt.title("Espectro de rayos X")
plt.show()