from PIL import Image, ImageTk
from globales import EstadoGlobal
import uproot
import pandas as pd
import tkinter as tk
from tkinter import ttk, filedialog
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import numpy as np

# Globales
estado = EstadoGlobal()
# Función para manejar el clic del mouse
# Variables globales para las coordenadas del rectángulo de selección
start_x1, start_y1, start_x2, start_y2 = None, None, None, None
selection_frame1, selection_frame2 = None, None  # Frames de los dos rectángulos
image_np = None
rectangulo_activo = 1  # Indica qué rectángulo está siendo manipulado (1 o 2)
intensidad_rect1, intensidad_rect2 = None, None  # Para almacenar las intensidades de cada rectángulo
std_rect1, std_rect2 = None, None
cnr_values = []
ROInum = 0
# Variables globales para las etiquetas ROI1 y ROI2
label_roi1, label_roi2 = None, None
# Función para manejar el clic del mouse
def on_click(event):
    global start_x1, start_y1, start_x2, start_y2, selection_frame1, selection_frame2, rectangulo_activo

    if rectangulo_activo == 1:
        # Al comenzar la selección del primer rectángulo
        start_x1, start_y1 = event.x, event.y
        print(f"Inicio de la selección del rectángulo 1 en: ({start_x1}, {start_y1})")

        # Crear el Frame de selección (borde rojo) si no existe
        if not selection_frame1:
            selection_frame1 = tk.Frame(estado.etiqueta_imagen, bg='', highlightthickness=2, highlightbackground="red")
        selection_frame1.place(x=start_x1, y=start_y1, width=1, height=1)

    elif rectangulo_activo == 2:
        # Al comenzar la selección del segundo rectángulo
        start_x2, start_y2 = event.x, event.y
        print(f"Inicio de la selección del rectángulo 2 en: ({start_x2}, {start_y2})")

        # Crear el Frame de selección (borde azul) si no existe
        if not selection_frame2:
            selection_frame2 = tk.Frame(estado.etiqueta_imagen, bg='', highlightthickness=2, highlightbackground="blue")
        selection_frame2.place(x=start_x2, y=start_y2, width=1, height=1)

# Función para manejar el arrastre del mouse
def on_drag(event):
    global selection_frame1, selection_frame2, rectangulo_activo

    if rectangulo_activo == 1 and selection_frame1:
        # Calcular el ancho y alto basados en el arrastre
        width = event.x - start_x1
        height = event.y - start_y1
        # Ajustar el tamaño del Frame de selección
        selection_frame1.place(x=start_x1, y=start_y1, width=width, height=height)
        print(f"Arrastrando rectángulo 1 a: ({event.x}, {event.y}) con tamaño {width}x{height}")

    elif rectangulo_activo == 2 and selection_frame2:
        # Calcular el ancho y alto basados en el arrastre
        width = event.x - start_x2
        height = event.y - start_y2
        # Ajustar el tamaño del Frame de selección
        selection_frame2.place(x=start_x2, y=start_y2, width=width, height=height)
        print(f"Arrastrando rectángulo 2 a: ({event.x}, {event.y}) con tamaño {width}x{height}")

