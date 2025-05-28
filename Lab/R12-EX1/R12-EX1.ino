#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Definições
#define FOSC 16000000UL // Clock Speed (16 MHz)
#define BAUD 9600
#define MYUBRR FOSC / 16 / BAUD - 1
#define BOTAO (1 << PD2) // Botão no PD2 (INT0)

// Variáveis globais
char msg_rx[32]; // Buffer de recepção
volatile uint8_t pos_msg_rx = 0;
volatile uint16_t contador = 0; // Contador de pressionamentos

// Protótipos de funções
void UART_Init(unsigned int ubrr);
void UART_Transmit(char *dados);
void itoa(uint16_t num, char* str);

// Função principal
int main(void) {
    // Inicializa UART
    UART_Init(MYUBRR);
    
    // Configura o botão (PD2 como entrada com pull-up)
    DDRD &= ~BOTAO;
    PORTD |= BOTAO;
    
    // Configura interrupção externa para borda de descida
    EICRA |= (1 << ISC01);  // Falling edge trigger
    EIMSK |= (1 << INT0);   // Habilita INT0
    
    // Habilita interrupções globais
    sei();
    
    UART_Transmit("Sistema iniciado. Pressione o botao.\n");
    UART_Transmit("Envie 'zerar' para resetar o contador.\n");
    
    // Super loop
    while(1) {
        // Verifica se recebeu comando "zerar"
        if (strncmp(msg_rx, "zerar", 5) == 0) {
            contador = 0;
            UART_Transmit("Contador zerado!\n");
            pos_msg_rx = 0; // Reseta o buffer de recepção
            memset(msg_rx, 0, sizeof(msg_rx));
        }
    }
}

// Interrupção do botão
ISR(INT0_vect) {
    _delay_ms(50); // Debounce
    if ((PIND & BOTAO) == 0) { // Verifica se ainda está pressionado
        contador++;
        
        char buffer[20];
        itoa(contador, buffer);
        
        UART_Transmit("Botao pressionado. Total: ");
        UART_Transmit(buffer);
        UART_Transmit("\n");
    }
}

// Interrupção de recepção UART
ISR(USART_RX_vect) {
    char received = UDR0;
    
    if (received == '\n' || received == '\r') {
        msg_rx[pos_msg_rx] = '\0'; // Finaliza string
        pos_msg_rx = 0;
    } else if (pos_msg_rx < sizeof(msg_rx)-1) {
        msg_rx[pos_msg_rx++] = received;
    }
}

// Inicialização da UART
void UART_Init(unsigned int ubrr) {
    // Configura baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    
    // Habilita receptor, transmissor e interrupção de recepção
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    
    // Configura formato: 8 bits de dados, 1 bit de stop
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Transmissão de string via UART
void UART_Transmit(char *dados) {
    while (*dados != 0) {
        while (!(UCSR0A & (1 << UDRE0))); // Aguarda buffer vazio
        UDR0 = *dados;
        dados++;
    }
}

// Conversão de inteiro para string (simplificado)
void itoa(uint16_t num, char* str) {
    int i = 0;
    
    if (num == 0) {
        str[i++] = '0';
    } else {
        while (num != 0) {
            str[i++] = (num % 10) + '0';
            num /= 10;
        }
    }
    
    str[i] = '\0';
    
    // Inverte a string
    int j = 0;
    i--;
    while (j < i) {
        char temp = str[j];
        str[j] = str[i];
        str[i] = temp;
        j++;
        i--;
    }
}