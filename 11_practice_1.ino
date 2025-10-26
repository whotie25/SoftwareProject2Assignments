#include <Servo.h>

// Arduino pin assignment
#define PIN_LED   9   // LED active-low
#define PIN_TRIG  12  // sonar sensor TRIGGER
#define PIN_ECHO  13  // sonar sensor ECHO
#define PIN_SERVO 10  // servo motor

// configurable parameters for sonar
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 180.0
#define _DIST_MAX 360.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

#define _EMA_ALPHA 0.3

#define _DUTY_MIN 300
#define _DUTY_NEU 1200
#define _DUTY_MAX 2500

// global variables
float  dist_ema, dist_prev = _DIST_MAX; // unit: mm
float dist_ema_prev = 0;
unsigned long last_sampling_time;       // unit: ms

Servo myservo;

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);    // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);     // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar

  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);

  // initialize USS related variables
  dist_prev = _DIST_MIN; // raw distance output from USS (unit: mm)

  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float  dist_raw, dist_filtered;
  
  if (millis() < last_sampling_time + INTERVAL)
    return;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // range filter
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
      dist_filtered = _DIST_MAX;
  } else if (dist_raw < _DIST_MIN) {
      dist_filtered = _DIST_MIN;
  } else {    // In desired Range
      dist_filtered = dist_raw;
      dist_prev = dist_raw;
  }

  // ema filter
  dist_ema = dist_filtered * _EMA_ALPHA + dist_ema_prev * (1 - _EMA_ALPHA);
  dist_ema_prev = dist_ema;
  

  // adjust servo position according to the USS read value
  if(dist_ema <= 180.0 || dist_ema >= 360.0){
    if(dist_ema <= 180.0) myservo.writeMicroseconds(_DUTY_MIN);
    else myservo.writeMicroseconds(_DUTY_MAX);
  }
  else {
    float tmp = dist_ema - 180.0;
    myservo.writeMicroseconds(_DUTY_MIN + (_DUTY_MAX - _DUTY_MIN)*tmp/180.0);
  }

  if(dist_raw > 180.0 && dist_raw < 360.0) digitalWrite(PIN_LED, 0);
  else digitalWrite(PIN_LED, 1);

  // serial output
  Serial.print("Min:");     Serial.print(_DIST_MIN);
  Serial.print(",dist:");   Serial.print(dist_raw);
  Serial.print(",ema:");    Serial.print(dist_ema);
  Serial.print(",Servo:");  Serial.print(myservo.read());  
  Serial.print(",Max:");    Serial.print(_DIST_MAX);
  Serial.println("");

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
