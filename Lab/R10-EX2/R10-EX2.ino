#include <avr/io.h>
#include <util/delay.h>

#define MAX_BRIGHTNESS 255
#define BRIGHTNESS_STEP 25  // ~10% de 255

int main(void) {
    // Configura PD6 (OC0A) como saída PWM
    DDRD |= (1 << PD6);
    
    // Configura PD2 como entrada com pull-up interno
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);
    
    // Timer0: Fast PWM, não-invertido, prescaler = 64
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00);
    
    uint8_t brightness = 0;
    uint8_t last_button_state = 1; // Começa com botão solto (HIGH)
    OCR0A = 0; // Inicia com LED apagado
    
    while (1) {
        uint8_t current_button_state = PIND & (1 << PD2);
        
        // Detecta borda de descida (botão pressionado)
        if (last_button_state && !current_button_state) {
            _delay_ms(20); // Debounce
            
            // Só incrementa se o botão ainda estiver pressionado após debounce
            if (!(PIND & (1 << PD2))) {
                brightness += BRIGHTNESS_STEP;
                
                if (brightness > MAX_BRIGHTNESS) {
                    brightness = 0;
                }
                
                OCR0A = brightness;
            }
            
            // Espera soltar o botão
            while (!(PIND & (1 << PD2)));
            _delay_ms(20);
        }
        
        last_button_state = current_button_state;
    }
}
