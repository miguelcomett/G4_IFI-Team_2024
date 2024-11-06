# 1.1. ========================================================================================================================================================

def Merge_Roots_Memory_Optimized(directory, starts_with, output_name):

    import uproot; import os; from tqdm import tqdm

    file_list = []

    # Crear lista de archivos para procesar
    for file in os.listdir(directory):
        if file.endswith('.root') and not file.startswith('merge') and not file.startswith(output_name):
            if starts_with == '' or file.startswith(starts_with):
                file_list.append(os.path.join(directory, file))

    # Crear el nombre de archivo de salida con numeración si ya existe
    merged_file = os.path.join(directory, output_name)
    counter = 0
    while os.path.exists(f"{merged_file}_{counter}.root"):
        counter += 1
    merged_file = f"{merged_file}_{counter}.root"

    with uproot.recreate(merged_file) as f_out:
        for file in tqdm(file_list, desc="Merging ROOT files", unit="file"):
            with uproot.open(file) as f_in:
                for key in f_in.keys():
                    base_key = key.split(';')[0]  # Obtener el nombre base sin número de ciclo
                    obj = f_in[key]

                    # Solo procesar si es un TTree
                    if isinstance(obj, uproot.TTree):
                        # Leer los datos por partes para optimizar el uso de memoria
                        for new_data in obj.iterate(library="np", step_size="10 MB"):
                            # Si el árbol ya existe en el archivo de salida, añadir los datos en partes
                            if base_key in f_out:
                                f_out[base_key].extend(new_data)
                            else:
                                # Crear un nuevo TTree en el archivo de salida con los primeros datos
                                f_out[base_key] = new_data

    print("Archivo final creado en:", merged_file)

# 1.1.1 ========================================================================================================================================================

import uproot
import os
from tqdm import tqdm
from concurrent.futures import ThreadPoolExecutor
import threading

def process_file(file, f_out, lock, step_size="10 MB"):
    with uproot.open(file) as f_in:
        for key in f_in.keys():
            base_key = key.split(';')[0]
            obj = f_in[key]

            # Solo procesar si es un TTree
            if isinstance(obj, uproot.TTree):
                # Leer los datos por partes para optimizar el uso de memoria
                for new_data in obj.iterate(library="np", step_size=step_size):
                    # Lock para asegurar que la escritura en f_out sea thread-safe
                    with lock:
                        # Añadir o crear el TTree en el archivo de salida
                        if base_key in f_out:
                            f_out[base_key].extend(new_data)
                        else:
                            f_out[base_key] = new_data

def Merge_Roots_Memory_Optimized_Parallel(directory, starts_with, output_name, max_workers=4):
    file_list = []

    # Crear lista de archivos para procesar
    for file in os.listdir(directory):
        if file.endswith('.root') and not file.startswith('merge') and not file.startswith(output_name):
            if starts_with == '' or file.startswith(starts_with):
                file_list.append(os.path.join(directory, file))

    # Crear el nombre de archivo de salida con numeración si ya existe
    merged_file = os.path.join(directory, output_name)
    counter = 0
    while os.path.exists(f"{merged_file}_{counter}.root"):
        counter += 1
    merged_file = f"{merged_file}_{counter}.root"

    # Crear un lock para el acceso a f_out
    lock = threading.Lock()

    with uproot.recreate(merged_file) as f_out:
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            futures = [
                executor.submit(process_file, file, f_out, lock)
                for file in file_list
            ]
            for future in tqdm(futures, desc="Merging ROOT files", unit="file"):
                future.result()  # Asegura que se complete cada tarea

    print("Archivo final creado en:", merged_file)



# 1.2. ========================================================================================================================================================

def ModifyRoot(directory, root_name, tree_name, branch_names, output_name, new_tree_name, new_branch_names):

    import uproot; import uproot.writing; import os

    input_file = directory + root_name + '.root'
    with uproot.open(input_file) as file:       
        tree = file[tree_name]
        branches = tree.arrays(branch_names, library="np")
        
    output_file = directory + output_name
    counter = 1
    while True:
        if not os.path.exists(f"{output_file}{counter}.root"):
            output_file = f"{output_file}{counter}.root"
            break
        counter = counter + 1

    with uproot.recreate(output_file) as new_file:
        new_file[new_tree_name] = {new_branch_names[0]: branches[branch_names[0]],
                                   new_branch_names[1]: branches[branch_names[1]]}

