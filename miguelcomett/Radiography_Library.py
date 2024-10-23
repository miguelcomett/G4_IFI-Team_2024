# 1.0. ========================================================================================================================================================

def Merge_Roots(directory, starts_with, output_name):

    import uproot
    import os
    import numpy as np
    from tqdm import tqdm

    file_list = []

    for file in os.listdir(directory):
        if file.endswith('.root') and not file.startswith('merge') and not file.startswith(output_name):
            if not starts_with == '' and file.startswith(starts_with):
                    file_list.append(os.path.join(directory, file))
    
    merged_file = os.path.join(directory, output_name)
    
    counter = 0
    while True:
        if not os.path.exists(f"{merged_file}{counter}.root"):
            merged_file = f"{merged_file}{counter}.root"
            break
        counter = counter + 1

    with uproot.recreate(merged_file) as f_out:
        data_dict = {}  # Dictionary to store merged data temporarily
        
        for file in tqdm(file_list, desc = "Reading files", unit = "File", leave = True):
            
            with uproot.open(file) as f_in:
                for key in f_in.keys():
                    obj = f_in[key]
                    
                    if isinstance(obj, uproot.TTree):
                        new_data = obj.arrays(library="np")
                        base_key = key.split(';')[0] # Extract base key name (ignore cycle numbers)

                        if base_key in data_dict: # If base_key is already in data_dict, concatenate data
                            existing_data = data_dict[base_key]
                            combined_data = {k: np.concatenate([existing_data[k], new_data[k]]) for k in new_data.keys() if k in existing_data}
                            data_dict[base_key] = {**existing_data, **combined_data} # Update with the combined data
                        
                        else:
                            data_dict[base_key] = new_data # If base_key is not in data_dict, add new data

        for key, data in tqdm(data_dict.items(), desc = 'Writing file', unit='data', leave = True):
            f_out[key] = data


# 2.0. ========================================================================================================================================================

