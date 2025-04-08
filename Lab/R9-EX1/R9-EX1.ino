
int cont =0;
void ConfigTimer0(void)
{
  TCCR0B |= 0b00000101;
  TCCR0A |= 0b00000010;
  OCR0A = 250;
  TIMSK0|=(1<<OCIE0A);
}
void DiableTimer0(void)
{
  TCCR0B = 0;
}

ISR(TIMER0_COMPA_vect) //vetor de interrupção compa
{
  cont++;
  if(cont >= 12)
  {
    PORTD ^=(1<< PD7);
    cont=0;
  }
}
int main(void)
{

  DDRD |= (1<<PD7);
  PORTD |= (1<<PD7);
  ConfigTimer0();
  //DiableTimer0();
  sei();
  while(1)
  {
    
  }

  
}
