import numpy as np
import matplotlib.pyplot as plt
import uproot
import os
import numpy as np
import dask.array as da
import dask.dataframe as dd

# ===========================================================================================

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


directory = 'RESULTS/'
root_name_starts = "arm_40kev_20M"

tree_name = "Photons"
x_branch  = "X_axis"
y_branch  = "Y_axis"
z_branch  = ""

decimal_places = 2

dataframe = root_to_dask(directory, root_name_starts, tree_name, x_branch, y_branch, decimal_places)

# ===========================================================================================

def heatmap_array_dask(dataframe, x_branch, y_branch, size, num, save_as):

    x_data = dataframe[x_branch].to_dask_array(lengths=True).compute()
    y_data = dataframe[y_branch].to_dask_array(lengths=True).compute()

    set_bins = np.arange(-size, size + 1, size/num)
    heatmap, x_edges, y_edges = np.histogram2d(x_data, y_data, bins = [set_bins, set_bins])
    heatmap = heatmap.T

    row = len(set_bins) // 2
    normal_map = 1 - heatmap / np.max(heatmap)
    maxi = np.max(normal_map[:5])
    maxi = maxi * 1.2
    normal_map[normal_map < maxi] = 0

    return normal_map, x_edges, y_edges


data = dataframe

x_branch = "X_axis"
y_branch = 'Y_axis'

size = 100
bins = 80

save_as = '1.jpg'

htmp_array, xlim, ylim = heatmap_array_dask(data, x_branch, y_branch, size, bins, save_as)

# ===========================================================================================

data = htmp_array
fig, ax = plt.subplots()
heatmap = ax.imshow(data, cmap='gray')

rectangles = []
start_pos = None
signal_avg = 0
background_avg = 0
background_std = 0

def on_press(event):
    global start_pos, rectangles
    if event.inaxes != ax: return
    start_pos = (event.xdata, event.ydata)
    rect = plt.Rectangle(start_pos, 1, 1, fill=False, color='yellow', lw=2)
    ax.add_patch(rect)
    rectangles.append(rect)
    
    # Delete old rectangles if more than 2
    if len(rectangles) > 2:
        first_rect = rectangles.pop(0)
        second_rect = rectangles.pop(0)
        first_rect.remove()
        second_rect.remove()
    
    fig.canvas.draw()

def on_motion(event):
    global start_pos
    if start_pos is None or event.inaxes != ax: return
    width = event.xdata - start_pos[0]
    height = event.ydata - start_pos[1]
    rect = rectangles[-1]  # Get the most recent rectangle
    rect.set_width(width)
    rect.set_height(height)
    fig.canvas.draw()

def on_release(event):
    global start_pos, signal_avg, background_avg, background_std
    if start_pos is None or event.inaxes != ax: return
    end_pos = (event.xdata, event.ydata)
    # print(f"Selected region from {start_pos} to {end_pos}")
    x1 = start_pos[0]
    y1 = start_pos[1]
    x2 = end_pos[0]
    y2 = end_pos[1]
    if len(rectangles) == 1:
        if x2 > x1:
            if y2 > y1:
                signal = data[round(y1):round(y2), round(x1):round(x2)]
            else:
                signal = data[round(y2):round(y1), round(x1):round(x2)]
        else:
            if y2 > y1:
                signal = data[round(y1):round(y2), round(x2):round(x1)]
            else:
                signal = data[round(y2):round(y1), round(x2):round(x1)]
        signal_avg = np.average(signal)
        print("Signal avg: "+str(signal_avg))
    else:
        if x2 > x1:
            if y2 > y1:
                background = data[round(y1):round(y2), round(x1):round(x2)]
            else:
                background = data[round(y2):round(y1), round(x1):round(x2)]
        else:
            if y2 > y1:
                background = data[round(y1):round(y2), round(x2):round(x1)]
            else:
                background = data[round(y2):round(y1), round(x2):round(x1)]
        background_avg = np.average(background)
        background_std = np.std(background)
        print("Background avg: "+str(background_avg))
        print("Background std dev: "+str(background_std))
        cnr = (signal_avg-background_avg)/background_std
        print("CNR: "+str(cnr)+'\n')
    start_pos = None

fig.canvas.mpl_connect('button_press_event', on_press)
fig.canvas.mpl_connect('motion_notify_event', on_motion)
fig.canvas.mpl_connect('button_release_event', on_release)

plt.show()
