// Arduino pin assignment
#define PIN_LED 9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 100      // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance (mm)
#define _DIST_MID 200.0   // middle distance (mm)
#define _DIST_MAX 300.0   // maximum distance (mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance
unsigned long last_sampling_time;   // unit: msec

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);  
  Serial.begin(115200);
}

void loop() {
  float distance = USS_measure(PIN_TRIG, PIN_ECHO); // read distance
  int pwmValue = 255; //기본값 꺼짐..
  
  if (millis() < (last_sampling_time + INTERVAL))
    return;
  if ((distance == 0.0) || (distance < _DIST_MIN) || (distance > _DIST_MAX)) {
    pwmValue = 255; // 범위 밖이면 꺼짐
  } else {
    if (distance <= _DIST_MID) {
      // 100 ~ 200mm: 증가 (255 → 0)
      pwmValue = (distance - _DIST_MIN) * (0 - 255) / (_DIST_MID - _DIST_MIN) + 255;
    } else {
      // 200 ~ 300mm: 감소 (0 → 255)
      pwmValue = (distance - _DIST_MID) * (255 - 0) / (_DIST_MAX - _DIST_MID) + 0;
    }
  }

  analogWrite(PIN_LED, pwmValue);

  Serial.print("distance: ");  Serial.print(distance);
  Serial.print(" mm, pwm: ");  Serial.println(pwmValue);
  Serial.print("Min:");        Serial.print(_DIST_MIN);
  Serial.print(",Max:");       Serial.print(_DIST_MAX);

  delay(INTERVAL);
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}

