#include <AFMotor.h>

#define DEBUG

/* Robot info */
#define ROBOT_WIDTH  6.0  // inches
#define ROBOT_LENGTH 7.0  // inches

/* steering parameters */
#define NORM_MOTORSPEED 100

/* wall detecting distance parameters */
#define SENSE_FRONT_FAR                     5.0      // inches; (*NOTE*) take a sharp left at this point
#define SENSE_FRONT_CLOSE                   2.0      // inches
#define SENSE_SIDE_FAR       11.0 - ROBOT_WIDTH      // inches; no wall if more than this
#define SENSE_SIDE_CLOSE    ((SENSE_SIDE_FAR) / 2.0)      // inches; too close if less than this

/* drive motor slots */
#define MOTOR_DRIVE_LEFT  1
#define MOTOR_DRIVE_RIGHT 2

/* hand motor slots */
#define MOTOR_HAND_BASE 3
#define MOTOR_HAND_GRAB 4

/* Ultrasonic sensor digital pins */
#define PING_FRONT_ECHO 2
#define PING_LEFT_ECHO 9
#define PING_RIGHT_ECHO 10
#define PING_COMM_TRIG 13

/* calculate the distance in inches for the time |D| */
#define calcInches(D) ((double) (D) / 74 / 2)

/* steering maneuvars */
typedef enum {
    UP, // both motors run forward at the same speed, but no monitoring
    DOWN, // same as front, but motors run backwards
    LEFT, // left motor runs in half of right motors speed
    RIGHT, // same as LEFT, this time right motor is slowed
    HARD_LEFT, // sharp left turning, left motor is released
    HARD_RIGHT, // sharp right turning, right motor is released
    // UP_LEFT, UP_RIGHT,
    // DOWN_LEFT, DOWN_RIGHT
    NULLMOV
} move_t;

/* state of the sensor reading */
typedef enum {
    INIT,  // initial sense
    CLEAR, // no wall in front of the sensor
    TOUCH, // sensing a wall at an optimal distant
    DANGER // wall is too close
} state_t;

typedef struct senses {
    double fwalldist;
    double lwalldist;
    double rwalldist;
    state_t fstate;
    state_t lstate;
    state_t rstate;
} senses_t;

/*
 *  left hand or right hand
 *  |hand| indicates which sensor should be used
 *  to detect wall and turns.
 */
typedef enum {
   LEFT_HAND, RIGHT_HAND
} hand_t;

/* define driving motors */
AF_DCMotor leftmt(MOTOR_DRIVE_LEFT);
AF_DCMotor rightmt(MOTOR_DRIVE_RIGHT);

/* define hand control motors */
AF_DCMotor basemt(MOTOR_HAND_BASE);
AF_DCMotor grabmt(MOTOR_HAND_GRAB);

hand_t hand = RIGHT_HAND; // use right hand rule, default
move_t drive = NULLMOV; // current driving state

senses_t sensors = {
    0.0,
    0.0,
    0.0,
    INIT,
    INIT,
    INIT
};

state_t frontreach = INIT; // state of the front reaching
state_t sidereach = INIT; // state of the side reaching
double frontdist = 0.0; // front distant to the wall
double sidedist = 0.0; // side distant to the wall

void setup()
{
    Serial.begin(9600);
    siri_init();
}

void loop()
{
    // Sense the world
    // take a move

    siri_sense(&sensors);
#ifndef DEBUG
    frontreach = sensors.fstate;
    sidereach = (hand == RIGHT_HAND) ? sensors.rstate : sensors.lstate;

    if (frontreach == CLEAR) {
        if (sidereach == CLEAR && drivestate != LEFT)
            siri_move(LEFT);
        else if (sidereach == TOUCH && drivestate != UP)
            siri_move(UP);
        else if (sidereach == DANGER && drivestate != UP_LEFT)
            siri_move(UP_LEFT);
        else
            siri_move(NULLMOV);
    } 
#endif
    
#ifdef DEBUG
    Serial.print("UltraSonic readings:: ");
    Serial.print("Front: "); Serial.print(sensors.fwalldist); Serial.print("\t");
    Serial.print("Left: "); Serial.print(sensors.lwalldist); Serial.print("\t");
    Serial.print("Right: "); Serial.print(sensors.rwalldist); Serial.println("\t");
#endif
}

void siri_init()
{
    pinMode(PING_FRONT_ECHO, INPUT);
    pinMode(PING_LEFT_ECHO, INPUT);
    pinMode(PING_RIGHT_ECHO, INPUT);
    pinMode(PING_COMM_TRIG, OUTPUT);

    int ishandswitchon = analogRead(A0);

    // on the hand switch to choose LEFT_HAND rule
//    if (ishandswtichon) 
//        hand = LEFT_HAND;
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
    digitalWrite(trig, LOW); // write an initial LOW for a clean trigger
    delayMicroseconds(2);
    digitalWrite(trig, HIGH); // write the high pulse
    delayMicroseconds(10);
    digitalWrite(trig, LOW); // end the high pulse

    long dtime = pulseIn(echo, HIGH);

    return calcInches(dtime);
}
