#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware/vreg.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "glitch.pio.h"

#define CPU_RESET_PIN 11
#define PLL_BYPASS_PIN 12
#define DEBUG_LED_PIN 25
#define GPIO_TRIGGER_PIN 15

// Función para configurar los pines GPIO en RP2040
void configurePins() {
    gpio_init(CPU_RESET_PIN);
    gpio_init(PLL_BYPASS_PIN);
    gpio_init(DEBUG_LED_PIN);
    gpio_set_dir(CPU_RESET_PIN, GPIO_IN);
    gpio_set_dir(PLL_BYPASS_PIN, GPIO_OUT);
    gpio_set_dir(DEBUG_LED_PIN, GPIO_OUT);
    gpio_set_slew_rate(CPU_RESET_PIN, GPIO_SLEW_RATE_FAST);
    gpio_set_drive_strength(CPU_RESET_PIN, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_slew_rate(PLL_BYPASS_PIN, GPIO_SLEW_RATE_FAST);
    gpio_set_drive_strength(PLL_BYPASS_PIN, GPIO_DRIVE_STRENGTH_12MA);
}

// Función para realizar glitching
void performGlitch() {
    // Configurar los parámetros de glitching (estos valores son solo ejemplos)
    uint32_t glitchWidth = 10;   // Ancho del pulso de glitch en ciclos de reloj
    uint32_t glitchDelay = 100;  // Retraso antes del glitch en ciclos de reloj

    // Realizar glitching manipulando un pin GPIO específico
    gpio_put(PLL_BYPASS_PIN, 1);  // Activa el pin PLL_BYPASS_PIN

    // Aplicar el glitch (puede variar según el objetivo)
    sleep_us(glitchDelay);       // Esperar el retraso antes del glitch
    gpio_put(PLL_BYPASS_PIN, 0);  // Desactivar el pin PLL_BYPASS_PIN

    // Realizar más operaciones de glitching si es necesario

    // Esperar a que se complete el glitch
    sleep_us(glitchWidth);
}


int main(void) {
    // Voltage min 0.85 (0b0110) y máx 1.30 (0b1111)
    vreg_set_voltage(VREG_VOLTAGE_1_30); 
    
    Establecer frecuencia del reloj de sistema en khz
    set_sys_clock_khz(266000, true);
    
    // Obtengo frecuencia del reloj de sistema, puede incluir índice como parámetro
    uint32_t freq = clock_get_hz(clk_sys);
    
    //CLOCKS_CLK_PERI_CTRL_AUXSRC_A.CLK_SYS 
    /*
    pub enum AUXSRC_A {
        CLK_SYS,
        CLKSRC_PLL_SYS,
        CLKSRC_PLL_USB,
        ROSC_CLKSRC_PH,
        XOSC_CLKSRC,
        CLKSRC_GPIN0,
        CLKSRC_GPIN1,
    }
    */
    // Vía simple para trabajar con el PLL_USB, cambiar (freq) por la requerida * MHZ
    // Comentar línea donde se obtiene la frequencia para modo manual
    clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS, freq, freq);

    // Configurar los pines GPIO
    configurePins();
    
    // loop infinito
    while (1) {
        // Esperar hasta que se cumpla una condición específica en el pin GPIO_TRIGGER_PIN 
        while (!gpio_get(GPIO_TRIGGER_PIN));

        // Encender el LED de depuración
        gpio_put(DEBUG_LED_PIN, 1);
        
        // Realizar glitching
        performGlitch();

        // Esperar un tiempo antes de continuar
        sleep_ms(200);

        // Realizar más operaciones si lo desea

        // Apagar el LED de depuración
        gpio_put(DEBUG_LED_PIN, 0);

        // Esperar hasta que se libere la condición en GPIO_TRIGGER_PIN
        while (gpio_get(GPIO_TRIGGER_PIN));
    }

    return 0;
}
