from globales import EstadoGlobal
import uproot
import pandas as pd
import tkinter as tk
from tkinter import ttk, filedialog
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import numpy as np

# Variable global para almacenar el frame donde se generan los widgets de los trees
# frame_widgets = None
# etiqueta_imagen = None  # Etiqueta para la imagen cargada
# canvas = None  # Para la gráfica en tkinter
estado = EstadoGlobal()  # Obtener la única instancia de la clase
boton_exportar_csv = None  # Variable global para controlar el botón de exportación
boton_visualizar = None
listbox_branches_actual = None  # Variable global para controlar el Listbox actual

# Función para cargar y procesar el archivo ROOT y actualizar los widgets
def cargar_root(ruta_root, ventana, etiqueta):
     # Si hay una imagen cargada, eliminarla pero restaurar el texto en el Label
    if estado.etiqueta_imagen:
        # Restablecer el texto y el tamaño original del Label
        etiqueta.config(text="Arrastra una imagen o archivo ROOT aquí", image='', width=63, height=25)
        estado.etiqueta_imagen = None  # Limpiar la referencia de la imagen

    if estado.frame_widgets:
        estado.frame_widgets.destroy()

    # Crear un nuevo frame para contener los widgets de este archivo ROOT
    estado.frame_widgets = ttk.Frame(ventana)
    ##Tamaño de todos los widgets
    estado.frame_widgets.place(width=320, height=700)

    print("Cargando archivo ROOT")
    tree_name = "Photons"

    # Abrir el archivo .root usando uproot
    with uproot.open(ruta_root) as file:
        # La información del classnames nos dará la clave (file.keys())
        for key, value in file.classnames().items():
            print(f"Nombre del objeto: {key}, Tipo del objeto: {value}")
        #Class names nos indicara si es un tree o no
            if value == "TTree":
                trees = file[key]
                nombre_tree = key[:-2]  # Eliminar el ";1" del nombre
                print(f"Tree encontrado: {nombre_tree}")
                typeT = 0

                # Crear un cuadro desplegable o botón para cada tree en la interfaz
                crear_widget_tree(estado.frame_widgets, nombre_tree, trees, etiqueta, typeT)
            elif value == "TH1D":
                hist = file[key]  # Aquí obtienes el histograma
                nombre_hist = key[:-2]  # Eliminar el ";1" del nombre
                print(f"Histograma TH1D encontrado: {nombre_hist}")
                typeT = 1
                crear_widget_tree(estado.frame_widgets, nombre_hist, hist, etiqueta, typeT)
                # Aquí podrías procesar el histograma
                #procesar_histograma(hist)
# Función que crea un cuadro desplegable o botón para cada tree
def crear_widget_tree(ventana, nombre_tree, trees, main_ventana, typeT):
    # Frame para contener el tree y las branches
    frame_tree = ttk.Frame(ventana)
    frame_tree.pack(fill="x", padx=5, pady=5)

    # Etiqueta para mostrar el nombre del tree
    etiqueta = ttk.Label(frame_tree, text=f"Tree: {nombre_tree}")
    etiqueta.pack(side="left", padx=5)

    # Crear un botón que mostrará las branches al hacer clic
    boton_branches = ttk.Button(frame_tree, text="Mostrar branches", command=lambda: mostrar_branches(trees, frame_tree, main_ventana, typeT))
    boton_branches.pack(side="left", padx=5)

