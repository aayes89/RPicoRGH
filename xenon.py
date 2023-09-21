# Plantilla de prueba para ajustar según requisitos de consola

import machine
import utime

# Configura los pines GPIO de la RP2040
TCK_PIN = machine.Pin(2, machine.Pin.OUT)  # Asigna el pin 2 a TCK
TMS_PIN = machine.Pin(3, machine.Pin.OUT)  # Asigna el pin 3 a TMS
TDI_PIN = machine.Pin(4, machine.Pin.OUT)  # Asigna el pin 4 a TDI
TDO_PIN = machine.Pin(5, machine.Pin.IN)   # Asigna el pin 5 a TDO
GND_PIN = machine.Pin(6, machine.Pin.OUT)  # Asigna el pin 6 a GND
VCC_PIN = machine.Pin(7, machine.Pin.OUT)  # Asigna el pin 7 a VCC

# Función para generar un pulso de clock (TCK)
def toggle_clock():
    TCK_PIN.on()
    utime.sleep_us(1)  # Ajusta este valor según tus necesidades
    TCK_PIN.off()
    utime.sleep_us(1)  # Ajusta este valor según tus necesidades

# Función para cambiar el estado de TMS
def set_tms(state):
    TMS_PIN.value(state)

# Función para cambiar el estado de TDI
def set_tdi(state):
    TDI_PIN.value(state)

# Función para leer el estado de TDO
def read_tdo():
    return TDO_PIN.value()

# Configura el estado inicial de los pines
TCK_PIN.off()
set_tms(0)
set_tdi(0)
GND_PIN.off()
VCC_PIN.on()

# Ejemplo de secuencia de JTAG (personaliza según tus necesidades)
set_tms(1)  # Entra en estado Test-Logic-Reset (TLR)
toggle_clock()

# Aquí puedes agregar tu secuencia de operaciones JTAG, incluyendo glitches

# Vuelve al estado TLR
set_tms(1)
toggle_clock()

# Libera los recursos
TCK_PIN.deinit()
TMS_PIN.deinit()
TDI_PIN.deinit()
TDO_PIN.deinit()
GND_PIN.deinit()
VCC_PIN.deinit()
