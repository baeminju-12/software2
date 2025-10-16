#include <Servo.h>

#define PIN_LED   9
#define PIN_TRIG  12
#define PIN_ECHO  13
#define PIN_SERVO 10

#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 180.0    // 18cm
#define _DIST_MAX 360.0    // 36cm
#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

#define _EMA_ALPHA 0.5

#define _DUTY_MIN  500     // 서보가 실제 0도에 근접하는 값
#define _DUTY_MAX  2300    // MG90S 최대 유효 값
#define _DUTY_NEU  ((_DUTY_MIN + _DUTY_MAX) / 2)

#define TARGET_LOW  _DIST_MIN  // LED 켜지는 거리 시작
#define TARGET_HIGH _DIST_MAX  // LED 켜지는 거리 끝

float dist_ema = _DIST_MAX;
float dist_prev = _DIST_MAX;
unsigned long last_sampling_time;

Servo myservo;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_NEU);

  Serial.begin(57600);
}

void loop() {
  if (millis() < last_sampling_time + INTERVAL) return;

  float dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);
  float dist_filtered;

  if (dist_raw == 0.0 || dist_raw > _DIST_MAX) {
    dist_filtered = _DIST_MAX;
  } else if (dist_raw < _DIST_MIN) {
    dist_filtered = _DIST_MIN;
  } else {
    dist_filtered = dist_raw;
    dist_prev = dist_raw;
  }

  dist_ema = _EMA_ALPHA * dist_filtered + (1.0 - _EMA_ALPHA) * dist_ema;

  int angle;
  if (dist_ema <= _DIST_MIN) {
    angle = 0;
  } else if (dist_ema >= _DIST_MAX) {
    angle = 180;
  } else {
    angle = map((int)dist_ema, (int)_DIST_MIN, (int)_DIST_MAX, 0, 180);
  }

  int duty = map(angle, 0, 180, _DUTY_MIN, _DUTY_MAX);
  myservo.writeMicroseconds(duty);

  if (dist_ema > TARGET_LOW && dist_ema < TARGET_HIGH) {
    digitalWrite(PIN_LED, LOW);  // active-low: 켜짐
  } else {
    digitalWrite(PIN_LED, HIGH); // 꺼짐
  }

  Serial.print("raw:"); Serial.print(dist_raw);
  Serial.print(", ema:"); Serial.print(dist_ema);
  Serial.print(", angle:"); Serial.print(angle);
  Serial.print(", duty:"); Serial.println(duty);

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
