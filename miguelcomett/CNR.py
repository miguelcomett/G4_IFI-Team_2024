import numpy as np
import matplotlib.pyplot as plt

# Generate some data
data = np.loadtxt('output.csv', delimiter=',')

# Create a heatmap
fig, ax = plt.subplots()
heatmap = ax.imshow(data, cmap='gray')

# Variables to store the initial click and the rectangle selection
rectangles = []
start_pos = None
signal_avg = 0
background_avg = 0
background_std = 0

# Mouse press event
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

# Mouse motion event to update the rectangle size
def on_motion(event):
    global start_pos
    if start_pos is None or event.inaxes != ax: return
    width = event.xdata - start_pos[0]
    height = event.ydata - start_pos[1]
    rect = rectangles[-1]  # Get the most recent rectangle
    rect.set_width(width)
    rect.set_height(height)
    fig.canvas.draw()

# Mouse release event to finalize the selection
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

# Connect the events
fig.canvas.mpl_connect('button_press_event', on_press)
fig.canvas.mpl_connect('motion_notify_event', on_motion)
fig.canvas.mpl_connect('button_release_event', on_release)

plt.show()
