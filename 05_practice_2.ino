int PIN_LED = 7;
int toggle = 0;
int count = 0;

void setup() {
  pinMode(PIN_LED,OUTPUT);
}

void loop() {
  digitalWrite(PIN_LED,0);
  delay(1000);

  while(count<10) {
    toggle =! toggle;
    digitalWrite(PIN_LED,toggle);
    delay(100);
    count++;
  }

  digitalWrite(PIN_LED,1);
  while(1) {
    
  }
}
