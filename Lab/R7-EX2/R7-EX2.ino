#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t int0_enabled = 1;

ISR(INT0_vect) {
    // Acende o LED no PD4
    PORTD |= (1 << PD4);
    _delay_ms(1000);
    // Apaga o LED no PD4
    PORTD &= ~(1 << PD4);
}

ISR(INT1_vect) {
    // Alterna o estado de INT0
    int0_enabled = !int0_enabled;
    if(int0_enabled) {
        EIMSK |= (1 << INT0);  // Habilita INT0
    } else {
        EIMSK &= ~(1 << INT0); // Desabilita INT0
    }
}
int main (void)
{
  //Configura PD5 ePD4 como saida
  DDRD |= (1<<PD5) | (1<<PD4);

  //pull up em PD2
  PORTD |=(1<<PD2) |(1<< PD1);

  //Configura INTO para transição de subida
  EICRA = (1<<ISC01) | (1<<ISC00);

  //habilita into
  EIMSK |=(1 << INT0);

  //acianar função q habilita as interrupções
  sei();

  while(1){

    if(EIFR &(1<<INTF0))
    {
      EIFR |= (INTF0);

      PORTD |= (1 << PD4);
      _delay_ms(1000);

      PORTD &= ~(1 << PD4);
    }
     // Pisca LED no PD5 a cada 0,5 segundos
        PORTD ^= (1 << PD5);
        _delay_ms(500);
  }
}
