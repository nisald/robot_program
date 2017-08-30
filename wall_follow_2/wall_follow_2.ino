#include <AFMotor.h>

/* Robot info */
#define ROBOT_WIDTH  6  // inches
#define ROBOT_LENGTH 7  // inches

/* steering parameters */
#define NORM_MOTORSPEED 100

/* wall detecting distance parameters */
#define SENSE_FRONT 5 // inches; (*NOTE*) take a sharp left at this point
#define SENSE_FAR       11 - ROBOT_WIDTH      // inches; no wall if more than this
#define SENSE_CLOSE     ((SENSE_FAR) / 2)     // inches; too close if less than this

/* drive motor slots */
#define MOTOR_DRIVE_LEFT  1
#define MOTOR_DRIVE_RIGHT 2

/* hand motor slots */
#define MOTOR_HAND_BASE 3
#define MOTOR_HAND_GRAB 4

/* Ultrasonic sensor digital pins */
#define PING_FRONT_ECHO 2
#define PING_RIGHT_ECHO 9
#define PING_LEFT_ECHO 10
#define PING_COMM_TRIG 13

/* calculate the distance in inches for the time |D| */
#define calcInches(D) ((double) (D) / 74 / 2)

/* steering maneuvars */
typedef enum {
    FRONT, // both motors run forward at the same speed, but no monitoring
    REVERSE, // same as front, but motors run backwards
    LEFT, // left motor runs in half of right motors speed
    RIGHT, // same as LEFT, this time right motor is slowed
    HARD_LEFT, // sharp left turning, left motor is released
    HARD_RIGHT // sharp right turning, right motor is released  
} dir_t;


/* define driving motors */
AF_DCMotor leftmt(MOTOR_DRIVE_LEFT);
AF_DCMotor rightmt(MOTOR_DRIVE_RIGHT);

/* define hand control motors */
AF_DCMotor basemt(MOTOR_HAND_BASE);
AF_DCMotor grabmt(MOTOR_HAND_GRAB);

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

/* read the ultrasonic input */ 
double readsonic(byte echo, byte trig)
{
    digitalWrite(echo, LOW); // write an initial LOW for a clean trigger
    delayMicroseconds(2);
    digitalWrite(echo, HIGH); // write the high pulse
    delayMicroseconds(5);
    digitalWrite(echo, LOW); // end the high pulse

    long dtime = pulseIn(trig, HIGH);

    return calcInches(dtime);
}

/* steer the robot by differentiating wheel speeds */ 
void steer(int leftspd, int rightspd)
{
    leftmt.setSpeed(abs(leftspd));
    rightmt.setSpeed(abs(rightspd));

    leftmt.run(leftspd > 0 ? FORWARD : BACKWARD);
    rightmt.run(rightspd > 0 ? FORWARD : BACKWARD);
}
