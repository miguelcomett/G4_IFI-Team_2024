from PIL import Image
import numpy as np
import matplotlib.pyplot as plt
import os
import numpy as np

image = Image.open('RESULTS/' + 'denoised_bone2.png')
cropped_image = image.crop((520, 450, image.width - 580, image.width - 440))

data = np.array(cropped_image)
fig, ax = plt.subplots()
plt.imshow(data)

rectangles = []
start_pos = None
signal_avg = 0
background_avg = 0
background_std = 0

x1 = 1160
y1 = 1130
x2 = 1740
y2 = 1980

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

x1 = 1790
y1 = 1130
x2 = 1940
y2 = 1980

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