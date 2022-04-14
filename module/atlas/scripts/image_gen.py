#!/usr/bin/python3

import argparse
import os
import pathlib
from colorsys import hsv_to_rgb
from random import randrange

from PIL import Image

parser = argparse.ArgumentParser()
parser.add_argument('--num_images', metavar='N', type=int, )
parser.add_argument('--output', metavar='N', type=str, )
parser.add_argument('--reuse', nargs='?', type=bool, default=False)
args = parser.parse_args()
if args.output is None:
    exit(1)
if args.reuse is not None and args.reuse:
    marker = os.path.join(args.output, "generated")
    if os.path.exists(marker):
        exit(0)
num_images = args.num_images
sizes = [
    32,
    64,
    128,
    256
]
print(f"Generating {num_images} images.")
for i in range(num_images):
    size = sizes[randrange(0, len(sizes))]
    img = Image.new('RGB', (size, size))
    (r, g, b) = hsv_to_rgb(randrange(0, 360) / 360, 0.6, 0.8)
    color = (int(r * 255), int(g * 255), int(b * 255))
    img.paste(color, [0, 0, size, size])
    out = os.path.join(args.output, f"image_{i}.png")
    img.save(out)

pathlib.Path(marker).touch();
