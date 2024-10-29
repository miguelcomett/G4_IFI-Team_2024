import Radiography_Library as RadLib

from PIL import Image
import matplotlib.pyplot as plt

directory = 'RESULTS/'
image = '1png' + '.png'
image = Image.open(directory + image)
image = image.convert('L')

print(image.size)
width = image.width
height = image.height

trim = 200
cropped_image = image.crop((trim, trim, width - trim, height - trim))

RadLib.Interactive_CNR(cropped_image)