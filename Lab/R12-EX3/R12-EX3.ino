#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)

// Definições para o PWM
#define PWM_PIN PD6  // OC0A - Pino do PWM (pino 6 no Arduino)
#define LED_VERDE PD6

// Buffer para receber mensagens via UART
char buffer[4];  // Suficiente para "100" + null terminator
uint8_t buffer_pos = 0;

// Protótipos de função
void UART_init(unsigned int ubrr);
void UART_Transmit(char *dados);
void PWM_init(void);
void set_PWM_duty(uint8_t duty);

int main(void) {
    // Inicializa a UART
    UART_init(MYUBRR);
    
    // Inicializa o PWM
    PWM_init();
    
    // Configura o pino do LED como saída
    DDRD |= (1 << LED_VERDE);
    
    // Habilita interrupções globais
    sei();
    
    // Mensagem inicial
    UART_Transmit("Digite o duty cycle (0-100):\n");
    
    while (1) {
        // O processamento principal é feito na interrupção de recepção UART
    }
}

// Inicialização do PWM no Timer0 (8-bit)
void PWM_init(void) {
    // Configura o pino OC0A (PD6) como saída
    DDRD |= (1 << PWM_PIN);
    
    // Modo Fast PWM, top em 0xFF
    TCCR0A |= (1 << WGM01) | (1 << WGM00);
    
    // Limpa OC0A no compare match, seta no BOTTOM (modo não-invertido)
    TCCR0A |= (1 << COM0A1);
    
    // Prescaler de 64 (CS02 = 0, CS01 = 1, CS00 = 1)
    TCCR0B |= (1 << CS01) | (1 << CS00);
    
    // Duty cycle inicial 0%
    OCR0A = 0;
}

// Ajusta o duty cycle do PWM (0-100%)
void set_PWM_duty(uint8_t duty) {
    if (duty > 100) duty = 100;  // Limita a 100%
    
    // Converte porcentagem para valor do registrador (0-255)
    OCR0A = (duty * 255) / 100;
}

// Rotina de transmissão UART
void UART_Transmit(char *dados) {
    while (*dados != 0) {
        while (!(UCSR0A & (1 << UDRE0)));  // Aguarda buffer vazio
        UDR0 = *dados;
        dados++;
    }
}

// Inicialização da UART
void UART_init(unsigned int ubrr) {
    // Configura baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    
    // Habilita receptor e transmissor
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    
    // Configura formato: 8 bits de dados, 1 bit de stop, sem paridade
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Interrupção de recepção UART
ISR(USART_RX_vect) {
    char received = UDR0;
    
    // Se for enter (CR ou LF), processa o comando
    if (received == '\r' || received == '\n') {
        if (buffer_pos > 0) {  // Se tem algo no buffer
            buffer[buffer_pos] = '\0';  // Termina a string
            
            // Converte para número
            uint8_t duty = 0;
            for (uint8_t i = 0; i < buffer_pos; i++) {
                if (buffer[i] >= '0' && buffer[i] <= '9') {
                    duty = duty * 10 + (buffer[i] - '0');
                }
            }
            
            // Limita a 100%
            if (duty > 100) duty = 100;
            
            // Ajusta o PWM
            set_PWM_duty(duty);
            
            // Confirmação
            UART_Transmit("Duty cycle ajustado para ");
            UART_Transmit(buffer);
            UART_Transmit("%\n");
            
            // Prepara para novo comando
            buffer_pos = 0;
            UART_Transmit("Digite novo duty cycle (0-100):\n");
        }
    } 
    // Se for backspace
    else if (received == '\b' || received == 127) {
        if (buffer_pos > 0) {
            buffer_pos--;
            // Envia backspace + espaço + backspace para apagar no terminal
            UART_Transmit("\b \b");
        }
    }
    // Se for dígito e ainda houver espaço no buffer
    else if (received >= '0' && received <= '9' && buffer_pos < 3) {
        buffer[buffer_pos++] = received;
        // Ecoa o caractere (opcional)
        UDR0 = received;
    }
}