#include <AFMotor.h>

#define MSPEED 150

#define SIDE_FAR 4.0 // inches
#define SIDE_CLS 2.0 // inches
#define FRONT_CLS 4.0 // inches

#define TRIG 13
#define RIGHT_ECHO 10
#define FRONT_ECHO 2

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define ROTL 4
#define ROTR 5
#define FREE 6

AF_DCMotor left(1);
AF_DCMotor right(2);

double fdist = 0.0, rdist = 0.0;

void setup() {
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    left.setSpeed(MSPEED);
    right.setSpeed(MSPEED);
    steer(UP);
}

void loop() {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(5);
    digitalWrite(TRIG, LOW);

    rdist = pulseIn(RIGHT_ECHO, HIGH) / 74.0 / 2;
    
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(5);
    digitalWrite(TRIG, LOW);

    fist = pulseIn(FRONT_ECHO, HIGH) / 74.0 / 2;

    if (fdist < FRONT_CLS) {
        if (rdist < SIDE_FAR) {
            steer(ROTL);
            delay(300);
            steer(FREE);
        } else {
            steer(ROTR);
            delay(300);
            steer(FREE);
        }
    } else {
        if (rdist > SIDE_FAR) {
            steer(RIGHT);
        } else if (rdist < SIDE_CLS) {
            steer(LEFT);
        } else {
            steer(UP);
        }
    }
    delay(50);
}

void steer(int dir)
{
    switch (dir) {
    case LEFT:
        left.run(RELEASE);
        right.run(FORWARD);
        break;
    case RIGHT:
        right.run(RELEASE);
        left.run(FORWARD);
        break;
    case UP:
        left.run(FORWARD);
        right.run(FORWARD);
        break;
    case DOWN:
        left.run(BACKWARD);
        right.run(BACKWARD);
        break;
    case ROTL:
        left.run(BACKWARD);
        right.run(FORWARD);
        break;
    case ROTR:
        left.run(FORWARD);
        right.run(BACKWARD);
        break;
    default:
        left.run(RELEASE);
        right.run(RELEASE);
        break;
    }
}

