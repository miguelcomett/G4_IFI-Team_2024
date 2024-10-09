import tkinter as tk
from tkinterdnd2 import DND_FILES, TkinterDnD
from tkinter import Label
from PIL import Image, ImageTk
import os
from procesadores.procesar_imagenes import cargar_imagen
from procesadores.procesar_root import cargar_root

# Función para manejar el evento de arrastrar y soltar la imagen
def on_drop(event, etiqueta, ventana):
    ruta_archivo = event.data
    ruta_archivo = ruta_archivo.replace("{", "").replace("}", "")
    extension = os.path.splitext(ruta_archivo)[1].lower()  # Obtener la extensión del archivo  # Eliminar los símbolos {}
    print(extension)
    if extension in [".png", ".jpg", ".jpeg"]:  # Si es una imagen
        cargar_imagen(ruta_archivo, etiqueta, ventana)
    elif extension == ".root":  # Si es un archivo ROOT
        cargar_root(ruta_archivo, ventana)
    else:
        tk.messagebox.showerror("Error", "Tipo de archivo no soportado.")

def crear_interfaz():
    ventana = TkinterDnD.Tk()
    ventana.title("Interfaz de Procesamiento de Imágenes y Archivos ROOT")
    ventana.geometry("1100x650")

    etiqueta_imagen = tk.Label(ventana, text="Arrastra una imagen o archivo ROOT aquí", width=50, height=25, bg="lightgray")
    etiqueta_imagen.pack(padx=10, pady=10)

    ventana.drop_target_register(DND_FILES)
    ventana.dnd_bind('<<Drop>>', lambda event: on_drop(event, etiqueta_imagen, ventana))  # Pasar la etiqueta a on_drop

    ventana.mainloop()
