#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t led4_state = 0;
volatile uint32_t led4_off_time = 0;

ISR(INT0_vect) {
    led4_state = 1;
    led4_off_time = 0; // Usaremos um contador simples
    PORTD |= (1 << PD4); // Liga LED4 imediatamente
}

ISR(INT1_vect) {
    // Alterna a habilitação da INT0
    EIMSK ^= (1 << INT0);
    // Simples debounce para SimulIDE
    _delay_ms(30);
}

int main() {
    // 1. Configuração de portas
    DDRD |= (1 << PD4) | (1 << PD5); // PD4 e PD5 como saída
    
    // Pull-ups desabilitados (no SimulIDE os botões já tem resistores)
    PORTD &= ~((1 << PD2) | (1 << PD3));
    
    // 2. Configura interrupções
    // INT0 - borda de descida (mais confiável no simulador)
    // INT1 - borda de descida
    EICRA = (1 << ISC11) | (1 << ISC01);
    
    // Habilita ambas interrupções
    EIMSK = (1 << INT0) | (1 << INT1);
    
    // 3. Habilita interrupções globais
    sei();

    // 4. Variáveis para temporização
    uint16_t counter = 0;
    
    while(1) {
        // Pisca LED principal (PD5) a cada 500ms
        if(counter % 500 == 0) {
            PORTD ^= (1 << PD5);
        }
        
        // Controla LED4 (1 segundo ligado)
        if(led4_state) {
            if(++led4_off_time >= 1000) {
                PORTD &= ~(1 << PD4);
                led4_state = 0;
            }
        }
        
        _delay_ms(1);
        counter++;
        
        // Reset do contador para evitar overflow
        if(counter >= 60000) counter = 0;
    }
}
