import uproot
import os
import numpy as np
import dask.array as da
import dask.dataframe as dd
import pandas as pd
import plotly.graph_objects as go
import plotly.io as pio
import matplotlib.pyplot as plt
from PIL import Image
from scipy.ndimage import gaussian_filter

def heatmap_array_dask(dataframe, x_branch, y_branch, size, num, save_as):

    x_data = dataframe[x_branch].to_dask_array(lengths=True).compute()
    y_data = dataframe[y_branch].to_dask_array(lengths=True).compute()

    set_bins = np.arange(-size, size + 1, size/num)
    heatmap, x_edges, y_edges = np.histogram2d(x_data, y_data, bins = [set_bins, set_bins])
    heatmap = heatmap.T
    # print(heatmap.shape)

    row = len(set_bins) // 2
    normal_map = 1 - heatmap / np.max(heatmap)
    # plt.plot(normal_map[:][row])
    maxi = np.max(normal_map[:5])
    maxi = maxi * 1.2
    print('altura de ruido:', round(maxi, 4))
    normal_map[normal_map < maxi] = 0

    # plt.figure(figsize=(10, 4))
    # plt.subplot(1, 2, 1)
    # plt.imshow(normal_map, cmap='gray', extent = [x_edges[0], x_edges[-1], y_edges[0], y_edges[-1]])
    # plt.subplot(1, 2, 2)
    # plt.plot(normal_map[:][row])
    
    plt.imshow(normal_map, cmap='gray', extent = [x_edges[0], x_edges[-1], y_edges[0], y_edges[-1]])
    plt.savefig(save_as, dpi = 900)

    return normal_map, x_edges, y_edges

def root_to_dask(directory, root_name_starts, tree_name, x_branch, y_branch, decimal_places):
    
    file_name = os.path.join(directory, root_name_starts + ".root")

    with uproot.open(file_name) as root_file:
        tree = root_file[tree_name]
        if tree is None:
            print(f"Tree '{tree_name}' not found in {file_name}")
            return

        x_values = tree[x_branch].array(library="np") if x_branch in tree else None
        y_values = tree[y_branch].array(library="np") if y_branch in tree else None

        if x_values is not None:
            x_values = np.round(x_values, decimal_places)
        if y_values is not None:
            y_values = np.round(y_values, decimal_places)

        if x_values is None or y_values is None:
            print(f"Could not retrieve data for branches {x_branch} or {y_branch}")
            return

        x_dask_array = da.from_array(x_values, chunks="auto")
        y_dask_array = da.from_array(y_values, chunks="auto")

        dask_df = dd.from_dask_array(da.stack([x_dask_array, y_dask_array], axis=1), columns=[x_branch, y_branch])
        
        return dask_df



def heatmap2(array, xlim, ylim, title, x_label, y_label, width, height, save_as):

    fig = go.Figure(go.Heatmap(
                    z = array,
                    x = xlim,
                    y = ylim,
                    colorscale = [[0, 'black'], [1, 'white']],  # Grayscale from black to white
                    colorbar = dict(title = "Density", tickfont = dict(family = 'Merriweather', size = 16, color = 'Black'))))
    
    font_family = 'Merriweather'
    font_small  = 16
    font_medium = 20
    font_large  = 18
    
    fig.update_layout(
                    title = dict(text = title, font = dict(family = font_family, size = font_large, color = "Black"), 
                                 x = 0.51, y = 0.93, yanchor = 'middle', xanchor = 'center'),
                    xaxis_title = dict(text = x_label, font = dict(family = font_family, size = font_medium, color = "Black")),
                    yaxis_title = dict(text = y_label, font = dict(family = font_family, size = font_medium, color = "Black")),
                    xaxis = dict(tickfont = dict(family = font_family, size = font_small, color = "Black"), title_standoff = 25),
                    yaxis = dict(tickfont = dict(family = font_family, size = font_small, color = "Black"), title_standoff = 10),
                    width  = width,
                    height = height,
                    margin = dict(l = 105, r = 90, t = 90, b = 90)
    )
    
    pio.write_image(fig, save_as, width = width, height = height, scale = 5)
    fig.show()

#Function to crop the image 
def CropImages(Im1H, Im2L, left, upper, right, lower, saveNames):
    # Cargar las dos imágenes
    imagen1 = Image.open(Im1H)
    imagen2 = Image.open(Im2L)

    # Definir la región para cortar (left, upper, right, lower)
    region = (left, upper, right, lower)

    # Recortar las imágenes
    imagen1_recortada = imagen1.crop(region)
    imagen2_recortada = imagen2.crop(region)

    # Guardar o mostrar las imágenes recortadas
    imagen1_recortada.save(saveNames[0])
    imagen2_recortada.save(saveNames[1])

##Image Processing methods

def sls(low_energy_img, high_energy_img, wa, wb):
    # Convertir imágenes a float para evitar errores de precisión
    low_energy_img = np.float64(low_energy_img)
    high_energy_img = np.float64(high_energy_img)

    # Aplicar la sustracción ponderada
    tissue_img = -wb * low_energy_img + high_energy_img
    bone_img = wa * low_energy_img - high_energy_img

    return tissue_img, bone_img

def ssh(low_energy_img, high_energy_img, wa, wb, sigma):
    # Aplicar un filtro gaussiano a la imagen de alta energía
    high_energy_img_smooth = gaussian_filter(high_energy_img, sigma=sigma)

    # Aplicar la sustracción ponderada
    tissue_img = -wb * low_energy_img + high_energy_img_smooth
    bone_img = wa * low_energy_img - high_energy_img_smooth

    return tissue_img, bone_img

def acnr(low_energy_img, high_energy_img, wa , wb, sigma):
    # Aplicar la técnica SLS para obtener imágenes base
    tissue_img, bone_img = sls(low_energy_img, high_energy_img, wa, wb)

    # Calcular la imagen complementaria de hueso
    high_filter_bone = 1 -  gaussian_filter(tissue_img, sigma=sigma)
    high_filter_tissue = 1 - gaussian_filter(bone_img, sigma=sigma)
    high_filter_image = 1 -  gaussian_filter(high_energy_img,  sigma=sigma)

    # Generar imágenes finales de ACNR
    bone_img_acnr = bone_img + high_filter_bone * high_filter_image 
    # Generar imágenes finales de ACNR
    tissue_img_acnr = tissue_img + high_filter_tissue * high_filter_image 

    return tissue_img_acnr, bone_img_acnr