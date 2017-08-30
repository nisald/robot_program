#include <Servo.h>

#define SERV 10

Servo serv;

void setup() {
  // put your setup code here, to run once:
  serv.attach(SERV);
//  serv.write(0);
  serv.write(90);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 256; i += 5) {
    analogWrite(SERV, i);
    delay(30);
  }

  delay(1000);
  for (int i = 255; i >= 0; i -= 5) {
    analogWrite(SERV, i);
    delay(30);
  }
}