def on_release(event):
    global start_x1, start_y1, start_x2, start_y2, selection_frame1, selection_frame2
    global rectangulo_activo, intensidad_rect1, intensidad_rect2, std_rect1, std_rect2
    global label_roi1, label_roi2

    end_x, end_y = event.x, event.y

    if rectangulo_activo == 1:
        print(f"Fin de la selección del rectángulo 1 en: ({end_x}, {end_y})")
        seleccion_x1, seleccion_y1 = min(start_x1, end_x), min(start_y1, end_y)
        seleccion_x2, seleccion_y2 = max(start_x1, end_x), max(start_y1, end_y)

        # Crear el texto "ROI1" encima del rectángulo 1
        if not label_roi1:
            label_roi1 = tk.Label(estado.etiqueta_imagen, text="ROI1", bg="white", fg="red")
        label_roi1.place(x=seleccion_x1, y=seleccion_y1 - 20)  # Posición encima del cuadro

        # Recortar la región seleccionada de la imagen usando NumPy para el primer rectángulo
        seleccion1 = image_np[seleccion_y1:seleccion_y2, seleccion_x1:seleccion_x2]
        intensidad_rect1 = np.mean(seleccion1)
        std_rect1 = np.std(seleccion1)
        print(f"Promedio de intensidad de la región seleccionada 1: {intensidad_rect1} y std: {std_rect1}")

        # Cambiar a la selección del segundo rectángulo
        rectangulo_activo = 2

    elif rectangulo_activo == 2:
        print(f"Fin de la selección del rectángulo 2 en: ({end_x}, {end_y})")
        seleccion_x1, seleccion_y1 = min(start_x2, end_x), min(start_y2, end_y)
        seleccion_x2, seleccion_y2 = max(start_x2, end_x), max(start_y2, end_y)

        # Crear el texto "ROI2" encima del rectángulo 2
        if not label_roi2:
            label_roi2 = tk.Label(estado.etiqueta_imagen, text="ROI2", bg="white", fg="blue")
        label_roi2.place(x=seleccion_x1, y=seleccion_y1 - 20)  # Posición encima del cuadro

        # Recortar la región seleccionada de la imagen usando NumPy para el segundo rectángulo
        seleccion2 = image_np[seleccion_y1:seleccion_y2, seleccion_x1:seleccion_x2]
        intensidad_rect2 = np.mean(seleccion2)
        std_rect2 = np.std(seleccion2)
        print(f"Promedio de intensidad de la región seleccionada 2: {intensidad_rect2} y std: {std_rect2}")

        # Resetear a la selección del primer rectángulo
        rectangulo_activo = 1


def cargar_imagen(ruta_imagen, etiqueta, ventana):
    global image_np
    print("Cargando imagen")
    # Si hay widgets (de un archivo ROOT), eliminarlos antes de cargar la imagen
    if estado.frame_widgets:
        estado.frame_widgets.destroy()
        estado.frame_widgets = None  # Limpiar la referencia del frame de widgets
    if estado.canvas:
        estado.canvas.get_tk_widget().destroy()
        estado.canvas = None

    # Abre la imagen sin cambiar su tamaño
    imagen = Image.open(ruta_imagen)
    # Obtén el tamaño original de la imagen
    ancho_original, alto_original = imagen.size
    print(f"Tamaño original de la imagen: {ancho_original}x{alto_original}")
    # Definir el tamaño máximo (por ejemplo, 600x600 píxeles)
    max_ancho, max_alto = 550, 390
    # Calcula la relación de aspecto para redimensionar la imagen manteniendo la calidad
    ratio = min(max_ancho / ancho_original, max_alto / alto_original)
    # Solo redimensiona si la imagen es más grande que el tamaño máximo
    if ratio < 1:
        nuevo_ancho = int(ancho_original * ratio)
        nuevo_alto = int(alto_original * ratio)
        imagen = imagen.resize((nuevo_ancho, nuevo_alto), Image.ANTIALIAS)
        print(f"Imagen redimensionada a: {nuevo_ancho}x{nuevo_alto}")
    else:
        nuevo_ancho, nuevo_alto = ancho_original, alto_original

    # Convierte la imagen a un formato que tkinter pueda manejar
    imagen_tk = ImageTk.PhotoImage(imagen)
    
    # Ajusta el tamaño del Label al nuevo tamaño de la imagen
    etiqueta.config(image=imagen_tk, width=nuevo_ancho, height=nuevo_alto)
    etiqueta.image = imagen_tk  # Necesario para mantener la referencia
    estado.etiqueta_imagen = etiqueta  # Guardar la referencia de la etiqueta actual

    ##Crear widgets del nuevo frame
    estado.frame_widgets = ttk.Frame(ventana)
    estado.frame_widgets.place(width = 320, height = 700)
    ImageOptions = ["CNR", "CorteV", "CorteH", "DENOISE"]
    for name in ImageOptions:
        crear_widget_tree(estado.frame_widgets, name, etiqueta, ventana)
    imagen = imagen.convert("L") 
    # Convertir la imagen a matriz NumPy para trabajar con los valores de intensidad
    image_np = np.array(imagen)
    #etiqueta.bind("<Motion>", lambda event: mostrar_coordenadas(event, etiqueta, imagen))
    # Vincular los eventos de clic, arrastre y soltar al canvas
    estado.etiqueta_imagen.bind("<ButtonPress-1>", on_click)
    estado.etiqueta_imagen.bind("<B1-Motion>", on_drag)
    estado.etiqueta_imagen.bind("<ButtonRelease-1>", on_release)

