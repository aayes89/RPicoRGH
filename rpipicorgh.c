#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware/vreg.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "glitch.pio.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"

#define CPU_RESET_PIN 11
#define PLL_BYPASS_PIN 12
#define DEBUG_LED_PIN 25
#define GPIO_TRIGGER_PIN 15

// Función para medir frecuencias
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
    // Inicializar pines
    gpio_init(CPU_RESET_PIN);
    gpio_init(PLL_BYPASS_PIN);
    gpio_init(DEBUG_LED_PIN);
    gpio_init(GPIO_TRIGGER_PIN);
    
    // Configurar direcciones
    gpio_set_dir(CPU_RESET_PIN, GPIO_IN);  // Entrada
    gpio_set_dir(PLL_BYPASS_PIN, GPIO_OUT);  // Salida
    gpio_set_dir(DEBUG_LED_PIN, GPIO_OUT);  // Salida
    gpio_set_dir(GPIO_TRIGGER_PIN, GPIO_IN);  // Entrada
    
    // Configurar características de pines de salida
    gpio_set_slew_rate(PLL_BYPASS_PIN, GPIO_SLEW_RATE_FAST);
    gpio_set_drive_strength(PLL_BYPASS_PIN, GPIO_DRIVE_STRENGTH_12MA);
    
    // Si el pin CPU_RESET_PIN no requiere configuración de velocidad y fuerza, se omite
}

// Función para realizar glitching
void performGlitch(uint32_t glitchWidth, uint32_t glitchDelay) {
    // Activar el pin PLL_BYPASS_PIN
    gpio_put(PLL_BYPASS_PIN, 1);

    // Aplicar el glitch con el retraso especificado
    sleep_us(glitchDelay);
    gpio_put(PLL_BYPASS_PIN, 0);

    // Esperar el tiempo del ancho del glitch
    sleep_us(glitchWidth);
}

int main(void) {
    // Medir las frecuencias iniciales
    measure_freqs();
    
    // Configurar el voltaje del regulador
    vreg_set_voltage(VREG_VOLTAGE_1_30); 
    
    // Establecer la frecuencia del reloj del sistema en kHz
    set_sys_clock_khz(266000, true);
    
    // Obtener y usar la frecuencia del reloj del sistema
    uint32_t freq = clock_get_hz(clk_sys);
    
    // Configurar el reloj periférico para usar la frecuencia del reloj del sistema
    clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS, freq, freq);
    
    // Configurar los pines GPIO
    configurePins();
    
    // Parámetros para el glitching
    uint32_t glitchWidth = 10;   // Ancho del pulso de glitch en ciclos de reloj
    uint32_t glitchDelay = 100;  // Retraso antes del glitch en ciclos de reloj

    // Bucle infinito
    while (1) {
        // Esperar a que se cumpla la condición en GPIO_TRIGGER_PIN
        while (!gpio_get(GPIO_TRIGGER_PIN));

        // Encender el LED de depuración
        gpio_put(DEBUG_LED_PIN, 1);
        
        // Realizar glitching
        performGlitch(glitchWidth, glitchDelay);

        // Esperar un tiempo antes de continuar
        sleep_ms(200);

        // Apagar el LED de depuración
        gpio_put(DEBUG_LED_PIN, 0);

        // Esperar a que se libere la condición en GPIO_TRIGGER_PIN
        while (gpio_get(GPIO_TRIGGER_PIN));
    }

    return 0;
}
