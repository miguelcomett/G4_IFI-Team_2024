import os
import subprocess
import uproot
import pandas as pd
import matplotlib.pyplot as plt

# Configuración
sim_dir = "C:\\Users\\conej\\Documents\\Universidad\\Geant4\\Projects\\Git\\build\\Release"
# Carpeta actual desde donde se ejecuta el script
output_folder = os.getcwd()
exec_file = "PCM.exe"
mac_name = "angle.mac"
mac_template = """\
/run/initialize
/gun/energy {energy} keV
/Pgun/Angle {angle}
/run/beamOn 10000000
"""

# Función para crear y guardar el archivo .mac
def create_mac_file(energy, angle, mac_name):
	mac_file_path = os.path.join(sim_dir, mac_name)
	mac_content = mac_template.format(energy=energy, angle=angle)
	with open(mac_file_path, 'w') as f:
        	f.write(mac_content)
	return mac_file_path

# run_simulation
def run_Sim(executable_file, mac_filename, directory):
	run_sim = f".\{executable_file} {mac_filename}"
	try:
        	subprocess.run(run_sim, cwd=directory, check=True, shell=True)
	except subprocess.CalledProcessError as e: 
        	print(f"Error al ejecutar la simulación: {e}")

for i in range(0, 90, 10):
	create_mac_file(70, i, mac_name)
	run_Sim(exec_file, mac_name, sim_dir)
	# Obtener la lista de archivos .root en la carpeta
	root_files = [f for f in os.listdir(sim_dir) if f.endswith(".root")]

	if root_files != None:
		# Procesar cada archivo .root
		for root_file in root_files:
		    	# Ruta completa del archivo .root
			root_file_path = os.path.join(sim_dir, root_file)
		    
		    	# Abrir el archivo .root usando uproot
			with uproot.open(root_file_path) as file:
				# Acceder a la ntuple G4_PCM
				ntuple = file["G4_PCM"]
				
				# Extraer las ramas Energy, PositionX, PositionY, PositionZ
				data = ntuple.arrays(["Energy", "PositionX", "PositionY"], library="pd")
				# Convertir a DataFrame de pandas

				df = pd.DataFrame(data)
				df = df.round(4)
				# Redondear a 4 decimales

				# Nombre del archivo de salida .csv
				csv_file_name = os.path.splitext(root_file)[0] + str(i) + ".csv"
				csv_file_path = os.path.join(output_folder, csv_file_name)
				
				# Guardar el DataFrame en un archivo .csv en la carpeta actual
				df.to_csv(csv_file_path, index=False)

				print(f"Archivo convertido: {csv_file_name}")
	else:
		print("No root files detected")
	
	##Save images
	# Leer los datos del archivo Sim0.csv
	df = pd.read_csv('Sim0' + str(i) + '.csv')

	# Crear la figura y el eje 2D
	plt.figure(figsize=(8, 6))

	# Graficar los puntos en 2D
	sc = plt.scatter(df['PositionX'], df['PositionY'], c=df['Energy'], cmap='Spectral', s=1)

	# Etiquetas de los ejes
	plt.xlabel('PositionX')
	plt.ylabel('PositionY')
	plt.title("Radiography with " + str(i) + "° angle")
	# Cambiar los rangos de los ejes
	plt.xlim(-80, 80)  # Rango para el eje X
	plt.ylim(-80, 80)  # Rango para el eje Y

	# Añadir la barra de color para la energía
	cbar = plt.colorbar(sc)
	cbar.set_label('Energy')

	# Save the plot as an image file
	plt.savefig('plot_image_os' + str(i) + '.png')


	


