### Pi Zero W

- I used Raspbian Jessie version 2017-0705 found at [Raspberry Pi Downloads](https://www.raspberrypi.org/downloads/raspbian)

	- Boot up and set preferences such as wifi, timezone, keyboard type, locale, etc

## LCD Driver

    - git clone this [goodtft/LCD-show repo](https://github.com/goodtft/LCD-show) to the /home/pi folder.
    - execute ./LCD35-show to enable the 3.5" display
    - execute ./LCD-hdmi to go back to a screen that you can actually read and get work done

## Neopixel Stick

    - Install the Pimoroni unicorn library
        - curl -sS https://get.pimoroni.com/unicornhat | bash

    - The LCD screen takes up many of the pins including the 5V power pins and BCM18 which is the default data pin for the unicorn hat
        - edit the /usr/local/lib/python2.7/dist-packages/unicornhat.py file and change LED_PIN from 18 to 21
            - LED_PIN        = 21      # GPIO pin connected to the pixels (must support PWM!).

    - Solder wires to the Neopixel stick and connect the pins:

| NeoPixel Pin | Pi Zero W Pin |
| --- | --- |
| 5V | Pin 2 - 5V |
| GND | Pin 34 - GND |
| Data | Pin 40 - BCM21 |

## Misc Pi changes

### Screen Saver, Menubar, Mouse AND autostart python script

    - edit the /home/pi/.config/lxsession/LXDE-pi/autostart to disable the screen saver, hide the menubar, hide the mouse and start the python script at start

