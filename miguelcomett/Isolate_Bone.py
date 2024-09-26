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
import cv2

# ===========================================================================================================

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
    
# ===========================================================================================================

def heatmap_array_dask(dataframe, x_branch, y_branch, size, num, save_as):

    x_data = dataframe[x_branch].to_dask_array(lengths=True)
    y_data = dataframe[y_branch].to_dask_array(lengths=True)

    set_bins = np.arange(-size, size + 1, size/num)
    heatmap, x_edges, y_edges = da.histogram2d(x_data, y_data, bins = [set_bins, set_bins])
    heatmap = heatmap.T

    heatmap = heatmap.compute()  
    x_edges = x_edges.compute()  
    y_edges = y_edges.compute()

    normal_map = 1 - heatmap / np.max(heatmap)

    columns = heatmap.shape[1]
    column_i = int(columns * 0.1)
    column_j = int(columns * 0.9)

    rows = heatmap.shape[0]
    row_i = int(rows * 0.01)
    row_j = int(rows * 0.1)

    maxi = np.max(normal_map[row_i:row_j, column_i:column_j])
    maxi = maxi * 1.1

    normal_map[normal_map < maxi] = 0

    plt.imshow(normal_map, cmap = 'gray', extent = [x_edges[0], x_edges[-1], y_edges[0], y_edges[-1]])
    plt.axis('off')
    plt.savefig(save_as + '.jpg', bbox_inches = 'tight', dpi = 900)

    return normal_map, x_edges, y_edges

# ===========================================================================================================

def CropImages(Im1H, Im2L, left, upper, right, lower, saveNames):

    imagen1 = Image.open(Im1H)
    imagen2 = Image.open(Im2L)

    region = (left, upper, right, lower) #(left, upper, right, lower)

    imagen1_recortada = imagen1.crop(region)
    imagen2_recortada = imagen2.crop(region)

    imagen1_recortada.save(saveNames[0])
    imagen2_recortada.save(saveNames[1])

# ===========================================================================================================

def sls(low_energy_img, high_energy_img, wa, wb):
    
    # Convertir imágenes a float para evitar errores de precisión
    low_energy_img = np.float64(low_energy_img)
    high_energy_img = np.float64(high_energy_img)

    # Aplicar la sustracción ponderada
    tissue_img = -wb * low_energy_img + high_energy_img
    bone_img = wa * low_energy_img - high_energy_img

    return tissue_img, bone_img

# ===========================================================================================================

def ssh(low_energy_img, high_energy_img, wa, wb, sigma):
    
    # Aplicar un filtro gaussiano a la imagen de alta energía   
    high_energy_img_smooth = gaussian_filter(high_energy_img, sigma=sigma)

    # Aplicar la sustracción ponderada
    tissue_img = -wb * low_energy_img + high_energy_img_smooth
    bone_img = wa * low_energy_img - high_energy_img_smooth

    return tissue_img, bone_img

# ===========================================================================================================

def acnr(low_energy_img, high_energy_img, wa , wb, sigma):

    # Aplicar la técnica SLS para obtener imágenes base
    tissue_img, bone_img = sls(low_energy_img, high_energy_img, wa, wb)

    # Calcular la imagen complementaria de hueso
    high_filter_bone = 1 -  gaussian_filter(tissue_img, sigma=sigma)
    high_filter_tissue = 1 - gaussian_filter(bone_img, sigma=sigma)
    high_filter_image = 1 -  gaussian_filter(high_energy_img,  sigma=sigma)

    bone_img_acnr = bone_img + high_filter_bone * high_filter_image 
    tissue_img_acnr = tissue_img + high_filter_tissue * high_filter_image 

    return tissue_img_acnr, bone_img_acnr