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
def cargar_imagen(ruta_imagen, etiqueta, ventana):
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
        crear_widget_tree(estado.frame_widgets, name, etiqueta)

def crear_widget_tree(ventana, nombre_tree, main_ventana):
    # Frame para contener el tree y las branches
    frame_tree = ttk.Frame(ventana)
    frame_tree.pack(fill="x", padx=5, pady=5)

    # Etiqueta para mostrar el nombre del tree
    etiqueta = ttk.Label(frame_tree, text=f"Option: {nombre_tree}")
    etiqueta.pack(side="left", padx=5)

    # Diccionario que asigna las opciones a funciones específicas
    funciones_opciones = {
        "CNR": funcion_cnr,
        "CorteV": funcion_cortev,
        "CorteH": funcion_corteh,
        "DENOISE" : funcion_denoise
    }

    # Crear un botón que ejecuta una función específica según el nombre_tree
    boton_branches = ttk.Button(frame_tree, text="Seleccionar región", command=lambda: funciones_opciones[nombre_tree](main_ventana))
    boton_branches.pack(side="left", padx=5)

# Funciones específicas para cada opción
def funcion_cnr(main_ventana):
    print("Ejecutando función CNR")
    # Aquí va la lógica específica de CNR

def funcion_cortev(main_ventana):
    print("Ejecutando función Corte Vertical")
    # Aquí va la lógica específica de CorteV

def funcion_corteh(main_ventana):
    print("Ejecutando función Corte Horizontal")
    # Aquí va la lógica específica de CorteH

def funcion_denoise(main_ventana):
    print("Ejecutando DENOISE")