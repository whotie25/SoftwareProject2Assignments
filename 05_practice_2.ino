#define PIN_LED 7

void setup() {
  pinMode(PIN_LED, OUTPUT);
}

void loop() {
  digitalWrite(PIN_LED, 0);
  delay(1000);
  digitalWrite(PIN_LED, 1);
  delay(100);
  for(int i = 0; i < 10; i++){
    digitalWrite(PIN_LED, i%2);
    delay(100);
  }
  
  digitalWrite(PIN_LED, 1);

  while(1){
    
  }
  
}
