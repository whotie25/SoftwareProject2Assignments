#include <Servo.h>

// GPIO pin assignment
#define PIN_SERVO 10
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity (unit: m/sec)
#define INTERVAL 25
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 30
#define _DIST_MAX 300
#define _DIST_DETECT 70

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

#define _EMA_ALPHA 0.35

unsigned long last_sampling_time;   // unit: msec
float dist_prev = _DIST_MAX;
float dist_raw, dist_filtered, dist_ema;
boolean isOpen = false;

Servo myServo;

unsigned long MOVING_TIME = 1500; // moving time (unit:mm)
unsigned long moveStartTime;
int startAngle = 90;
int stopAngle  = 180;

void setup() {
  // initialize GPIO pins n' Serial
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  myServo.attach(PIN_SERVO);

  myServo.write(startAngle);
  delay(500);

  Serial.begin(57600);
}

void loop() {
  if (millis() < last_sampling_time + INTERVAL)
    return;

  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);

  // invalid data filter
  if(dist_raw == 0.0 || dist_raw > _DIST_MAX) dist_filtered = dist_prev;
  else if(dist_raw < _DIST_MIN) dist_filtered = dist_prev;
  else {
    dist_filtered = dist_raw;
    dist_prev = dist_raw;
  }

  // EMA filter
  dist_ema = dist_filtered*_EMA_ALPHA + dist_ema*(1 - _EMA_ALPHA);

  // Serial output
  Serial.print("Min:");       Serial.print(_DIST_MIN);
  
  Serial.print(",raw:");      Serial.print(min(dist_raw, _DIST_MAX + 10));
  Serial.print(",filtered:"); Serial.print(min(dist_filtered, _DIST_MAX + 10));
  Serial.print(",ema:");      Serial.print(min(dist_ema, _DIST_MAX + 10));
  
  Serial.print(",Max:");      Serial.print(_DIST_MAX);
  Serial.println("");

  if(dist_ema <= _DIST_DETECT && isOpen == false) {
    manipulateServo(false);
  }
  else if(dist_ema > _DIST_DETECT && isOpen == true) {
    manipulateServo(true);
  }
  
  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}

void manipulateServo(boolean _isOpen) {
  int manipulator;
  if(_isOpen == true) manipulator = 1;
  else manipulator = -1;
  
  unsigned long detected_time = millis();
  while(true) {
    unsigned long elapsed_time = millis() - detected_time;

    if(elapsed_time <= MOVING_TIME) {
      int mapped_time = map(elapsed_time, 0, MOVING_TIME, -5000, 5000);
      float sigmoid = (1.0/(1 + pow(M_E, manipulator*mapped_time/1000.0)))*1000;
      int angle = map(sigmoid, 0, 1000, startAngle, stopAngle);

      myServo.write(angle);
    } else {
      if(_isOpen == true) isOpen = false;
      else isOpen =true;

      last_sampling_time += MOVING_TIME;
      
      break;
    }
  }
}
