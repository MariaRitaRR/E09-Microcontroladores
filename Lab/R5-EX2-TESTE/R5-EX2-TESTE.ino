#define BOTAO1 (1<<PD7)
#define BOTAO2 (1<<PD6)
#define BOTAO3 (1<<PD5)

#define LED1 (1 << PB1)  // LED do motor M1 agora no pino PB1
#define LED2 (1 << PB2)  // LED do motor M2 agora no pino PB2
#define LED3 (1 << PB3)  // LED do motor M3 agora no pino PB3

#define POTENCIA_M1 30
#define POTENCIA_M2 50
#define POTENCIA_M3 70

int main(void) {
    // Definir botões como entrada com pull-up
    DDRD &= ~(BOTAO1 | BOTAO2 | BOTAO3);
    PORTD |= (BOTAO1 | BOTAO2 | BOTAO3);

    // Definir LEDs como saída e iniciar desligados
    DDRB |= (LED1 | LED2 | LED3);
    PORTB &= ~(LED1 | LED2 | LED3);

    for (;;) {
        int potenciaTotal = 0;
        int estadoA = !(PIND & BOTAO1);
        int estadoB = !(PIND & BOTAO2);
        int estadoC = !(PIND & BOTAO3);

        // Somar a potência dos motores ligados
        if (estadoA) potenciaTotal += POTENCIA_M1;
        if (estadoB) potenciaTotal += POTENCIA_M2;
        if (estadoC) potenciaTotal += POTENCIA_M3;

        // Se ultrapassar 90HP, desligar motores na ordem de menor potência primeiro
        if (potenciaTotal > 90) {
            if (estadoA) { // Desligar M1 primeiro (30HP)
                estadoA = 0;
                potenciaTotal -= POTENCIA_M1;
            }
            if (potenciaTotal > 90 && estadoB) { // Se ainda for maior que 90, desligar M2 (50HP)
                estadoB = 0;
                potenciaTotal -= POTENCIA_M2;
            }
            if (potenciaTotal > 90 && estadoC) { // Se ainda ultrapassar, desligar M3 (70HP)
                estadoC = 0;
                potenciaTotal -= POTENCIA_M3;
            }
        }

        // Atualizar os LEDs de acordo com os motores ligados
        //PORTB &= ~(LED1 | LED2 | LED3); // Apagar todos os LEDs antes de atualizar

        if (estadoA) PORTB |= LED1;
        if (estadoB) PORTB |= LED2;
        if (estadoC) PORTB |= LED3;
    }
}
