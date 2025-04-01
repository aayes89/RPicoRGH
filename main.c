#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "glitch.new.pio"  // Archivo generado desde el código PIO

// Configuración de pines
#define CPU_RESET_PIN    11
#define GLITCH_OUT_PIN   12
#define DEBUG_LED_PIN    25

// Parámetros del glitch
#define GLITCH_DELAY_CYCLES  20
#define GLITCH_WIDTH_CYCLES  3

volatile bool trigger_glitch = false;
PIO glitch_pio = pio0;
uint glitch_sm = 0;

// Interrupción para detectar el reset
void gpio_irq_handler(uint gpio, uint32_t events) {
    if (gpio == CPU_RESET_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
        trigger_glitch = true;
    }
}

// Inicializar PIO
void init_glitch_pio() {
    uint offset = pio_add_program(glitch_pio, &glitch_program);
    pio_sm_config cfg = glitch_program_get_default_config(offset);
    
    sm_config_set_out_pins(&cfg, GLITCH_OUT_PIN, 1);
    sm_config_set_clkdiv(&cfg, 1.0);  // 125 MHz → 8 ns/ciclo
    
    pio_sm_init(glitch_pio, glitch_sm, offset, &cfg);
    pio_sm_set_enabled(glitch_pio, glitch_sm, true);
}

int main() {
    // Inicializar hardware (sin stdio)
    gpio_init(CPU_RESET_PIN);
    gpio_init(GLITCH_OUT_PIN);
    gpio_init(DEBUG_LED_PIN);
    
    gpio_set_dir(CPU_RESET_PIN, GPIO_IN);
    gpio_set_dir(GLITCH_OUT_PIN, GPIO_OUT);
    gpio_set_dir(DEBUG_LED_PIN, GPIO_OUT);
    
    // Configurar interrupción
    gpio_set_irq_enabled_with_callback(CPU_RESET_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    
    // Iniciar PIO
    init_glitch_pio();
    
    // Bucle principal
    while(1) {
        if(trigger_glitch) {
            gpio_put(DEBUG_LED_PIN, 1);
            pio_sm_put_blocking(glitch_pio, glitch_sm, GLITCH_DELAY_CYCLES);
            pio_sm_put_blocking(glitch_pio, glitch_sm, GLITCH_WIDTH_CYCLES);
            gpio_put(DEBUG_LED_PIN, 0);
            trigger_glitch = false;
        }
        __wfi();
    }
}
