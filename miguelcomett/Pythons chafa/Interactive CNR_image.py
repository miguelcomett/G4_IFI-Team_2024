import numpy as np
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image

# ===========================================================================================

image_name = 'bone2' + '.png'

image = Image.open('RESULTS/' + image_name)
image = image.convert('L')
# plt.imshow(image, cmap='gray')

cropped_image = image
# cropped_image = image.crop((1000, 1300, image.width - 1000, image.width - 1300))
cropped_image = image.crop((650, 1100, image.width - 650, image.width - 1100))
# cropped_image = image.crop((520, 450, image.width - 580, image.width - 440))

data = np.array(cropped_image)

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

