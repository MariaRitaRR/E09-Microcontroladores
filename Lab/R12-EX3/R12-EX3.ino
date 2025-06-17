#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "i2c.h"
#include "lcd_i2c.h"

// Definições de pinos
#define LED_QUARTO     PB0
#define LED_SALA       PB1
#define LED_COZINHA    PB2
#define BOTAO_TEMPO    PD2  // INT0
#define BOTAO_EMERG    PD3  // INT1
#define SENSOR_GAS     PC0
#define SERVO_PORTA    PD5  // OC0B
#define SERVO_JANELA   PD6  // OC0A

// Estados do sistema
volatile uint8_t emergencia = 0;
volatile uint8_t timer_ligado = 0;
volatile uint16_t tempo_restante = 0;
volatile uint8_t gas_detectado = 0;

// Variáveis para data e hora (inicializadas com valores padrão)
volatile uint8_t horas = 12;
volatile uint8_t minutos = 0;
volatile uint8_t segundos = 0;
volatile uint8_t dia = 16;
volatile uint8_t mes = 6;
volatile uint16_t ano = 2025;

// Protótipos de função
void init_ports(void);
void init_timer0_pwm(void);
void init_timer1_1hz(void);
void init_interrupts(void);
void update_lcd(void);
void serial_init(void);
void serial_send(char *data);
char serial_receive(void);
void handle_serial_command(char cmd);
void set_luminosidade_automatica(void);
void process_time_input(char *input);
void set_datetime(uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t mo, uint16_t y);

// Buffer para entrada serial
char serial_buffer[32];
uint8_t buffer_index = 0;

int main(void) {
    // Inicializações
    init_ports();
    init_timer0_pwm();
    init_timer1_1hz();
    init_interrupts();
    lcd_init();
    serial_init();
    
    // Mensagem inicial
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Aguardando hora");
    lcd_set_cursor(0, 1);
    lcd_print("Envie: HH:MM:SS");
    serial_send("Sistema iniciado. Envie a hora no formato HH:MM:SS\r\n");
    serial_send("Ou data+hora no formato DD/MM/YYYY HH:MM:SS\r\n");
    
    // Loop principal
    while(1) {
        // Verifica comunicação serial
        if(UCSR0A & (1<<RXC0)) {
            char received = serial_receive();
            
            // Processa entrada de hora/data
            if(received == '\r' || received == '\n') {
                serial_buffer[buffer_index] = '\0'; // Termina a string
                process_time_input(serial_buffer);
                buffer_index = 0;
            } else if(buffer_index < sizeof(serial_buffer)-1) {
                serial_buffer[buffer_index++] = received;
            }
        }
        
        // Atualiza display
        update_lcd();
        
        // Lógica de emergência (prioridade máxima)
        if(emergencia) {
            // Desliga todas as luzes
            PORTB &= ~((1<<LED_QUARTO) | (1<<LED_SALA) | (1<<LED_COZINHA));
            // Fecha portas e janelas
            OCR0A = 5;   // Janela fechada (5% duty cycle)
            OCR0B = 5;   // Porta fechada (5% duty cycle)
            continue;
        }
        
        // Lógica de detecção de gás
        if(ADC > 512 || gas_detectado) {
            gas_detectado = 1;
            // Fecha portas e janelas
            OCR0A = 5;   // Janela fechada
            OCR0B = 5;   // Porta fechada
            // Pisca LED da cozinha (alerta)
            if(segundos % 2 == 0) {
                PORTB ^= (1<<LED_COZINHA);
            }
        }
        
        // Lógica do temporizador do quarto
        if(timer_ligado) {
            if(tempo_restante > 0) {
                PORTB |= (1<<LED_QUARTO);  // Liga LED do quarto
            } else {
                PORTB &= ~(1<<LED_QUARTO);  // Desliga LED do quarto
                timer_ligado = 0;
            }
        }
        
        // Controle automático de luminosidade baseado na hora
        set_luminosidade_automatica();
        
        _delay_ms(100);
    }
    
    return 0;
}

// Configura luminosidade baseada na hora do dia
void set_luminosidade_automatica(void) {
    static uint8_t last_hour = 255;
    
    // Só atualiza quando a hora muda
    if(horas != last_hour) {
        last_hour = horas;
        
        // Manhã (6h-11h) - luminosidade aumenta gradualmente
        if(horas >= 6 && horas < 12) {
            uint8_t intensity = (horas - 6) * 51; // 6h=0, 11h=255
            OCR1A = intensity; // PWM para LED da sala
            OCR1B = intensity; // PWM para LED da cozinha
        }
        // Tarde (12h-17h) - máxima luminosidade
        else if(horas >= 12 && horas < 18) {
            OCR1A = 255;
            OCR1B = 255;
        }
        // Noite (18h-23h) - luminosidade diminui gradualmente
        else if(horas >= 18 && horas < 24) {
            uint8_t intensity = 255 - ((horas - 18) * 42); // 18h=255, 23h=45
            OCR1A = intensity;
            OCR1B = intensity;
        }
        // Madrugada (0h-5h) - luzes mínimas ou desligadas
        else {
            OCR1A = 30; // Luz noturna mínima
            OCR1B = 30;
        }
    }
}

// Processa entrada de hora/data
void process_time_input(char *input) {
    uint8_t h, m, s, d, mo;
    uint16_t y;
    
    // Tenta ler formato HH:MM:SS
    if(sscanf(input, "%hhu:%hhu:%hhu", &h, &m, &s) == 3) {
        if(h < 24 && m < 60 && s < 60) {
            set_datetime(h, m, s, dia, mes, ano);
            serial_send("Hora configurada com sucesso!\r\n");
        } else {
            serial_send("Hora inválida! Use HH:MM:SS\r\n");
        }
    }
    // Tenta ler formato DD/MM/YYYY HH:MM:SS
    else if(sscanf(input, "%hhu/%hhu/%hu %hhu:%hhu:%hhu", &d, &mo, &y, &h, &m, &s) == 6) {
        if(d > 0 && d < 32 && mo > 0 && mo < 13 && y >= 2023 && 
           h < 24 && m < 60 && s < 60) {
            set_datetime(h, m, s, d, mo, y);
            serial_send("Data e hora configuradas com sucesso!\r\n");
        } else {
            serial_send("Data/hora inválida! Use DD/MM/YYYY HH:MM:SS\r\n");
        }
    } else {
        serial_send("Formato inválido! Use HH:MM:SS ou DD/MM/YYYY HH:MM:SS\r\n");
    }
}

// Atualiza data e hora
void set_datetime(uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t mo, uint16_t y) {
    horas = h;
    minutos = m;
    segundos = s;
    dia = d;
    mes = mo;
    ano = y;
    
    // Atualiza display imediatamente
    update_lcd();
}

// [As outras funções permanecem como no código original...]