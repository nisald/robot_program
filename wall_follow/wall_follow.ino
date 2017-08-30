#include <AFMotor.h>

/* Robot info */
#define ROBOT_WIDTH  6.0  // inches
#define ROBOT_LENGTH 7.0  // inches

/* steering parameters */
#define NORM_MOTORSPEED 100

/* wall detecting distance parameters */
#define SENSE_FRONT_FAR                     5.0      // inches; (*NOTE*) take a sharp left at this point
#define SENSE_FRONT_CLOSE                   2.0      // inches
#define SENSE_SIDE_FAR       11.0 - ROBOT_WIDTH      // inches; no wall if more than this
#define SENSE_SIDE_CLOSE    ((SENSE_FAR) / 2.0)      // inches; too close if less than this

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

/* state of the sensor reading */
typedef enum {
    INIT,  // initial sense
    CLEAR, // no wall in front of the sensor
    TOUCH, // sensing a wall at an optimal distant
    DANGER // wall is too close
} sensestate_t;

typedef struct senses {
    double fwalldist;
    double lwalldist;
    double rwalldist;
    sensestate_t fstate;
    sensestate_t lstate;
    sensestate_t rstate;
} senses_t;

/* define driving motors */
AF_DCMotor leftmt(MOTOR_DRIVE_LEFT);
AF_DCMotor rightmt(MOTOR_DRIVE_RIGHT);

/* define hand control motors */
AF_DCMotor basemt(MOTOR_HAND_BASE);
AF_DCMotor grabmt(MOTOR_HAND_GRAB);

/* 
 *  left hand or right hand 
 *  |touchsensor| indicates which sensor should be used
 *  to detect wall and turns.
 */
typedef enum {
   LEFT_SENSOR, RIGHT_SENSOR
} touch_t;

touch_t touchsensor = RIGHT_SENSOR; // default to RIGHT sensor

senses_t sensors = {
    0.0,
    0.0,
    0.0,
    INIT,
    INIT,
    INIT
};

void setup()
{
    Serial.begin(9600);
}

void loop()
{}

void siri_init() 
{
    pinMode(PING_FRONT_ECHO, INPUT);
    pinMode(PING_LEFT_ECHO, INPUT);
    pinMode(PING_RIGHT_ECHO, INPUT);
    pinMode(PING_COMM_TRIG, OUTPUT);

    int istouchon = analogRead(A0);

    if (istouchon) touchsensor = LEFT_SENSOR;
    else touchsensor = RIGHT_SENSOR;
}

void siri_sense(senses_t *sensors) 
{
    sensors->fwalldist = siri_readsonic(PING_FRONT_ECHO, PING_COMM_TRIG);
    sensors->lwalldist = siri_readsonic(PING_LEFT_ECHO, PING_COMM_TRIG);
    sensors->rwalldist = siri_readsonic(PING_RIGHT_ECHO, PING_COMM_TRIG);

    if (sensors->fwalldist > SENSE_FRONT_FAR)
        sensors->fstate = CLEAR;
    else if (sensors->fwalldist < SENSE_FRONT_CLOSE)
        sensors->fstate = DANGER;
    else
        sensors->fstate = TOUCH;

    if (sensors->rwalldist > SENSE_SIDE_FAR)
        sensors->rstate = CLEAR;
    else if (sensors->rwalldist < SENSE_SIDE_CLOSE)
        sensors->rstate = DANGER;
    else
        sensors->rstate = TOUCH;

    if (sensors->lwalldist > SENSE_SIDE_FAR)
        sensors->lstate = CLEAR;
    else if (sensors->lwalldist < SENSE_SIDE_CLOSE)
        sensors->lstate = DANGER;
    else
        sensors->lstate = TOUCH;
}

/* read the ultrasonic input */ 
double siri_readsonic(byte echo, byte trig)
{
    digitalWrite(echo, LOW); // write an initial LOW for a clean trigger
    delayMicroseconds(2);
    digitalWrite(echo, HIGH); // write the high pulse
    delayMicroseconds(5);
    digitalWrite(echo, LOW); // end the high pulse

    long dtime = pulseIn(trig, HIGH);

    return calcInches(dtime);
}
