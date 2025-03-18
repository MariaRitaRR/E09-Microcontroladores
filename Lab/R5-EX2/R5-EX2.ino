#define BOTAO1 (1<<PD7)
#define BOTAO2 (1<<PD6)
#define BOTAO3 (1<<PD5)

#define LED1 (1 << PB1)  // Atualizando LED1 para PB1
#define LED2 (1 << PB2)  // Atualizando LED2 para PB2
#define LED3 (1 << PB3)  // Atualizando LED3 para PB3

#define POTENCIA_M1 30
#define POTENCIA_M2 50
#define POTENCIA_M3 70

int main(void) {

    // Defini botoes como entrada e coloquei como pull-up
    DDRD &= ~(BOTAO1 | BOTAO2 | BOTAO3);
    PORTD |= (BOTAO1 | BOTAO2 | BOTAO3);

    // Defini leds como saída e iniciei eles desativados
    DDRB |= (LED1 | LED2 | LED3);
    PORTB &= ~(LED1 | LED2 | LED3);
    int potenciaTotal = 0;
    
    for (;;) {
        int potenciaTotal = 0;
        int estadoA = !(PIND & BOTAO1);
        int estadoB = !(PIND & BOTAO2);
        int estadoC = !(PIND & BOTAO3);
            

        

        while (potenciaTotal < 90) {
            if (!(PIND & BOTAO1)) {
                PORTB |= LED1;  // Acende LED1 (PB1)
                potenciaTotal += POTENCIA_M1;   
            }
            if (!(PIND & BOTAO2)) {
                PORTB |= LED2;  // Acende LED2 (PB2)
                potenciaTotal += POTENCIA_M2;
            }
            if (!(PIND & BOTAO3)) {
                PORTB |= LED3;  // Acende LED3 (PB3)
                potenciaTotal += POTENCIA_M3;            }
        }
        while(potenciaTotal > 90) {
            if (!(PIND & BOTAO1)) {  // Desligar M1 primeiro (30HP)
                PORTB &= ~LED1;*+
                potenciaTotal -= POTENCIA_M1;
            }
            if (potenciaTotal > 90 && estadoB) {  // Se ainda for maior que 90, desligar M2 (50HP)
                PORTB &= ~LED2;
                potenciaTotal -= POTENCIA_M2;
            }
            if (potenciaTotal > 90 && estadoC) {  // Se ainda ultrapassar, desligar M3 (70HP)
                PORTB &= ~LED3;
                potenciaTotal -= POTENCIA_M3;
            }
        }

    }
}
