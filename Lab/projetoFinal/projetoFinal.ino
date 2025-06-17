#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

// ================= DEFINIÇÕES =================
#define LED_PWM         PD6     // Pino D6 (OC0A PWM)
#define LED_ESPELHO     PD7     // Pino D7 (PWM software)
#define LED_ALERTA      PD4     // LED de alerta de gás
#define LED_QUARTO      PD5     // LED do quarto
#define LIMIAR_GAS      300     // Valor ADC para acionar alerta
#define BAUDRATE        9600
#define UBRR_VAL        ((F_CPU/16/BAUDRATE)-1)

// ================= VARIÁVEIS GLOBAIS =================
volatile uint8_t hora = 0, minuto = 0;
volatile uint8_t alerta_gas = 0;
volatile uint8_t quarto_ligado = 0;
volatile uint16_t contador_timer = 0;
volatile uint8_t dado_serial_pronto = 0;
char buffer_serial[16];
uint8_t indice_buffer = 0;
uint16_t ultimo_valor_gas = 0;
uint8_t contador_estabilidade = 0;

// ================= PROTÓTIPOS =================
void InicializarSistema();
void ProcessarSerial();
void AtualizarLEDs();
uint16_t LerADC(uint8_t canal);
void InicializarUART();
void EnviarUART(char dado);
void EnviarTextoUART(const char* texto);
void EnviarNumeroUART(uint16_t numero);
void VerificarSensorGas();

// ================= IMPLEMENTAÇÕES =================

uint16_t LerADC(uint8_t canal) {
    ADMUX = (ADMUX & 0xF0) | (canal & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

// ================= INTERRUPÇÕES =================
ISR(TIMER1_COMPA_vect) {
    if (quarto_ligado && (++contador_timer >= 5000)) {
        quarto_ligado = 0;
        contador_timer = 0;
    }
}

ISR(INT0_vect) {
    alerta_gas = !alerta_gas;
}

ISR(INT1_vect) {
    quarto_ligado = 1;
    contador_timer = 0;
}

ISR(USART_RX_vect) {
    char caractere = UDR0;
    if (caractere == '\n' || caractere == '\r') {
        buffer_serial[indice_buffer] = '\0';
        indice_buffer = 0;
        dado_serial_pronto = 1;
    } 
    else if (indice_buffer < sizeof(buffer_serial)-1) {
        buffer_serial[indice_buffer++] = caractere;
    }
}

// ================= INICIALIZAÇÃO =================
void InicializarSistema() {
    // Configuração de portas
    DDRD |= (1 << LED_PWM) | (1 << LED_ESPELHO) | (1 << LED_ALERTA) | (1 << LED_QUARTO);
    PORTD &= ~((1 << LED_PWM) | (1 << LED_ESPELHO) | (1 << LED_ALERTA) | (1 << LED_QUARTO));
    
    // Configuração PWM
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS00);
    OCR0A = 0;
    
    // Timer1 (1ms)
    TCCR1B = (1 << WGM12) | (1 << CS11);
    OCR1A = 1999;
    TIMSK1 = (1 << OCIE1A);
    
    // Interrupções externas
    EICRA = (1 << ISC11) | (1 << ISC01);
    EIMSK = (1 << INT1) | (1 << INT0);
    
    // ADC
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    // UART com interrupção
    InicializarUART();
    UCSR0B |= (1 << RXCIE0);
    
    sei(); // Habilita interrupções globais
}

void InicializarUART() {
    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t)UBRR_VAL;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// ================= FUNÇÕES UART =================
void EnviarUART(char dado) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = dado;
}

void EnviarTextoUART(const char* texto) {
    while (*texto) EnviarUART(*texto++);
}

void EnviarNumeroUART(uint16_t numero) {
    char buffer[6];
    itoa(numero, buffer, 10);
    EnviarTextoUART(buffer);
}

// ================= MONITORAMENTO DE GÁS =================
void VerificarSensorGas() {
    static uint8_t ultimo_estado_alerta = 0;
    static uint16_t ultimo_valor_estavel = 0;
    const uint16_t limite_variacao = 5;
    
    uint16_t valor_atual = LerADC(0);
    
    // Verifica variação significativa
    if (abs(valor_atual - ultimo_valor_gas) > limite_variacao) {
        contador_estabilidade = 0;
    } 
    else {
        if (contador_estabilidade < 10) contador_estabilidade++;
    }
    
    // Atualiza estado do alerta
    uint8_t novo_estado_alerta = (valor_atual >= LIMIAR_GAS);
    
    // Se o estado mudou
    if (novo_estado_alerta != ultimo_estado_alerta) {
        EnviarTextoUART("Alerta: ");
        EnviarTextoUART(novo_estado_alerta ? "GÁS DETECTADO! " : "Nível normal ");
        EnviarTextoUART("Valor: ");
        EnviarNumeroUART(valor_atual);
        EnviarTextoUART("\r\n");
        ultimo_estado_alerta = novo_estado_alerta;
        alerta_gas = novo_estado_alerta;
    }
    
    // Reporta valores estáveis
    if (contador_estabilidade >= 10) {
        if (abs(valor_atual - ultimo_valor_estavel) > limite_variacao) {
            EnviarTextoUART("Nível estável: ");
            EnviarNumeroUART(valor_atual);
            EnviarTextoUART("\r\n");
            ultimo_valor_estavel = valor_atual;
        }
    }
    
    ultimo_valor_gas = valor_atual;
}

// ================= PROCESSAMENTO SERIAL =================
void ProcessarSerial() {
    if (!dado_serial_pronto) return;
    
    // Formato esperado: "HH:MM"
    char* ponteiro = buffer_serial;
    hora = atoi(ponteiro);
    
    while (*ponteiro && *ponteiro != ':') ponteiro++;
    if (*ponteiro == ':') minuto = atoi(ponteiro+1);
    
    EnviarTextoUART("Hora configurada: ");
    EnviarNumeroUART(hora);
    EnviarTextoUART(":");
    EnviarNumeroUART(minuto);
    EnviarTextoUART("\r\n");
    
    dado_serial_pronto = 0;
}

// ================= CONTROLE DE LEDs =================
void AtualizarLEDs() {
    static uint16_t contador_pwm = 0;
    
    // Determina brilho baseado na hora
    uint8_t brilho;
    if (hora >= 6 && hora < 18) brilho = 255;     // Dia (máximo)
    else if (hora >= 18 && hora < 22) brilho = 128; // Tarde (metade)
    else brilho = 64;                              // Noite (baixo)
    
    // PWM hardware (D6)
    OCR0A = brilho;
    
    // PWM software (D7)
    if (++contador_pwm >= 255) contador_pwm = 0;
    if (contador_pwm < brilho) PORTD |= (1 << LED_ESPELHO);
    else PORTD &= ~(1 << LED_ESPELHO);
    
    // LEDs de alerta e quarto
    PORTD = (PORTD & ~(1 << LED_ALERTA)) | (alerta_gas << LED_ALERTA);
    PORTD = (PORTD & ~(1 << LED_QUARTO)) | (quarto_ligado << LED_QUARTO);
}

// ================= PROGRAMA PRINCIPAL =================
int main() {
    InicializarSistema();
    EnviarTextoUART("Sistema pronto. Envie horário no formato HH:MM\r\n");
    
    while (1) {
        ProcessarSerial();
        VerificarSensorGas();
        AtualizarLEDs();
        _delay_ms(100);
    }
}