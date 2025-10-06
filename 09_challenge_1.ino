#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 10
#define _DIST_MAX 350

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

unsigned long last_sampling_time;   // unit: msec

const int N = 30;            // array size
float rawDataArr[N] = {0};  // data array
int idx = 0;

void setup() {
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered;
  
  if (millis() < last_sampling_time + INTERVAL)
    return;

  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);

  //appending to Array
  rawDataArr[idx] = dist_raw;
  idx = (idx+1)%N;

  // Median Filter
  dist_filtered = medianFilter(rawDataArr);

  // SERIAL OUTPUT
  Serial.print("Min:");   Serial.print(_DIST_MIN);
  Serial.print(",raw:"); Serial.print(min(dist_raw, _DIST_MAX + 100));
  Serial.print(",median:");  Serial.print(min(dist_filtered, _DIST_MAX + 100));
  Serial.print(",Max:");  Serial.print(_DIST_MAX);
  Serial.println("");

  if ((dist_raw < _DIST_MIN) || (dist_raw > _DIST_MAX))
    digitalWrite(PIN_LED, 1);
  else
    digitalWrite(PIN_LED, 0);

  // update last sampling time
  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}

// Median Filter
float medianFilter(float* arr){
  float copiedArr[N];
  for(int i = 0; i < N; i++) copiedArr[i] = arr[i];

  //insertion sort
  for(int i = 1; i < N; i++) {
    float trg = copiedArr[i];

    int j = i-1;
    while(j >= 0 && trg  < copiedArr[N]) {
      copiedArr[j+1] = copiedArr[j];
      j--;
    }

    copiedArr[j+1] = trg;
  }

  //median
  if(N % 2 == 1) return copiedArr[N/2];
  else return 0.5f*(copiedArr[N/2] + copiedArr[N/2 - 1]);
}
