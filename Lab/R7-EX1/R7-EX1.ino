ISR(INT0_vect){

   PORTD ^= (1<<PD4);
   _delay_ms(1000);
   PORTD ^= (1<<PD4);
  
}
int main(){

DDRD |= (1<<PD4)|(1 << PD5);
//erro talvez aqui
PORTD &= ~(1<<PD2);
EICRA = 0b00000011;
EIMSK |= (1<<INT0);


sei();

while(1){

  PORTD ^= (1<<PD5);
  _delay_ms(500);
  
}

  
}void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
