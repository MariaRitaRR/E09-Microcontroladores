#define temperatura PD2
#define pressao PD4
#define nivel PD5
#define peso PB0
#define led PD0
int main(){
  DDRD = 0; //TODOS COMO ENTRADA
  DDRD|= (1<<PD0); //SOMENTE PD0 COMO SAÍDA
  PORTD = 0; //TODOS OS PINOS PD0 INICIAM DESLIGADOS
  DDRB=0; //TODOS COMO ENTRADA

  while(1){
    PORTD=0;
    //Nivel alto com temperatura alta e pressao alta

    while((PIND&(1<<temperatura))&&(PIND&(1<<nivel)) && (PIND&(1<<pressao)))
    {

      PORTD|=(1<<led);
    }
    //Nível baixo temperatura alta e peso alto
    while((PIND&(1<<temperatura))&&(PINB&(1<<peso)) && !(PIND&(1<<nivel)))
    {
      PORTD|=(1<<led);
    }
    //Nível baixo com temperatura baixa e pressão alta
    while(!(PIND&(1<<temperatura))&& !(PIND&(1<<nivel)) && (PIND&(1<<pressao)))
    {
      PORTD|=(1<<led);
    }
    //nivel baixo com peso baixo e temperatura alta
     while((PIND&(1<<temperatura))&& !(PINB&(1<<peso)) && !(PIND&(1<<nivel)))
    {
      PORTD|=(1<<led);
    }
    }
}
