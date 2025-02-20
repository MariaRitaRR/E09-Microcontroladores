int main(void)
{
  DDRD = DDRD | 0b10000000; //Pino PD7 definido como saída
  PORTD = PORTD | 0b00100000; // Habilitar PULL-UP no PD5
  PORTD = PORTD & ~(0b10000000); //Desliga a saída PD7


  DDRD = DDRD | 0b01000000; //Pino PD6 definido como saída
  PORTD = PORTD | 0b00010000; // Habilitar PULL-UP no PD4
  PORTD = PORTD & ~(0b01000000); //Desliga a saída PD6


  for (;;)//Super Lopp
  {
    int botao1 = PIND & 0b00100000; //Le o estado de PD5
    int botao2 = PIND & 0b00010000; //Le o estado de PD4
    
    if (botao1 == 0)//Botão esta pressionado?
    {
      PORTD = PORTD | 0b10000000; //PD7 -> High
      _delay_ms(100);
      PORTD = PORTD & ~(0b10000000); ///PD7 -> LOW
      PORTD = PORTD | 0b01000000; //PD6 -> High
      _delay_ms(100);
      PORTD = PORTD & ~(0b01000000); ///PD6 -> LOW
      }
    if (botao2 == 0)//Botão esta pressionado?
    {
      PORTD = PORTD | 0b10000000; //PD7 -> High
      _delay_ms(1000);
      PORTD = PORTD & ~(0b10000000); ///PD7 -> LOW
      PORTD = PORTD | 0b01000000; //PD6 -> High
      _delay_ms(1000);
      PORTD = PORTD & ~(0b01000000); ///PD6 -> LOW
      
      }
    
    }
}
