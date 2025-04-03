unsigned char j;
ISR(INT0_vect)
{
  PORTC &= ~(1<<PC5);
  for(j =0; j < 20; j++)
  {
    PORTC ^= (1<<PC0);
    _delay_ms(200);
  }
  
}

int main(void){
  DDRC |=(1<<PC5);
  DDRC |=(1<<PC0);
  DDRD =0; //todos como entrada
  PORTC = 0; //led começão como 0
  
  EICRA = (1<<ISC00);//int na mudança de vivel logico
  EIMSK = (1<<INT0);//habilita a int 0
  
  sei();//liga o  serviço geral de int
  while(1){
 
    if(PIND&(1<<PD1)){
      PORTC |= (1<<PC5);
      _delay_ms(7000);
      PORTC &=~(1<<PC5);
    }
     
  }
}
