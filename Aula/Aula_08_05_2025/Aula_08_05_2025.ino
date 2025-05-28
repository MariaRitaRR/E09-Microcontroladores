#include <avr/io.h>
#include <avr/interrupt.h>
unsigned char cont =0;

ISR(TIMER0_OVF_vect) {
  cont ++;
  if(cont >122){
    PORTC ^= (1 << PC0);  // Alterna o LED no PC0
    cont =0;
  }
    
}

int main() {
    DDRC |= (1 << PC0);   // Configura PC0 como saída
    PORTC &= ~(1 << PC0); // Inicia desligado

    // Configura TIMER0 (Normal Mode, Prescaler = 1024)
    TCCR0A = 0;
    TCCR0B = (1 << CS02) | (1 << CS00); // Prescaler 1024
    TIMSK0 = (1 << TOIE0); // Habilita interrupção por overflow

    sei(); // Habilita interrupções globais

    while (1) { }
    return 0;
}