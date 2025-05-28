


int main(void){
    DDRC= 255; //PORTAL C COMO SAIDA
    DDRD = 0; //PORTAL D COMO ENTRADA
    TCCR0A = 0;
    TCCR0B |= (1<<CS02) | (1<<CS01) | (1<<CS00);
    //PORTC = 0;
    while(1){
      
      if(TCNT0 == 255)
      {
        PORTC ^= 255; //LIGA O LED
        TCNT0 = 0; //REINICIA O CONTADORS
      }
    }
}
