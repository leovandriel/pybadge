# PyBadge Experiments

# Setup

Setup CircuitPython: [https://circuitpython.org/board/pybadge/](https://circuitpython.org/board/pybadge/)

1. Double tap the reset button (or turn off/on while connected)
2. Drag and drop the CircuitPython UF2 file to PYBADGEBOOT

Connect Terminal:

    ls /dev/tty.*
    screen /dev/tty.usbmodem<..> 115200

# Read

* Adafruit PyBadge Product: [https://www.adafruit.com/product/4200](https://www.adafruit.com/product/4200)
* Adafruit PyBadge Learn: [https://learn.adafruit.com/adafruit-pybadge](https://learn.adafruit.com/adafruit-pybadge)
* CircuitPython Essentials: [https://learn.adafruit.com/circuitpython-essentials/](https://learn.adafruit.com/circuitpython-essentials/)
* PyBadge Demo: [https://learn.adafruit.com/pybadge-conference-badge-multi-language-unicode-fonts](https://learn.adafruit.com/pybadge-conference-badge-multi-language-unicode-fonts)


# Slideshow

Download [NeoPixel library](https://github.com/adafruit/Adafruit_CircuitPython_NeoPixel/releases) and copy `neopixel.mpy` to `lib/` on CIRCUITPY.

Copy slideshow folder:

    ./slideshow.sh <folder with images>

Install:

    ./install.sh slideshow.py


# License

MIT
