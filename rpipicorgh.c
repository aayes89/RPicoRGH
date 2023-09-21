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


void measure_freqs(void) {
    uint f_pll_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint f_pll_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    uint f_rosc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint f_clk_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
    uint f_clk_rtc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC);
 
    printf("pll_sys  = %dkHz\n", f_pll_sys);
    printf("pll_usb  = %dkHz\n", f_pll_usb);
    printf("rosc     = %dkHz\n", f_rosc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    printf("clk_peri = %dkHz\n", f_clk_peri);
    printf("clk_usb  = %dkHz\n", f_clk_usb);
    printf("clk_adc  = %dkHz\n", f_clk_adc);
    printf("clk_rtc  = %dkHz\n", f_clk_rtc);
}

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
    // Medir la frecuencia
    measure_freqs();
    
    // Voltage min 0.85 (0b0110) y máx 1.30 (0b1111)
    vreg_set_voltage(VREG_VOLTAGE_1_30); 
    
    Establecer frecuencia del reloj de sistema en khz
    set_sys_clock_khz(266000, true);
    
    // Obtengo frecuencia del reloj de sistema, puede incluir índice como parámetro
    uint32_t freq = clock_get_hz(clk_sys);
    
    // Liberar/des-inicializar el PLL especificado del RP2040
    // pll_sys: hasta 180MHZ y pll_usb: hasta 48MHZ
    // pll_deinit(pll_sys);
    
    // Trabajar con el PLL_USB, cambiar (freq) por la requerida * MHZ
    // Comentar línea donde se obtiene la frequencia para modo manual
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
