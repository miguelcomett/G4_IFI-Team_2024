import uproot
import pandas as pd
import tkinter as tk
from tkinter import ttk, filedialog
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import numpy as np

# Variable global para almacenar el frame donde se generan los widgets de los trees
frame_widgets = None
boton_exportar_csv = None  # Variable global para controlar el botón de exportación
boton_visualizar = None
listbox_branches_actual = None  # Variable global para controlar el Listbox actual
canvas = None  # Para la gráfica en tkinter
etiqueta_imagen = None  # Etiqueta para la imagen cargada

# Función para cargar y procesar el archivo ROOT y actualizar los widgets
def cargar_root(ruta_root, ventana, etiqueta):
    global frame_widgets
    global etiqueta_imagen
    
    # Si hay una imagen mostrada, eliminarla antes de cargar el archivo ROOT
    if etiqueta_imagen:
        etiqueta_imagen.destroy()
        etiqueta_imagen = None

    # Si ya hay widgets creados (de un archivo previo), eliminarlos
    if frame_widgets:
        frame_widgets.destroy()

    # Crear un nuevo frame para contener los widgets de este archivo ROOT
    frame_widgets = ttk.Frame(ventana)
    frame_widgets.place(width=320, height=700)

    print("Cargando archivo ROOT")
    tree_name = "Photons"

    # Abrir el archivo .root usando uproot
    with uproot.open(ruta_root) as file:
        # La información del classnames nos dará la clave (file.keys())
        for key, value in file.classnames().items():
        #Class names nos indicara si es un tree o no
            if value == "TTree":
                trees = file[key]
                nombre_tree = key[:-2]  # Eliminar el ";1" del nombre
                print(f"Tree encontrado: {nombre_tree}")

                # Crear un cuadro desplegable o botón para cada tree en la interfaz
                crear_widget_tree(frame_widgets, nombre_tree, trees, etiqueta)

# Función que crea un cuadro desplegable o botón para cada tree
def crear_widget_tree(ventana, nombre_tree, trees, main_ventana):
    # Frame para contener el tree y las branches
    frame_tree = ttk.Frame(ventana)
    frame_tree.pack(fill="x", padx=5, pady=5)

    # Etiqueta para mostrar el nombre del tree
    etiqueta = ttk.Label(frame_tree, text=f"Tree: {nombre_tree}")
    etiqueta.pack(side="left", padx=5)

    # Crear un botón que mostrará las branches al hacer clic
    boton_branches = ttk.Button(frame_tree, text="Mostrar branches", command=lambda: mostrar_branches(trees, frame_tree, main_ventana))
    boton_branches.pack(side="left", padx=5)

# Función que muestra las branches de un tree y permite la selección múltiple
def mostrar_branches(trees, frame_tree, main_ventana):
    global boton_exportar_csv  # Usamos la variable global para el botón de exportar
    global listbox_branches_actual  # Usamos la variable global para el Listbox actual
    global canvas  # Para la gráfica
    global boton_visualizar
    # Si ya existe un Listbox de branches (de un tree anterior), eliminarlo
    if listbox_branches_actual:
        listbox_branches_actual.destroy()
        if boton_exportar_csv:
            boton_exportar_csv.destroy()
        if canvas:
            canvas.get_tk_widget().destroy()
    if boton_visualizar:
            boton_visualizar.destroy()

    # Obtener las branches (keys) del tree
    branches = trees.keys()
    print(f"Branches: {branches}")

    # Crear un Listbox para seleccionar múltiples branches
    listbox_branches_actual = tk.Listbox(frame_tree, selectmode=tk.MULTIPLE, height=5)
    listbox_branches_actual.pack(side="top", padx=5, pady=5)

    # Insertar las branches en el Listbox
    for branch in branches:
        listbox_branches_actual.insert(tk.END, branch)

    # Crear un botón para exportar las branches seleccionadas a CSV
    boton_exportar_csv = ttk.Button(frame_tree, text="Exportar CSV", command=lambda: exportar_csv(trees, listbox_branches_actual))
    boton_exportar_csv.pack(side="top", padx=5)

    # Crear un botón para visualizar las branches seleccionadas
    boton_visualizar = ttk.Button(frame_tree, text="Visualizar", command=lambda: visualizar_branch(trees, listbox_branches_actual, main_ventana))
    boton_visualizar.pack(side="top", padx=5, pady=5)

# Función para exportar las branches seleccionadas a un archivo CSV
def exportar_csv(trees, listbox_branches):
    # Obtener las branches seleccionadas
    selected_indices = listbox_branches.curselection()
    selected_branches = [listbox_branches.get(i) for i in selected_indices]

    if not selected_branches:
        print("No se han seleccionado branches.")
        return

    print(f"Branches seleccionadas: {selected_branches}")

    # Extraer las ramas seleccionadas desde el tree usando uproot
    try:
        # Convertir a DataFrame utilizando las branches seleccionadas
        # Verificar que efectivamente existen datos en las branches
        data = {}
        for branch in selected_branches:
            branch_data = trees[branch].array(library="np")
            if branch_data is not None and len(branch_data) > 0:
                data[branch] = branch_data
            else:
                print(f"Branch '{branch}' no contiene datos o está vacía.")
        
        if not data:
            print("No se encontraron datos para las branches seleccionadas.")
            return

        df = pd.DataFrame(data)

        # Abrir diálogo para guardar el archivo CSV
        file_path = filedialog.asksaveasfilename(defaultextension=".csv", filetypes=[("CSV files", "*.csv")])

        if file_path:
            df.to_csv(file_path, index=False)
            print(f"Branches exportadas a {file_path}")
        else:
            print("Exportación cancelada.")
    except Exception as e:
        print(f"Error al exportar branches: {e}")

# Función para visualizar los datos de la branch seleccionada como histograma
def visualizar_branch(trees, listbox_branches, etiqueta_imagen):
    global canvas  # Para la gráfica

    # Obtener la primera branch seleccionada
    selected_indices = listbox_branches.curselection()
    if not selected_indices:
        print("No se ha seleccionado ninguna branch para visualizar.")
        return

    branch_seleccionada = listbox_branches.get(selected_indices[0])
    print(f"Visualizando branch: {branch_seleccionada}")

    # Extraer los datos de la branch seleccionada
    data = trees[branch_seleccionada].array(library="np")

    # Crear la figura para la gráfica con un tamaño personalizado
    fig, ax = plt.subplots(figsize=(12, 6))  # Ajusta el tamaño de la gráfica aquí (ancho, alto)
    
    # Crear el histograma
    ax.hist(data, bins=50, color='skyblue', edgecolor='black')  # Histograma con 50 bins ajustables
    ax.set_title(f"Histograma de {branch_seleccionada}")
    ax.set_xlabel("Valores")
    ax.set_ylabel("Frecuencia")

    # Si ya existe una gráfica previa, destruirla
    if canvas:
        canvas.get_tk_widget().destroy()

    # Mostrar la gráfica en la ventana de la etiqueta_imagen
    canvas = FigureCanvasTkAgg(fig, master=etiqueta_imagen)
    canvas.draw()

    # Configurar la posición del Canvas dentro del Label para que se ajuste correctamente
    canvas_widget = canvas.get_tk_widget()
    canvas_widget.place(x=0, y=0, width=etiqueta_imagen.winfo_width(), height=etiqueta_imagen.winfo_height())

    # Si quieres que el canvas se ajuste automáticamente si la ventana cambia de tamaño:
    etiqueta_imagen.bind("<Configure>", lambda event: canvas_widget.place(x=0, y=0, width=event.width, height=event.height))
