#include <avr/io.h>
#include <util/delay.h>

#define LED1 (1 << PD7)
#define LED2 (1 << PD6)
#define BOTAO1 (1 << PD5)
#define BOTAO2 (1 << PD4)

int main(void)
{
    // Configura LEDs como saída
    DDRD |= (LED1 | LED2);
    PORTD &= ~(LED1 | LED2); // Inicializa LEDs desligados

    // Configura botões como entrada com pull-up ativado
    DDRD &= ~(BOTAO1 | BOTAO2); // Configura pinos como entrada
    PORTD |= (BOTAO1 | BOTAO2); // Ativa pull-ups internos

    for (;;)
    {
        // Se BOTAO1 for pressionado, acende LED1 por 1 segundo
        if (!(PIND & BOTAO2))
        {
            PORTD |= LED1;  // Acende LED1
            _delay_ms(1000); // Mantém aceso por 1 segundo
            PORTD &= ~LED1;  // Apaga LED1
            PORTD |= LED2;  // Acende LED2
             _delay_ms(1000); // Mantém aceso por 1 segundo
            PORTD &= ~LED2;  // Apaga LED2
        }

        // Se BOTAO2 for pressionado, acende LED2 por 1 segundo
        if (!(PIND & BOTAO1))
           PORTD |= LED1;  // Acende LED1
            _delay_ms(100); // Mantém aceso por 1 segundo
            PORTD &= ~LED1;  // Apaga LED1
            PORTD |= LED2;  // Acende LED2
             _delay_ms(100); // Mantém aceso por 1 segundo
            PORTD &= ~LED2;  // Apaga LED2
        }
    }
}