# Función que muestra las branches de un tree y permite la selección múltiple
def mostrar_branches(trees, frame_tree, main_ventana, typeT):
    global boton_exportar_csv  # Usamos la variable global para el botón de exportar
    global listbox_branches_actual  # Usamos la variable global para el Listbox actual
    global boton_visualizar
    # Si ya existe un Listbox de branches (de un tree anterior), eliminarlo
    if listbox_branches_actual:
        listbox_branches_actual.destroy()
        listbox_branches_actual = None
        if boton_exportar_csv:
            boton_exportar_csv.destroy()
            boton_exportar_csv = None
        if estado.canvas:
            estado.canvas.get_tk_widget().destroy()
            estado.canvas = None
    if boton_visualizar:
            boton_visualizar.destroy()
            boton_visualizar = None

    # Obtener las branches (keys) del tree
    if typeT == 0:
        branches = trees.keys()
        print(f"Tree Branches: {branches}")
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
    elif typeT == 1:
        print("Histogram data")
        # Crear un botón para visualizar las branches seleccionadas
        boton_visualizar = ttk.Button(frame_tree, text="Visualizar", command=lambda: visualizar_hist(trees, main_ventana))
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

    # Obtener la primera branch seleccionada
    selected_indices = listbox_branches.curselection()
    if not selected_indices:
        print("No se ha seleccionado ninguna branch para visualizar.")
        return

    branch_seleccionada = listbox_branches.get(selected_indices[0])
    print(f"Visualizando branch: {branch_seleccionada}")

    # Extraer los datos de la branch seleccionada
    data = trees[branch_seleccionada].array(library="np")

    # Si ya existe una gráfica previa, destruirla
    if estado.canvas:
        estado.canvas.get_tk_widget().destroy()
        estado.canvas = None

    # Crear la figura para la gráfica con un tamaño personalizado
    fig, ax = plt.subplots(figsize=(1, 1))  # Ajusta el tamaño de la gráfica aquí (ancho, alto)

    # Crear el histograma
    ax.hist(data, bins=50, color='skyblue', edgecolor='black')  # Histograma con 50 bins ajustables
    ax.set_title(f"Histograma de {branch_seleccionada}")
    ax.set_xlabel("Valores")
    ax.set_ylabel("Frecuencia")

    # Mostrar la gráfica en la ventana de la etiqueta_imagen
    estado.canvas = FigureCanvasTkAgg(fig, master=etiqueta_imagen)
    estado.canvas.draw()

    # Configurar la posición del Canvas dentro del Label para que se ajuste correctamente
    canvas_widget = estado.canvas.get_tk_widget()
    canvas_widget.place(x=0, y=0, width=etiqueta_imagen.winfo_width(), height=etiqueta_imagen.winfo_height())

    # Si quieres que el canvas se ajuste automáticamente si la ventana cambia de tamaño:
    etiqueta_imagen.bind("<Configure>", lambda event: canvas_widget.place(x=0, y=0, width=event.width, height=event.height))

# Función para visualizar los datos de la branch seleccionada como histograma
def visualizar_hist(hist,  etiqueta_imagen):
    print("Procesando histograma")
     # Si ya existe una gráfica previa, destruirla
    if estado.canvas:
        estado.canvas.get_tk_widget().destroy()
        estado.canvas = None
    # Función para procesar y plotear el histograma
    # Extraer el contenido del histograma
    valores = hist.values()  # Los valores en cada bin
    bordes = hist.axis().edges()  # Los bordes de los bins

    # Crear la figura para la gráfica con un tamaño personalizado
    fig, ax = plt.subplots(figsize=(1, 1))  # Ajusta el tamaño de la gráfica aquí (ancho, alto)
    # Plotear el histograma
     # Crear el histograma
    ax.hist(bordes[:-1], bins=bordes, weights=valores, histtype='step', label=hist.name)
    ax.set_title(f"Histograma: {hist.name}")
    ax.set_xlabel("Valores")
    ax.set_ylabel("Frecuencia")
    ax.set_xlim(0, 200)  # limite_x debe ser una tupla (xmin, xmax)


    # Mostrar la gráfica en la ventana de la etiqueta_imagen
    estado.canvas = FigureCanvasTkAgg(fig, master=etiqueta_imagen)
    estado.canvas.draw()

    # Configurar la posición del Canvas dentro del Label para que se ajuste correctamente
    canvas_widget = estado.canvas.get_tk_widget()
    canvas_widget.place(x=0, y=0, width=etiqueta_imagen.winfo_width(), height=etiqueta_imagen.winfo_height())

    # Si quieres que el canvas se ajuste automáticamente si la ventana cambia de tamaño:
    etiqueta_imagen.bind("<Configure>", lambda event: canvas_widget.place(x=0, y=0, width=event.width, height=event.height))

    #
