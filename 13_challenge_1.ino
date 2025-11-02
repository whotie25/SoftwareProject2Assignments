#include <Servo.h>

#define PIN_TRIG  12
#define PIN_ECHO  13
#define PIN_SERVO 10

#define SND_VEL 346.0
#define PULSE_DURATION 10
#define _DIST_MIN 180.0
#define _DIST_MAX 360.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

// configurable parameters for Servo
#define _DUTY_MIN 300  // 0 degree
#define _DUTY_NEU 1200 // 90 degree
#define _DUTY_MAX 2500 // 180 degree

#define _POS_START (_DUTY_MIN + 100)
#define _POS_END   (_DUTY_MAX - 100)

#define _SERVO_SPEED 0.3 // servo angular speed (unit: degree/sec)

// Loop Interval
#define INTERVAL 20     // servo update interval (unit: msec)

// global variables
unsigned long last_sampling_time;

Servo myservo;

float duty_change_per_interval; // maximum duty difference per interval
float duty_target;    // Target duty time
float duty_curr;      // Current duty time

int toggle_interval, toggle_interval_cnt;

void setup() {
  // initialize GPIO pins
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myservo.attach(PIN_SERVO); 
  
  duty_target = duty_curr = _POS_START;
  myservo.writeMicroseconds(duty_curr);
  
  // initialize serial port
  Serial.begin(2000000);


  duty_change_per_interval = (_DUTY_MAX - _DUTY_MIN) * (_SERVO_SPEED / 180.0) * (INTERVAL / 1000.0);

  // initialize variables for servo update.
  toggle_interval = (180.0 / _SERVO_SPEED) * 1000 / INTERVAL;
  toggle_interval_cnt = toggle_interval;
  
  // initialize last sampling time
  last_sampling_time = 0;
}

void loop() {
  float  dist_raw;

  if (millis() < (last_sampling_time + INTERVAL))
    return;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // adjust duty_curr toward duty_target by duty_change_per_interval
  if (duty_target > duty_curr) {
    duty_curr += duty_change_per_interval;
    if (duty_curr > duty_target)
        duty_curr = duty_target;
  } else {
    duty_curr -= duty_change_per_interval;
    if (duty_curr < duty_target)
      duty_curr = duty_target;
  }

  // update servo position
  myservo.writeMicroseconds(duty_curr);

  // output the read value to the serial port
  Serial.print("Min:1000");
  Serial.print(",duty_target:"); Serial.print(duty_target);
  Serial.print(",duty_curr:");   Serial.print(duty_curr);
  Serial.print(",dist_raw:");   Serial.print(dist_raw);
  Serial.print(",duty_change_per_interval:");   Serial.print(duty_change_per_interval);
  Serial.println(",Max:2000");

  // toggle duty_target between _DUTY_MIN and _DUTY_MAX.
  if (toggle_interval_cnt >= toggle_interval) {
    toggle_interval_cnt = 0;
    if (duty_target == _POS_START)
      duty_target = _POS_END;
    else
      duty_target = _POS_START;
  } else {
    toggle_interval_cnt++;
  }

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
