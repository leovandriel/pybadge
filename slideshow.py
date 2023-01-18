# Show slideshows of BMP images on the PyBadge

import board
import displayio
import keypad
import neopixel
import os
import random
import time

delay = 5 # seconds between pictures
interval = 0.1 # seconds between button checks

display = board.DISPLAY
pad = keypad.ShiftRegisterKeys(clock=board.BUTTON_CLOCK, data=board.BUTTON_OUT, latch=board.BUTTON_LATCH, key_count=8, value_when_pressed=True)
neopixels = neopixel.NeoPixel(board.NEOPIXEL, 5, brightness=0.02, auto_write=False, pixel_order=neopixel.GRB)
files = sorted([file for file in os.listdir('/') if file.endswith('.bmp')], key=lambda _: random.random())
colorize = lambda v: min(max(abs((v % 6) * 768 // 6 - 384) - 64, 0), 255)
# palette = [colorize(i) + colorize(i + 6 // 3) * 0x100 + colorize(i + 6 * 2 // 3) * 0x10000 for i in range(0, 6)]
palette = [0xff0000, 0xffff00, 0x00ff00, 0x00ffff, 0x0000ff, 0xff00ff]

index = 0
show = False
freeze = False

while True:
    file = files[index % len(files)]
    bitmap = displayio.OnDiskBitmap(file)
    tile_grid = displayio.TileGrid(bitmap, pixel_shader=bitmap.pixel_shader)
    group = displayio.Group()
    group.append(tile_grid)
    display.show(group)
    for i in range(delay / interval + 1):
        for j in range(0, 5):
            if show:
                neopixels[j] = palette[(index + (0 if freeze else i // max(delay, 1)) + j) % len(palette)]
            else:
                neopixels[j] = 0x000000 if 4 - j < delay else 0xFF0000
            neopixels.show()
        event = pad.events.get()
        if event and event.pressed:
            if event.key_number in (7, 4): # Left/right button for previous/next
                index -= event.key_number % 2 * 2
                show = False
                break
            elif event.key_number in (5, 6): # Up/down button for faster/slower
                delay = min(max(delay + 11 - event.key_number * 2, 0), 5)
                if not freeze:
                    index -= 1
                show = False
                break
            elif event.key_number == 1: # A button for show/hide
                show = not show
            elif event.key_number == 0: # B button for freeze/unfreeze
                freeze = not freeze
        time.sleep(interval)
    if not freeze:
        index += 1
