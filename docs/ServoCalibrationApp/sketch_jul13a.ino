#include <Servo.h>

Servo servo;

void setup() {

  Serial.begin(115200);
  pinMode(6, OUTPUT);
  servo.attach(6);
}

void loop() {

  if (Serial.available() != 0) {

    int ms = Serial.parseInt();
    Serial.println(ms);

    servo.writeMicroseconds(ms);
  }

}
