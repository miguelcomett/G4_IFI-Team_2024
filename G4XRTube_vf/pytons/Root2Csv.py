import uproot
import pandas as pd
import os

# Ruta de la carpeta que contiene los archivos .root
folder_path = "/home/alosnamon/G4_IFI-Team_2024-RealTube/G4XRTube_vf/build"

# Obtener la lista de archivos .root en la carpeta
root_files = [f for f in os.listdir(folder_path) if f.endswith(".root")]

# Carpeta actual desde donde se ejecuta el script
output_folder = os.getcwd()

if root_files != None:
	# Procesar cada archivo .root
	for root_file in root_files:
	    	# Ruta completa del archivo .root
		root_file_path = os.path.join(folder_path, root_file)
	    
	    	# Abrir el archivo .root usando uproot
		with uproot.open(root_file_path) as file:
			# Acceder a la ntuple G4_PCM
			ntuple = file["spectrum"]
			
			# Extraer las ramas Energy, PositionX, PositionY, PositionZ
			data = ntuple.arrays(["fSpectrum"], library="pd")
			
			# Convertir a DataFrame de pandas
			df = pd.DataFrame(data)
			
			# Nombre del archivo de salida .csv
			csv_file_name = os.path.splitext(root_file)[0] + ".csv"
			csv_file_path = os.path.join(output_folder, csv_file_name)
			
			# Guardar el DataFrame en un archivo .csv en la carpeta actual
			df.to_csv(csv_file_path, index=False)

			print(f"Archivo convertido: {csv_file_name}")
else:
	print("No root files detected")