def Root_to_Dask(directory, root_name_starts, tree_name, x_branch, y_branch):
    
    import uproot
    import os
    import numpy as np
    import dask.array as da
    import dask.dataframe as dd

    file_name = os.path.join(directory, root_name_starts + ".root")

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

    import matplotlib.pyplot as plt
    import numpy as np
    import dask.array as da
    import dask.dataframe as dd

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

    maxi = np.max(heatmap) 
    normal_map = np.log( maxi / (heatmap + log_factor) )
    # normal_map = heatmap

    plt.figure(figsize = (14, 4))
    plt.subplot(1, 3, 1)
    plt.imshow(normal_map, cmap = 'gray', extent = [x_edges[0], x_edges[-1], y_edges[0], y_edges[-1]])
    plt.axis('off')
    if save_as != '': plt.savefig('Results/' + save_as + '.png', bbox_inches = 'tight', dpi = 900)
    plt.subplot(1, 3, 2)
    plt.plot(normal_map[2*rows//3,:])
    plt.subplot(1, 3, 3)
    plt.plot(normal_map[:,rows//2])

    return normal_map, x_edges, y_edges

# 4.1 ========================================================================================================================================================

def LoadRoots(directory, rootnames, tree_name, x_branch, y_branch):

    x_1, y_1 = Root_to_Dask(directory, rootnames[0], tree_name, x_branch, y_branch)
    x_2, y_2 = Root_to_Dask(directory, rootnames[1], tree_name, x_branch, y_branch)
    print("Dataframes created")

    return x_1, y_1, x_2, y_2

def IsolateTissues(low_energy_img, high_energy_img, save_as):

    from scipy.ndimage import gaussian_filter
    import matplotlib.pyplot as plt

    U_b_l = 0.7519 # mu1
    U_b_h = 0.3012 # mu2
    U_t_l = 0.26 # mu3
    U_t_h = 0.18 # mu4

    SLS_Bone = ( (U_t_h/U_t_l) * low_energy_img) - high_energy_img
    SLS_Tissue = high_energy_img - ( low_energy_img * (U_b_h/U_b_l) )

    sigma = 10
    high_energy_filter = gaussian_filter(high_energy_img, sigma = sigma)
    SSH_Bone = ( (U_t_h/U_t_l) * low_energy_img) - high_energy_filter
    SSH_Tissue = high_energy_filter - ( low_energy_img * (U_b_h/U_b_l) )

    sigma = 100
    wn = 1
    tissue_filter = 1 - (gaussian_filter(SSH_Tissue, sigma = sigma) * wn)
    ACNR_Bone = SSH_Bone - (tissue_filter)
    # acnr_bone[acnr_bone < 0.0] = 0

    fig, ax = plt.subplots(1, 4, figsize=(12, 6))
    img1 = ax[0].imshow(low_energy_img, cmap='gray')
    ax[0].set_title("Low Energy")
    img2 = ax[1].imshow(high_energy_img, cmap='gray')
    ax[1].set_title("High Energy")
    img7 = ax[2].imshow(SSH_Bone, cmap='gray')
    ax[2].set_title("Bone [SSH]")
    img8 = ax[3].imshow(ACNR_Bone, cmap='gray')
    ax[3].set_title("Bone [ACNR + SSH]")
    plt.tight_layout()
    if save_as != '': plt.savefig('Results/' + save_as + '.png', bbox_inches = 'tight', dpi = 900)
    plt.show()

    # ACNR 2____________
    # w = U_t_h / U_t_l
    # wc = U_b_h / U_b_l
    # wn = 0.1
    # sigma = 15
    # low  = - (wn * wc * gaussian_filter(low_energy_img, sigma = sigma) ) + (w * low_energy_img)
    # high = - high_energy_img + ( wn * gaussian_filter(high_energy_img, sigma = sigma))
    # acnr2 = low + high

    return SSH_Bone, ACNR_Bone

def BMO(SLS_Bone, SLS_Tissue):

    import matplotlib.pyplot as plt

    U_b_l = 0.7519 # mu1
    U_b_h = 0.3012 # mu2
    U_t_l = 0.26 # mu3
    U_t_h = 0.18 # mu4

    Thick_cons_bone = (U_t_l) / ( (U_t_h * U_b_l) - (U_t_l * U_b_h) )
    thickness_bone = Thick_cons_bone * SLS_Bone
    Thick_cons_tissue = (U_t_l) / ( (U_t_l * U_b_h) - (U_t_h * U_b_l) )
    thickness_tissue = Thick_cons_tissue * SLS_Tissue

    plt.figure(figsize = (12, 3))
    plt.subplot(1, 3, 1)
    plt.imshow(thickness_bone)
    plt.colorbar()
    plt.subplot(1, 3, 2)
    plt.plot(thickness_bone[120,:])
    plt.subplot(1, 3, 3)
    plt.plot(thickness_bone[:,120])
    plt.show()

    return thickness_bone

# 5.1 ========================================================================================================================================================

def Interactive_CNR(cropped_image):

    import numpy as np
    import matplotlib.pyplot as plt

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
    
    from PIL import Image
    import numpy as np
    import numpy as np
    import matplotlib.pyplot as plt

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

    from skimage.restoration import denoise_bilateral
    import matplotlib.pyplot as plt
    from PIL import Image
    import numpy as np
    
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
    
    import numpy as np
    from scipy.fft import fft2, fftshift, ifft2
    import matplotlib.pyplot as plt
    from scipy import signal

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
        plt.subplot(1, 2, 1)
        plt.plot(a)
        plt.title('Window')
        plt.subplot(1, 2, 2)
        plt.plot(np.abs((fft_image_2[:][rows//2])))
        plt.title('F. Transform Slice')

        plt.figure(figsize = (7, 3))
        plt.subplot(1, 2, 1)
        plt.plot(image[:][rows//2])
        plt.title('Original Slice')
        plt.subplot(1, 2, 2)
        plt.plot(np.abs(fft_image[:][rows//2]))
        plt.title('Denoised Slice')

    plt.figure(figsize = (8, 4))
    plt.subplot(1, 2, 1)
    plt.title('Original Image')
    plt.imshow(image, cmap = 'gray')
    plt.axis('off')
    plt.subplot(1, 2, 2)
    plt.title('Filtered Image')
    plt.imshow(fft_image, cmap = 'gray')
    plt.axis('off')
    if save_as != '': plt.savefig('Results/' + save_as + '.png', dpi = 900)
    plt.show()

    return fft_image

# 7.0 ========================================================================================================================================================

def Plotly_Heatmap(array, xlim, ylim, title, x_label, y_label, annotation, width, height, save_as):

    import plotly.graph_objects as go
    import plotly.io as pio

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

# end ========================================================================================================================================================