# 2.0. ========================================================================================================================================================

def Root_to_Dask(directory, root_name_starts, tree_name, x_branch, y_branch):
    
    import uproot; import numpy as np
    import dask.array as da; import dask.dataframe as dd

    file_name = directory + root_name_starts + ".root"

    with uproot.open(file_name) as root_file:
        tree = root_file[tree_name]
        if tree is None:
            print(f"Tree '{tree_name}' not found in {file_name}")
            return

        x_values = tree[x_branch].array(library="np") if x_branch in tree else print('error_x')
        y_values = tree[y_branch].array(library="np") if y_branch in tree else print('error_y')

        decimal_places = 3

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

        x_data = dask_df[x_branch].to_dask_array(lengths=True)
        y_data = dask_df[y_branch].to_dask_array(lengths=True)
        
        return x_data, y_data
    
# 3.0 ========================================================================================================================================================

def Heatmap_from_Dask(x_data, y_data, size, log_factor, x_shift, y_shift, save_as):

    import matplotlib.pyplot as plt; import numpy as np
    import dask.array as da; import dask.dataframe as dd

    x_data_shifted = x_data - x_shift
    y_data_shifted = y_data - y_shift

    pixel_size = 0.5 # mm
    set_bins = np.arange(-size, size + pixel_size, pixel_size)
    heatmap, x_edges, y_edges = da.histogram2d(x_data_shifted, y_data_shifted, bins = [set_bins, set_bins])
    heatmap = heatmap.T
    heatmap = np.rot90(heatmap, 2)
    print('Heatmap size:', heatmap.shape, '[pixels]')
    rows = heatmap.shape[0]

    heatmap = heatmap.compute()  
    x_edges = x_edges.compute()  
    y_edges = y_edges.compute()

    heatmap[heatmap == 0] = log_factor
    maxi = np.max(heatmap)
    normal_map = np.log(maxi / heatmap)

    plt.figure(figsize = (14, 4))
    plt.subplot(1, 3, 1); plt.imshow(normal_map, cmap = 'gray', extent = [x_edges[0], x_edges[-1], y_edges[0], y_edges[-1]]); plt.axis('off')
    if save_as != '': plt.savefig('Results/' + save_as + '.png', bbox_inches = 'tight', dpi = 900)
    plt.subplot(1, 3, 2); plt.plot(normal_map[2*rows//3,:])
    plt.subplot(1, 3, 3); plt.plot(normal_map[:,rows//2])

    return normal_map, x_edges, y_edges

# 4.1 ========================================================================================================================================================

def LoadRoots(directory, rootnames, tree_name, x_branch, y_branch):

    x_1, y_1 = Root_to_Dask(directory, rootnames[0], tree_name, x_branch, y_branch)
    x_2, y_2 = Root_to_Dask(directory, rootnames[1], tree_name, x_branch, y_branch)
    print("Dataframes created")

    return x_1, y_1, x_2, y_2

def IsolateTissues(low_energy_img, high_energy_img, sigma1, sigma2, wn, save_in, 
                   save_as_1, save_as_2, save_as_3, save_as_4, save_as_5, save_as_6, save_as_7, save_as_8):

    from scipy.ndimage import gaussian_filter
    import matplotlib.pyplot as plt

    U_b_l = 0.75187 # mu1
    U_b_h = 0.301189 # mu2
    U_t_l = 0.28283 # mu3
    U_t_h = 0.19155 # mu4

    SLS_Bone = ( (U_t_h/U_t_l) * low_energy_img ) - high_energy_img
    SLS_Tissue = high_energy_img - ( low_energy_img * (U_b_h/U_b_l) )

    SSH_Bone = ( (U_t_h/U_t_l) * low_energy_img) - gaussian_filter(high_energy_img, sigma = sigma1)
    SSH_Tissue = gaussian_filter(high_energy_img, sigma = sigma1) - ( low_energy_img * (U_b_h/U_b_l) )

    ACNR_Bone     = SLS_Bone +  gaussian_filter(SLS_Tissue, sigma = sigma1) - 1
    ACNR_SSH_Bone = SSH_Bone + (gaussian_filter(SSH_Tissue, sigma = sigma2) * wn) - 1
    ACNR_Tissue   = SLS_Tissue + gaussian_filter(SLS_Bone, sigma = sigma1) - 1

    plt.imshow(low_energy_img, cmap='gray'); plt.axis('off')
    if save_as_1 != '': plt.savefig(save_in + save_as_1, bbox_inches = 'tight', dpi = 600); plt.close()
    plt.imshow(high_energy_img, cmap='gray'); plt.axis('off')
    if save_as_2 != '': plt.savefig(save_in + save_as_2, bbox_inches = 'tight', dpi = 600); plt.close()
    plt.imshow(SLS_Bone, cmap='gray'); plt.axis('off')
    if save_as_3 != '': plt.savefig(save_in + save_as_3, bbox_inches = 'tight', dpi = 600); plt.close()
    plt.imshow(SLS_Tissue, cmap='gray'); plt.axis('off')
    if save_as_4 != '': plt.savefig(save_in + save_as_4, bbox_inches = 'tight', dpi = 600); plt.close()
    plt.imshow(SSH_Bone, cmap='gray'); plt.axis('off')
    if save_as_5 != '': plt.savefig(save_in + save_as_5, bbox_inches = 'tight', dpi = 600); plt.close()
    plt.imshow(SSH_Tissue, cmap='gray'); plt.axis('off')
    if save_as_6 != '': plt.savefig(save_in + save_as_6, bbox_inches = 'tight', dpi = 600); plt.close()
    plt.imshow(ACNR_Bone, cmap='gray'); plt.axis('off')
    if save_as_7 != '': plt.savefig(save_in + save_as_7, bbox_inches = 'tight', dpi = 600); plt.close()
    plt.imshow(ACNR_Tissue, cmap='gray'); plt.axis('off')
    if save_as_8 != '': plt.savefig(save_in + save_as_8, bbox_inches = 'tight', dpi = 600); 
    plt.close()

    plt.figure(figsize = (18, 10))
    plt.tight_layout()
    plt.subplot(2, 4, 1); plt.imshow(low_energy_img,  cmap='gray'); plt.axis('off');  plt.title("Low Energy")
    plt.subplot(2, 4, 2); plt.imshow(high_energy_img, cmap='gray'); plt.axis('off');  plt.title("High Energy")
    plt.subplot(2, 4, 3); plt.imshow(SLS_Bone,        cmap='gray'); plt.axis('off');  plt.title("Bone [SLS]")
    plt.subplot(2, 4, 4); plt.imshow(SLS_Tissue,      cmap='gray'); plt.axis('off');  plt.title("Tissue [SLS]")
    plt.subplot(2, 4, 5); plt.imshow(SSH_Bone,        cmap='gray'); plt.axis('off');  plt.title("Bone [SSH]")
    plt.subplot(2, 4, 6); plt.imshow(SSH_Tissue,      cmap='gray'); plt.axis('off');  plt.title("Tissue [SSh]")
    plt.subplot(2, 4, 7); plt.imshow(ACNR_Bone,       cmap='gray'); plt.axis('off');  plt.title("Bone [ACNR]")
    # plt.subplot(2, 4, 8); plt.imshow(ACNR_SSH_Bone,     cmap='gray'); plt.axis('off');  plt.title("Bone [ACNR + SSH]")
    plt.subplot(2, 4, 8); plt.imshow(ACNR_Tissue,     cmap='gray'); plt.axis('off');  plt.title("Tissue [ACNR]")
   
    return SLS_Bone, SSH_Bone, ACNR_Bone, ACNR_Tissue

def BMO(SLS_Bone, SLS_Tissue):

    import matplotlib.pyplot as plt

    U_b_l = 0.7519 # mu1
    U_b_h = 0.3012 # mu2
    # U_t_l = 0.26 # mu3
    # U_t_h = 0.18 # mu4
    U_t_l = 0.281
    U_t_h = 0.192

    Thick_cons_bone = (U_t_l) / ( (U_t_h * U_b_l) - (U_t_l * U_b_h) )
    thickness_bone = Thick_cons_bone * SLS_Bone
    Thick_cons_tissue = (U_t_l) / ( (U_t_l * U_b_h) - (U_t_h * U_b_l) )
    thickness_tissue = Thick_cons_tissue * SLS_Tissue

    plt.figure(figsize = (12, 3))
    plt.subplot(1, 3, 1); plt.imshow(thickness_bone); plt.colorbar()
    plt.subplot(1, 3, 2); plt.plot(thickness_bone[120,:])
    plt.subplot(1, 3, 3); plt.plot(thickness_bone[:,120])
    plt.show()

    return thickness_bone

# 5.1 ========================================================================================================================================================

def Interactive_CNR(cropped_image):

    import numpy as np; import matplotlib.pyplot as plt

    data = np.array(cropped_image)
    fig, ax = plt.subplots()
    heatmap = ax.imshow(data, cmap='gray')

    rectangles = []
    start_pos = [None]  # Using a list to store coordinates
    signal_avg = [0]
    background_avg = [0]
    background_std = [0]

    def on_press(event):
        if event.inaxes != ax: return
        start_pos[0] = (event.xdata, event.ydata)
        rect = plt.Rectangle(start_pos[0], 1, 1, fill=False, color='blue', lw=1)
        ax.add_patch(rect)
        rectangles.append(rect)

        if len(rectangles) > 2:
            first_rect = rectangles.pop(0)
            second_rect = rectangles.pop(0)
            first_rect.remove()
            second_rect.remove()

        fig.canvas.draw()

    def on_motion(event):
        if start_pos[0] is None or event.inaxes != ax: return
        width = event.xdata - start_pos[0][0]
        height = event.ydata - start_pos[0][1]
        rect = rectangles[-1]
        rect.set_width(width)
        rect.set_height(height)
        fig.canvas.draw()

    def on_release(event):
        if start_pos[0] is None or event.inaxes != ax: return
        end_pos = (event.xdata, event.ydata)

        x1 = start_pos[0][0]
        y1 = start_pos[0][1]
        x2 = end_pos[0]
        y2 = end_pos[1]

        if len(rectangles) == 1:
            if x2 > x1:
                if y2 > y1: signal = data[round(y1):round(y2), round(x1):round(x2)]
                else:       signal = data[round(y2):round(y1), round(x1):round(x2)]
            else:
                if y2 > y1: signal = data[round(y1):round(y2), round(x2):round(x1)]
                else:       signal = data[round(y2):round(y1), round(x2):round(x1)]

            signal_avg[0] = np.average(signal)
            print("Signal avg: "+str(signal_avg[0]))
        else:
            if x2 > x1:
                if y2 > y1: background = data[round(y1):round(y2), round(x1):round(x2)]
                else:       background = data[round(y2):round(y1), round(x1):round(x2)]
            else:
                if y2 > y1: background = data[round(y1):round(y2), round(x2):round(x1)]
                else:       background = data[round(y2):round(y1), round(x2):round(x1)]

            background_avg[0] = np.average(background)
            background_std[0] = np.std(background)
            print("Background avg: "+str(background_avg[0]))
            print("Background std dev: "+str(background_std[0]))
            cnr = (signal_avg[0] - background_avg[0]) / background_std[0]
            print("CNR: " + str(cnr) + '\n')

        start_pos[0] = None

    fig.canvas.mpl_connect('button_press_event', on_press)
    fig.canvas.mpl_connect('motion_notify_event', on_motion)
    fig.canvas.mpl_connect('button_release_event', on_release)

    plt.show()

# 5.2 ========================================================================================================================================================

def Fixed_CNR(image_path, save_as, coords_signal, coords_bckgrnd):
    
    from PIL import Image; import numpy as np; import matplotlib.pyplot as plt

    image = Image.open(image_path)
    image = image.convert('L')
    cropped_image = image
    # cropped_image = image.crop((520, 450, image.width - 580, image.width - 440))
    data = np.array(cropped_image)

    plt.imshow(data, cmap = 'gray')
    plt.axis('off')

    signal_avg = 0
    background_avg = 0
    background_std = 0

    x1_signal = coords_signal[0]
    y1_signal = coords_signal[1]
    x2_signal = coords_signal[2]
    y2_signal = coords_signal[3]

    plt.gca().add_patch(plt.Rectangle((x1_signal, y1_signal), x2_signal - x1_signal, y2_signal - y1_signal, linewidth=2, edgecolor='yellow', facecolor='none'))

    if x2_signal > x1_signal:
        if y2_signal > y1_signal:
            signal = data[round(y1_signal):round(y2_signal), round(x1_signal):round(x2_signal)]
        else:
            signal = data[round(y2_signal):round(y1_signal), round(x1_signal):round(x2_signal)]
    else:
        if y2_signal > y1_signal:
            signal = data[round(y1_signal):round(y2_signal), round(x2_signal):round(x1_signal)]
        else:
            signal = data[round(y2_signal):round(y1_signal), round(x2_signal):round(x1_signal)]

    signal_avg = np.average(signal)
    # signal_std = np.std(signal)
    print("Signal avg: ", round(signal_avg, 3))

    x1_background = coords_bckgrnd[0]
    y1_background = coords_bckgrnd[1]
    x2_background = coords_bckgrnd[2]
    y2_background = coords_bckgrnd[3]

    plt.gca().add_patch(plt.Rectangle((x1_background, y1_background), x2_background - x1_background, y2_background - y1_background, linewidth=2, edgecolor='red', facecolor='none'))

    if x2_background > x1_background:
        if y2_background > y1_background:
            background = data[round(y1_background):round(y2_background), round(x1_background):round(x2_background)]
        else:
            background = data[round(y2_background):round(y1_background), round(x1_background):round(x2_background)]
    else:
        if y2_background > y1_background:
            background = data[round(y1_background):round(y2_background), round(x2_background):round(x1_background)]
        else:
            background = data[round(y2_background):round(y1_background), round(x2_background):round(x1_background)]

    background_avg = np.average(background)
    background_std = np.std(background)

    print("Background avg: ", round(background_avg, 3))
    print("Background std dev: ", round(background_std, 3))

    cnr = (signal_avg - background_avg) / background_std
    # cnr = (background_avg - signal_avg) / signal_std
    print("CNR: ", round(cnr, 1))

    if save_as != '': plt.savefig('RESULTS/' + save_as + '.png', bbox_inches = 'tight', dpi = 900)

# 6.1 ========================================================================================================================================================

def Denoise_EdgeDetection(path, isArray, sigma_color, sigma_spatial):

    import numpy as np; import matplotlib.pyplot as plt
    from skimage.restoration import denoise_bilateral; from PIL import Image
    
    if isArray == True:
        original_image = np.array(path)
    else:
        original_image = Image.open(path)
        
    denoised_image = denoise_bilateral(original_image, sigma_color = sigma_color, sigma_spatial = sigma_spatial, channel_axis = None)

    save_as = ''

    plt.figure(figsize = (10, 5))

    plt.subplot(1, 2, 1)
    plt.imshow(denoised_image, cmap = 'gray')
    plt.title('Denoised Image')
    plt.axis('off')
    if save_as != '': plt.savefig('RESULTS/' + save_as + '.png', bbox_inches = 'tight', dpi = 900)

    plt.subplot(1, 2, 2)
    plt.imshow(original_image, cmap = 'gray')
    plt.title('Original Image')
    plt.axis('off')

    plt.show()

    return denoised_image

# 6.2 ========================================================================================================================================================

def Denoise(array, isHann, alpha, save_as, isCrossSection):
    
    import numpy as np; import matplotlib.pyplot as plt
    from scipy import signal; from scipy.fft import fft2, fftshift, ifft2

    image = array

    fft_image = fft2(image)
    fft_image = fftshift(fft_image)

    rows, cols = image.shape

    if isHann == True:
    
        l = rows * alpha
        a = np.hanning(l)
        b = np.hanning(l)

        padding_size = rows - len(a)
        left_padding = padding_size // 2
        right_padding = padding_size - left_padding
        a = np.pad(a, (left_padding, right_padding), mode='constant')

        padding_size = cols - len(b)
        left_padding = padding_size // 2
        right_padding = padding_size - left_padding
        b = np.pad(b, (left_padding, right_padding), mode='constant')

        window = np.outer(a, b)

    else:

        a = signal.windows.tukey(rows, alpha)
        b = signal.windows.tukey(rows, alpha)
        window = np.outer(a, b)

    fft_image_2 = fft_image * (window)
    fft_image = fftshift(fft_image_2)
    fft_image = (ifft2(fft_image))
    fft_image = (np.abs(fft_image))

    if isCrossSection == True:
        
        plt.figure(figsize = (7, 3))
        plt.subplot(1, 2, 1); plt.plot(a); plt.title('Window')
        plt.subplot(1, 2, 2); plt.plot(np.abs((fft_image_2[:][rows//2]))); plt.title('F. Transform Slice')

        plt.figure(figsize = (7, 3))
        plt.subplot(1, 2, 1); plt.plot(image[:][rows//2]); plt.title('Original Slice')
        plt.subplot(1, 2, 2); plt.plot(np.abs(fft_image[:][rows//2])); plt.title('Denoised Slice')

    plt.figure(figsize = (8, 4))
    plt.subplot(1, 2, 1); plt.imshow(image, cmap = 'gray'); plt.title('Original Image'); plt.axis('off')
    plt.subplot(1, 2, 2); plt.imshow(fft_image, cmap = 'gray'); plt.title('Filtered Image'); plt.axis('off')
    if save_as != '': plt.savefig('Results/' + save_as + '.png', dpi = 900)
    plt.show()

    return fft_image

# 7.0 ========================================================================================================================================================

def Plotly_Heatmap(array, xlim, ylim, title, x_label, y_label, annotation, width, height, save_as):

    import plotly.io as pio; import plotly.graph_objects as go

    font_family = 'Merriweather'
    family_2    = 'Optima'
    font_small  = 16
    font_medium = 20
    font_large  = 18

    fig = go.Figure(go.Heatmap(z = array, x = xlim, y = ylim,
                                colorscale = [[0, 'black'], [1, 'white']], 
                                colorbar = dict(title = "Density", tickfont = dict(family = family_2, size = 15, color = 'Black'))))
    
    fig.update_layout(
                    title = dict(text = title, font = dict(family = font_family, size = font_large, color = "Black"), 
                                 x = 0.51, y = 0.93, yanchor = 'middle', xanchor = 'center'),
                    xaxis_title = dict(text = x_label, font = dict(family = font_family, size = font_medium, color = "Black")),
                    yaxis_title = dict(text = y_label, font = dict(family = font_family, size = font_medium, color = "Black")),
                    xaxis = dict(tickfont = dict(family = family_2, size = font_small, color = "Black"), title_standoff = 25),
                    yaxis = dict(tickfont = dict(family = family_2, size = font_small, color = "Black"), title_standoff = 10),
                    width = width, height = height, margin = dict(l = 105, r = 90, t = 90, b = 90),
                    annotations = [dict(x = 0.95, y = 0.15,  xref = 'paper', yref = 'paper', showarrow = False,
                                        font = dict(family = family_2, size = 15, color = "White"),
                                        bgcolor = "rgba(255, 255, 255, 0.1)", borderpad = 8, bordercolor = "White", borderwidth = 0.2,
                                        text = annotation)]
    )
   
    if save_as != '': pio.write_image(fig, 'Results/' + save_as + '.png', width = width, height = height, scale = 5)
    fig.show()

# 8.0 ========================================================================================================================================================

def CT_Heatmap_from_Dask(x_data, y_data, size_x, size_y, log_factor, x_shift, y_shift, pixel_size):

    import matplotlib.pyplot as plt; import numpy as np
    import dask.array as da; import dask.dataframe as dd

    x_data_shifted = x_data - x_shift
    y_data_shifted = y_data - y_shift

    set_bins_x = np.arange(-size_x, size_x + pixel_size, pixel_size)
    set_bins_y = np.arange(-size_y, size_y + pixel_size, pixel_size)
    heatmap, x_edges, y_edges = da.histogram2d(x_data_shifted, y_data_shifted, bins = [set_bins_x, set_bins_y])
    heatmap = heatmap.T
    heatmap = np.rot90(heatmap, 2)

    heatmap = heatmap.compute()  
    x_edges = x_edges.compute()  
    y_edges = y_edges.compute()
    
    # maxi = np.max(heatmap)
    # log_map = np.log(maxi/(heatmap + log_factor)) / (pixel_size * 0.1)

    log_map = heatmap

    # plt.imshow(log_map, cmap = 'gray', extent=[x_edges[0], x_edges[-1], y_edges[0], y_edges[-1]])

    return log_map, x_edges, y_edges


def Calculate_Projections(directory, roots, tree_name, x_branch, y_branch, dimensions, log_factor, pixel_size, csv_folder):
    
    import numpy as np; from tqdm import tqdm

    start = roots[0]
    end = roots[1]
    deg = roots[2]

    size_x = dimensions[0]
    size_y = dimensions[1]
    x_shift = dimensions[2]
    y_shift = dimensions[3]

    sims = np.arange(start, end+1, deg)

    for i, sim in tqdm(enumerate(sims), desc = 'Calculating heatmaps', unit = ' Heatmaps', leave = True):
                    #    dynamic_ncols=True, bar_format='{desc}: {percentage:3.0f}%|{bar}| {n_fmt}/{total_fmt} [{elapsed}<{remaining}, {rate_fmt}{postfix}]'):
        
        root_name = "/Sim" + str(round(sim))

        x_data, y_data = Root_to_Dask(directory, root_name, tree_name, x_branch, y_branch)
        htmp_array, xlim, ylim = CT_Heatmap_from_Dask(x_data, y_data, size_x, size_y, log_factor, x_shift, y_shift, pixel_size)

        name = csv_folder + f"/Sim{round(sims[i])}.csv"
        np.savetxt(name, htmp_array, delimiter=',', fmt='%.5f')

    return htmp_array, xlim, ylim


def htmps_from_csv(csv_folder, roots):

    import numpy as np

    start = roots[0]
    end = roots[1]
    deg = roots[2]
    sims = np.arange(start, end+1, deg)
    
    htmps = np.zeros(len(sims), dtype=object)
    for i, sim in enumerate(sims):
        name = csv_folder + f"/Sim{round(sim)}.csv"
        htmps[i] = np.genfromtxt(name, delimiter = ',')

    return htmps

def LogaritmicTransformation(radiographs, log_factor, pixel_size):
    
    import matplotlib.pyplot as plt; import numpy as np

    # maxi =  np.max(radiographs[0])
    htmps = np.zeros(len(radiographs), dtype = 'object')

    for i, radiograph in enumerate(radiographs):

        maxi = np.max(radiograph)
        htmps[i] = np.log(maxi/(radiograph + log_factor)) / (pixel_size * 0.1)

    plt.imshow(htmps[-1]); plt.colorbar(); plt.show()

    return htmps


def RadonReconstruction(roots, htmps, slices):

    from skimage.transform import iradon
    import numpy as np; import matplotlib.pyplot as plt
    import plotly.graph_objects as go; import plotly.io as pio

    height = len(htmps[0])
    n = height/(slices+1)
    heights = np.round(np.arange(n, height, n)).astype(int)

    start = roots[0]
    end = roots[1]
    deg = roots[2]

    thetas = np.arange(start, end+1, deg)
    reconstructed_imgs = np.zeros(slices, dtype="object")

    for i, layer in enumerate(heights):

        p = np.array([heatmap[layer] for heatmap in htmps]).T
        reconstructed_imgs[i] = iradon(p, theta = thetas)
        #reconstructed_imgs[i][reconstructed_imgs[i] > 0.2] = 0
        reconstructed_imgs[i][reconstructed_imgs[i] < 0] = 0
        reconstructed_imgs[i][reconstructed_imgs[i] > 0.5] = 0

    # plt.figure(figsize = (6,6)); plt.imshow(reconstructed_imgs[slices//2], cmap = 'gray'); plt.colorbar(); plt.show()
    
    fig = go.Figure(go.Heatmap(z = reconstructed_imgs[0]))
    fig.update_layout(width = 800, height = 800, xaxis = dict(autorange = 'reversed'), yaxis = dict(autorange = 'reversed'))
    fig.show()

    return reconstructed_imgs


def coefficients_to_HU(reconstructed_imgs, slices, mu_water):

    import numpy as np; import plotly.graph_objects as go; import plotly.io as pio

    # air_parameter = -600

    HU_images = np.zeros(slices, dtype="object")

    for i in range(len(HU_images)):

        HU_images[i] = np.round(1000 * ((reconstructed_imgs[i] - mu_water) / mu_water)).astype(int)
        # HU_images[i][HU_images[i] < air_parameter] = -1000

    fig = go.Figure(go.Heatmap(z = HU_images[0], colorscale = [[0, 'black'], [1, 'white']],))
    fig.update_layout(width = 800, height = 800, xaxis = dict(autorange = 'reversed'), yaxis = dict(autorange = 'reversed'))
    fig.show()

    return HU_images


def export_to_dicom(HU_images, size_y, slices, directory):

    import numpy as np; import pydicom; from pydicom.dataset import Dataset, FileDataset; from pydicom.uid import ExplicitVRLittleEndian

    image2d = HU_images[0].astype(np.uint16)

    print("Setting file meta information...")

    meta = pydicom.Dataset()
    meta.MediaStorageSOPClassUID = pydicom.uid.CTImageStorage
    meta.MediaStorageSOPInstanceUID = pydicom.uid.generate_uid()
    meta.TransferSyntaxUID = pydicom.uid.ImplicitVRLittleEndian  

    ds = Dataset()
    ds.file_meta = meta

    ds.is_little_endian = True
    ds.is_implicit_VR = False

    ds.SOPClassUID = pydicom.uid.CTImageStorage 
    ds.PatientName = "NAME^NONE"
    ds.PatientID = "NOID"

    ds.Modality = "CT"
    ds.SeriesInstanceUID = pydicom.uid.generate_uid()
    ds.StudyInstanceUID = pydicom.uid.generate_uid()
    ds.FrameOfReferenceUID = pydicom.uid.generate_uid()
    ds.SeriesNumber = 4

    ds.BitsStored = 16
    ds.BitsAllocated = 16
    ds.SamplesPerPixel = 1
    ds.HighBit = 15

    ds.Rows = image2d.shape[0]
    ds.Columns = image2d.shape[1]
    ds.AcquisitionNumber = 1

    ds.ImageOrientationPatient = r"1\0\0\0\1\0"
    ds.ImageType = r"ORIGINAL\PRIMARY\AXIAL"

    ds.RescaleIntercept = "0"
    ds.RescaleSlope = "1"
    ds.PixelSpacing = r"0.5\0.5"
    ds.PhotometricInterpretation = "MONOCHROME2"
    ds.PixelRepresentation = 1
    # ds.RescaleIntercept = "-1024"
    ds.RescaleType = 'HU'

    pydicom.dataset.validate_file_meta(ds.file_meta, enforce_standard=True)

    print("Setting pixel data...")
    i = 0
    for image in HU_images:
        thickness = (size_y * 2)/slices
        image2d = image.astype(np.uint16)
        ds.PixelData = image2d.tobytes()
        name = directory + f"/I{i}.dcm"
        ds.SliceThickness = str(thickness)
        ds.SpacingBetweenSlices = str(thickness)
        ds.ImagePositionPatient = f"0\\0\\{thickness * i}"
        ds.SliceLocation = str(thickness * i)+'00'
        ds.InstanceNumber = i+1
        ds.save_as(name)
        i += 1

# end ========================================================================================================================================================