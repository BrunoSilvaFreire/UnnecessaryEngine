#!/usr/bin/python3

import argparse
import os
from colorsys import hsv_to_rgb
from random import randrange

from PIL import Image

parser = argparse.ArgumentParser()
parser.add_argument('--num_images', metavar='N', type=int, )
parser.add_argument('--output', metavar='N', type=str, )
parser.add_argument('--reuse', nargs='?', type=bool, default=False)
args = parser.parse_args()
if args.output is not None and args.reuse is not None and args.reuse:
    marker = os.path.join(args.output, "generated")
    if os.path.exists(marker):
        exit(0)
num_images = args.num_images
min = 16
max = 512
print(f"Generating {num_images} images.")
for i in range(num_images):
    size_x = randrange(min, max)
    size_y = randrange(min, max)
    img = Image.new('RGB', (size_x, size_y))
    (r, g, b) = hsv_to_rgb(randrange(0, 360) / 360, 0.6, 0.8)
    color = (int(r * 255), int(g * 255), int(b * 255))
    img.paste(color, [0, 0, size_x, size_y])
    out = os.path.join(args.output, f"image_{i}.png")
    img.save(out)
