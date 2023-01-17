import board
import displayio
import os
import time

DELAY = 5 # seconds between pictures

display = board.DISPLAY

while True:
    for file in os.listdir('/'):
        if file.endswith('.bmp'):
            bitmap = displayio.OnDiskBitmap(file)
            tile_grid = displayio.TileGrid(bitmap, pixel_shader=bitmap.pixel_shader)
            group = displayio.Group()
            group.append(tile_grid)
            display.show(group)
            time.sleep(DELAY)
