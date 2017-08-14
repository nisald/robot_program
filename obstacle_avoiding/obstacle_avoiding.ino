#include <Servo.h>
#include <AFMotor.h>

/* servo (head) rotation degrees */
#define SRV_FRONT  90
#define SRV_LEFT    0
#define SRV_RIGHT 180
#define SRV_BACK   -1  // for steering; not used for the servo

/* macros to avoid long function names */
#define Wlow(A)       digitalWrite((A), LOW); 
#define Whigh(A)      digitalWrite((A), HIGH);
#define Microdelay(T) delayMicroseconds((T));
#define p(S)          Serial.print((S))
#define pn(S)         Serial.println((S))

/* absolute value macro */
#define ABS(A)        ((A) > 0 ? (A) : (-(A)))

/* CONSTANTS */
#define TRIG         5 // ultrasonic (head) TRIGger pin
#define ECHO         6 // ultrasonic (head) ECHO pin
#define SERV        10 // SERVo motor 1 (SERVo; adafruit) signal pin
#define MOTORSPEED 100 // normal motor speed. (full speed is too much)
#define OBSTACLE_THRESHOLD 5.0  // minimum distance to an obstacle
#define QUARTER_STEER_DELAY 450  // time to turn one quarter (90-degree) in MOTORSPEED

/* globals */
AF_DCMotor mtleft(1);     // motor left is connected to M1
AF_DCMotor mtright(2);    // motor right is connected to M2
Servo      head;          // head servo motor
double     obstacle_dist; // distance to the obstacle
int        current_view = SRV_FRONT;  // current viewing side (default to front (SRV_FRONT))

void setup() {
  pinMode(TRIG, OUTPUT); 
  pinMode(ECHO, INPUT);
  pinMode(SERV, OUTPUT);
  head.attach(SERV);
  head.write(SRV_FRONT);
}

void loop() {
  obstacle_dist = readinchdist();

  if (obstacle_dist < OBSTACLE_THRESHOLD) {
    /* obstacle has found in the current viewing direction */
    switch (current_view) {
    case SRV_FRONT:
      head.write(SRV_RIGHT);
      current_view = SRV_RIGHT;
      break;
    case SRV_RIGHT:
      head.write(SRV_LEFT);
      current_view = SRV_LEFT;
      break;
    case SRV_LEFT:
      current_view = SRV_BACK;
      break;
    default:
      pn("TRAPPED");
      break;
    }
  } else {
    /* no obstacles in the current viewing side, free to go */
    switch (current_view) {
    case SRV_FRONT:
      steerfront();
      break;  
    case SRV_RIGHT:
      steerright();
      head.write(SRV_FRONT);
      current_view = SRV_FRONT;
      break;
    case SRV_LEFT:
      steerleft();
      head.write(SRV_FRONT);
      current_view = SRV_FRONT;
      break;
    case SRV_BACK:
      steerback();
      head.write(SRV_FRONT);
      current_view = SRV_FRONT;
    default:
      p("Unknown direction: "); pn(current_view);
      break;
    }
  }
}

double readinchdist() {
  double dtime;

  /* send trigger signal */
  Wlow(TRIG);
  Microdelay(2); 
  Whigh(TRIG);
  Microdelay(5);
  Wlow(TRIG);

  dtime = pulseIn(ECHO, HIGH);

  return ttoin(dtime);
}

double ttoin(long dtime) {
  return (double) dtime / 74 / 2;
}

void steer(int left_speed, int right_speed) {
  int left_dir = left_speed > 0 ? FORWARD : BACKWARD;
  int right_dir = right_speed > 0 ? FORWARD : BACKWARD;

  left_speed = ABS(left_speed);
  right_speed = ABS(right_speed);

  mtleft.setSpeed(left_speed);
  mtright.setSpeed(right_speed);

  mtleft.run(left_dir);
  mtright.run(right_dir);
}

void steerleft() {
  steer(-MOTORSPEED, MOTORSPEED);
  delay(QUARTER_STEER_DELAY);
  mtleft.run(RELEASE);
  mtright.run(RELEASE);
}

void steerright() {
  steer(MOTORSPEED, -MOTORSPEED);
  delay(QUARTER_STEER_DELAY);
  mtright.run(RELEASE);
  mtleft.run(RELEASE);
}

void steerfront() {
  steer(MOTORSPEED, MOTORSPEED);
}

void steerback() {
  steer(MOTORSPEED, -MOTORSPEED);
  delay(QUARTER_STEER_DELAY * 2);
  mtright.run(RELEASE);
  mtleft.run(RELEASE);
}

void releasemotors() {
  mtleft.run(RELEASE);
  mtright.run(RELEASE);
}
