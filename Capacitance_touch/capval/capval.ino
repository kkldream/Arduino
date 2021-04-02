int ledPin = 13;
int capval;
void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(250000);
  Serial.println("Touch senser");
}

void loop ()
{
  digitalWrite(ledPin, LOW);
  capval = readCapacitivePin(A0);
  Serial.println(capval, DEC);
  if (capval > 5) {
    digitalWrite(ledPin, HIGH);
    delay(10);
  }
}

uint8_t readCapacitivePin(int pinToMeasure) {
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  volatile uint8_t* pin;
  byte bitmask;
  port = portOutputRegister(digitalPinToPort(pinToMeasure));
  ddr = portModeRegister(digitalPinToPort(pinToMeasure));
  bitmask = digitalPinToBitMask(pinToMeasure);
  pin = portInputRegister(digitalPinToPort(pinToMeasure));
  *port &= ~(bitmask);
  *ddr |= bitmask;
  delay(1);
  *ddr &= ~(bitmask);
  *port |= bitmask;

  uint8_t cycles = 17;
  
  if (*pin & bitmask) cycles = 0;
  else if (*pin & bitmask) cycles = 1;
  else if (*pin & bitmask) cycles = 2;
  else if (*pin & bitmask) cycles = 3;
  else if (*pin & bitmask) cycles = 4;
  else if (*pin & bitmask) cycles = 5;
  else if (*pin & bitmask) cycles = 6;
  else if (*pin & bitmask) cycles = 7;
  else if (*pin & bitmask) cycles = 8;
  else if (*pin & bitmask) cycles = 9;
  else if (*pin & bitmask) cycles = 10;
  else if (*pin & bitmask) cycles = 11;
  else if (*pin & bitmask) cycles = 12;
  else if (*pin & bitmask) cycles = 13;
  else if (*pin & bitmask) cycles = 14;
  else if (*pin & bitmask) cycles = 15;
  else if (*pin & bitmask) cycles = 16;
  
  *port &= ~(bitmask);
  *ddr |= bitmask;
  return cycles;
}