def crear_widget_tree(ventana, nombre_tree, main_ventana, canvas):
    # Frame para contener el tree y las branches
    frame_tree = ttk.Frame(ventana)
    frame_tree.pack(fill="x", padx=5, pady=5)

    # Etiqueta para mostrar el nombre del tree
    etiqueta = ttk.Label(frame_tree, text=f"Option: {nombre_tree}")
    etiqueta.pack(side="left", padx=5)

    # Diccionario que asigna las opciones a funciones específicas
    funciones_opciones = {
        "CNR": lambda: funcion_cnr(canvas, intensidad_rect1, intensidad_rect2, std_rect2),
        "CorteV": lambda: funcion_cortev(main_ventana),
        "CorteH": lambda: funcion_corteh(main_ventana),
        "DENOISE": lambda: funcion_denoise(main_ventana)
    }
    #Obtener la función y los argumentos asociados a la opción seleccionada
    funcion = funciones_opciones[nombre_tree]
    # Crear un botón que ejecuta una función específica según el nombre_tree
    boton_branches = ttk.Button(frame_tree, text="Seleccionar región", command=funcion)
    boton_branches.pack(side="left", padx=5)

# Función para mostrar las coordenadas del cursor sobre la imagen
def mostrar_coordenadas(event, etiqueta, imagen):
    x, y = event.x, event.y
    if 0 <= x < imagen.width and 0 <= y < imagen.height:
        print(f"Coordenadas del cursor sobre la imagen: ({x}, {y})")

# # Función para ejecutar el cálculo de CNR
def funcion_cnr(main_ventana, signal_avg, background_avg, background_std):
    print(signal_avg, background_avg, background_std)
    if signal_avg and background_avg and background_std:
        cnr = (signal_avg-background_avg)/background_std
        print("CNR: "+str(cnr)+'\n')
   # Agregar el valor de CNR a la lista global
        cnr_values.append(cnr)

        # Actualizar el histograma
        actualizar_grafico_barras(main_ventana)
    else:
        print("Faltan datos para calcular el CNR")

def funcion_cortev(main_ventana):
    print("Ejecutando función Corte Vertical")
    # Aquí va la lógica específica de CorteV

def funcion_corteh(main_ventana):
    print("Ejecutando función Corte Horizontal")
    # Aquí va la lógica específica de CorteH

def funcion_denoise(main_ventana):
    print("Ejecutando DENOISE")

def actualizar_grafico_barras(main_ventana):
    global estado, cnr_values

    # Limpiar el canvas anterior si existe
    if estado.canvas:
        estado.canvas.get_tk_widget().destroy()
        estado.canvas = None

    # Crear una nueva figura y dibujar el gráfico de barras de los valores de CNR
    fig, ax = plt.subplots()

    # Crear una lista de etiquetas ROI para el eje X
    roi_labels = [f'ROI{i+1}' for i in range(len(cnr_values))]

    # Crear gráfico de barras
    ax.bar(roi_labels, cnr_values, color='blue', alpha=0.7)

    # Configuraciones del gráfico
    ax.set_title("Valores de CNR por ROI")
    ax.set_xlabel("ROI")
    ax.set_ylabel("CNR")

    # Añadir los valores de CNR encima de cada barra
    for i, cnr in enumerate(cnr_values):
        ax.text(i, cnr + 0.05, f'{cnr:.2f}', ha='center', va='bottom', fontsize=10, color='black')

    # Crear un nuevo canvas para mostrar la figura en Tkinter
    estado.canvas = FigureCanvasTkAgg(fig, master=main_ventana)
    estado.canvas.draw()
    estado.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)