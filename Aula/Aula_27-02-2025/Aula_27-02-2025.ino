#define VERMELHO PC5
#define AMARELO PC1
#define VERDE PB5
int main(){
  
  DDRD = DDRD | 0; //TODOS OS PINOS DO PORTD COMO ENTRADAS
  DDRB = DDRB | 0b00100010;//PB1 E PB5 COMO SAÍDAS
  DDRC = DDRC | 0b00100010;//PC1 E PC5 COMO SAÍDAS

  while(1){
    PORTB = 0b00100000; 
    PORTC = 0b00000010;
    _delay_ms(500);
    PORTC = 0b00100000; 
    PORTB = 0b00000010;
    _delay_ms(500);
  }
  
}
