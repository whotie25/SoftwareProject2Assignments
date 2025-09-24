#define PIN_LED 7

int highTerm, lowTerm;
int period, cycle;
int tmp = 0;
int change = 1;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  set_period(10000);
}

void loop() {
  set_duty((int)(tmp*(100.0/cycle)));

  //LED
  digitalWrite(PIN_LED, LOW);
  delayMicroseconds(highTerm);
  digitalWrite(PIN_LED, HIGH);
  delayMicroseconds(period - highTerm);

  tmp += change;
  if(tmp == cycle || tmp == 0) change *= -1;
}

void set_period(int _period){
  period = _period;
  cycle = 1000000 / (2 * period);
}

void set_duty(int duty){
  highTerm = (int)(period * (duty / 100.0));
}
