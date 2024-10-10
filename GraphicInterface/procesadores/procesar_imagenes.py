from PIL import Image, ImageTk
import tkinter as tk
from tkinter import ttk, filedialog
# frame_widgets = None
def cargar_imagen(ruta_imagen, etiqueta, ventana):
    # global frame_widgets
    # #Eliminar widgets previos
    # if frame_widgets:
    #     frame_widgets.destroy()

    # frame_widgets = ttk.Frame(ventana)
    # frame_widgets.pack(fill = "x", padx = 5, pady = 5)

    print("Cargando imagen")
    
    # Abre la imagen sin cambiar su tamaño
    imagen = Image.open(ruta_imagen)
    # Obtén el tamaño original de la imagen
    ancho_original, alto_original = imagen.size
    print(f"Tamaño original de la imagen: {ancho_original}x{alto_original}")
    # Definir el tamaño máximo (por ejemplo, 600x600 píxeles)
    max_ancho, max_alto = 550, 380
    # Calcula la relación de aspecto para redimensionar la imagen manteniendo la calidad
    ratio = min(max_ancho / ancho_original, max_alto / alto_original)
    # Solo redimensiona si la imagen es más grande que el tamaño máximo
    if ratio < 1:
        nuevo_ancho = int(ancho_original * ratio)
        nuevo_alto = int(alto_original * ratio)
        imagen = imagen.resize((nuevo_ancho, nuevo_alto), Image.ANTIALIAS)
        print(f"Imagen redimensionada a: {nuevo_ancho}x{nuevo_alto}")
    else:
        # Si la imagen ya es más pequeña que el tamaño máximo, se mantiene su tamaño original
        nuevo_ancho, nuevo_alto = ancho_original, alto_original
    # Convierte la imagen a un formato que tkinter pueda manejar
    imagen_tk = ImageTk.PhotoImage(imagen)
    etiqueta_imagen = etiqueta
    # Ajusta el tamaño del Label al nuevo tamaño de la imagen
    etiqueta_imagen.config(image=imagen_tk, width=nuevo_ancho, height=nuevo_alto)
    etiqueta_imagen.image = imagen_tk  # Necesario para mantener la referencia