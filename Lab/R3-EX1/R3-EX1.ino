void setup() {
  DDRD = 0b00001110;
  DDRB = 0b00001110;

}

void loop() {
  PORTB = PORTB | 0b00001110;

}
