#define PIN_LED 13
unsigned int cnt;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  Serial.println("Serial port connected!");
  cnt = 0;
  digitalWrite(PIN_LED, cnt);
}

void loop() {
  Serial.println(++cnt);
  digitalWrite(PIN_LED, cnt%2);
  delay(1000);
}
