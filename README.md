# RPicoRGH
RGH 1.2 for Xbox 360 on a Raspberry Pi Pico

Forked version of Trinity model made by Balázs Triszka (2022)<br>
Some feedback and improvements to support other Xbox 360 models.<br>
For connecting pins, use the references available in a simple Google search for Xbox 360 motherboard.<br>
<b>(POST, CPU_RESET, CPU_PLL, 3.3V and GND)</b>

# Build
```
mkdir build
cd build
cmake ..
make
```

# TESTS MADE
- Tested in Xenon although you still get the expected result.

# Connections schema
+------------------------------+                    +-----------------------------+
| Raspberry Pi Pico            |                    | Xbox 360 Motherboard (Xenon)|
|------------------------------|                    |-----------------------------|
| 3.3V   --------------------> |                    |                             |
| GND    --------------------> | -----------------> | GND                         |
| GPIO 11 (CPU_RESET_PIN) ---->|                    | (CPU_RESET)                 |
| GPIO 12 (GLITCH_OUT_PIN) --->|                    | (CPU_PLL)                   |
| GPIO 14 (LED verde)          |                    |                             |
| GPIO 15 (LED rojo)           |                    |                             |
+------------------------------+                    +-----------------------------+

 
# TODO
- Add pin scheme for each Xbox 360 model, although by objective, the Xenon, Opus and Zephyr versions; since the Falcon, Jasper and onwards, have their methods for RGH3
- Compile the uf2 file
