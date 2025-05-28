void adc_init(void) {
    // Configura a tensão de referência para AVCC (5V)
    ADMUX = (1 << REFS0); 

    // Prescaler de 128 (Velocidade de conversão)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); 
}

uint16_t adc_read(void) {
    ADCSRA |= (1 << ADSC);              // Inicia a conversão
    while (!(ADCSRA & (1 << ADIF)));     // Espera a conversão terminar
    ADCSRA |= (1 << ADIF);               // Limpa a flag ADIF (escrevendo 1)

    uint16_t adc_result = ADCL;
    adc_result |= (ADCH << 8);           // Junta ADCL e ADCH
    return adc_result;
}

int main(void) {
    DDRD |= (1<<PD7);
    PORTD = 0;     
    uint16_t valorLido;
    unsigned long int aux; 
    unsigned int tensao_mV; // tensão em milivolts

    adc_init(); // Inicializa o ADC

    while (1) {
        valorLido = adc_read(); // Lê o valor binário do ADC (0-1023)

        aux = valorLido * 5000UL; // Multiplica para converter em milivolts
        aux = aux / 1023;         // Divide por 1023
        tensao_mV = (unsigned int) aux; // Faz o cast para uint

        // Agora, tensao_mV contém a tensão em mV (de 0 a 5000 mV)

        if (tensao_mV>=2000)
        {
          PORTD |= (1<<PD7);
        }else{
          PORTD =0;
        }
    }
}
