/*Feito por:
 * Maria Rita Raposo Rosa, 2019, GEC
 * Anna Clara Almeida Pereira, 9778, GET
 */
int main(void) {
    // Configura PD6 (OC0A) como saída PWM
    DDRD |= (1 << PD6);
    
    // Configura PD2 como entrada com pull-up interno
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);   // Ativa pull-up interno
    
    // Timer0: Fast PWM, não-invertido, prescaler = 64
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00);
    
    while (1) {
        if (!(PIND & (1 << PD2))) { // Botão PRESSIONADO (LOW)
            _delay_ms(20);          // Debounce
            OCR0A = 128;            // LED 50% de brilho
        } else {                    // Botão SOLTO (HIGH)
            OCR0A = 0;              // LED apagado
        }
    }
}
