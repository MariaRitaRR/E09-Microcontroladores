
#define sensor1 PD1
#define sensor2 PB7
#define led1 PC5
#define led2 PB5
unsigned char j=0;

ISR(PCINT0_vect)
{
  PORTC &= ~(1<<led1);
  for(j=0;j<10;j++)
  {
  PORTB |= (1<<led2);
  _delay_ms(300);
  PORTB &= ~(1<<led2);
  _delay_ms(300);
  PORTB |= (1<<led2);
   
  }
}
ISR(PCINT2_vect)
{
  PORTC &= ~(1<<led1);
  
  for(j=0;j<10;j++)
  {
  PORTB |= (1<<led2);
  _delay_ms(600);
  PORTB &= ~(1<<led2);
  _delay_ms(600);
  PORTB |= (1<<led2);
   
  }
   
}
int main(){
  DDRD &= ~(1 << sensor1); // PD1 como entrada (sensor1)
  DDRB &= ~(1 << sensor2); // PB7 como entrada (sensor2)
  DDRB|=(1<<led2);
  DDRC|= (1<<led1);
  PORTC &= ~(1 << led1);
  PORTB &= ~(1 << led2);
  PCICR |= (1<<PCIE2)|(1<<PCIE0);
  PCMSK0 |= (1<<PCINT7);
  PCMSK2 |= (1<<PCINT17);
  sei();

  
  while(1){
    j=0;
    PORTC ^= (1<<led1);
    _delay_ms(5000);
    
  }
}
