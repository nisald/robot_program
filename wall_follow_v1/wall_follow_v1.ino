// necessary libraries 
#include <AFMotor.h> // for Adafruit motor shield

#define ROBOT_WIDTH 6.0 

/* normal wheel speed */
#define NORMAL_MOTORSPEED 200

/* distants to walls */
/*
    -------------------------------
         \           /            |
         -\---------/- FAR        |
           \       /              |
         ---\-----/--- CLOSE      |
             \   /                |
           +-------+              |
           |       |   C     F    |
           |       | --|-----|----|
           |       | --|-----|----|
           |       |              |
           +-------+              |
             
 */
 #define DIST_FRONT_FAR 5.0                   // inches
 #define DIST_FRONT_CLS 2.0                   //   "
 #define DIST_SIDE_FAR  (11.0 - ROBOT_WIDTH)  //   "
 #define DIST_SIDE_CLS  (DIST_SIDE_FAR - 3.0) //   "

 /* motor slots for driving motors */
 #define MOTOR_DRIVE_LEFT  1
 #define MOTOR_DRIVE_RIGHT 2

 /* motor slots for hand motors */
 #define MOTOR_HAND_BASE 3
 #define MOTOR_HAND_GRAB 4

 /* ultrasonic pin layout */
 #define USS_FRONT_ECHO 2
 #define USS_LEFT_ECHO  9
 #define USS_RIGHT_ECHO 10
 #define USS_TRIGGER    13

/* define ∆time to inches */
#define ttoinch(D) ((double) (D) / 74 / 2)

#define DUMP_STATES \
Serial.print(front_state); Serial.print("\t"); Serial.print(side_state); Serial.print("\t");\
Serial.print(current_drive); Serial.print("\t")

/* steering maneuvars */
typedef enum {
    UP, DOWN,
    LEFT, RIGHT,
    HRDLEFT, HRDRIGHT,
    UPLEFT, UPRIGHT,
    DOWNLEFT, DOWNRIGHT,
    FREE, NULLMOVE
} Move;

/* states of the sensor readings */
typedef enum {
    INIT, CLEAR, OPTIMAL, DANGER
} State;

/* sensor readings */
typedef struct {
    double fwalldist;
    double lwalldist;
    double rwalldist;
    State  fstate;
    State  lstate;
    State  rstate;
} Sensor;

/* left/right hand folowing */
typedef enum {
	LEFT_HAND, RIGHT_HAND
} Hand;


/* driving motors */
AF_DCMotor leftmt(MOTOR_DRIVE_LEFT);
AF_DCMotor rightmt(MOTOR_DRIVE_RIGHT);

/* hand motors */
AF_DCMotor basemt(MOTOR_HAND_BASE);
AF_DCMotor grabmt(MOTOR_HAND_GRAB);

/* which side of the wall to follow */
Hand hand = RIGHT_HAND;

/* current driving behaviour */
Move current_drive = NULLMOVE;
Move left_drive = NULLMOVE;
Move right_drive = NULLMOVE;

/* Sensor output */
Sensor sensors = { 0.0, 0.0, 0.0, INIT, INIT, INIT };

/* Front and side states of the wall */
State front_state = INIT;
State side_state = INIT;
byte turning = RIGHT;

/* Front and side distants to the wall */
double front_dist = 0.0;
double side_dist = 0.0;

void siri_init()
{
    pinMode(USS_FRONT_ECHO, INPUT);
    pinMode(USS_LEFT_ECHO, INPUT);
    pinMode(USS_RIGHT_ECHO, INPUT);
    pinMode(USS_TRIGGER, OUTPUT);

    int ishandswitchon = analogRead(A2);

    // switch on the hand switch to choose LEFT_HAND rule
    if (ishandswitchon) {
        hand = LEFT_HAND;
        turning = LEFT;
        Serial.println("Following LEFT_HAND rule");
    } else 
        Serial.println("Following RIGHT_HAND rule");
        

    leftmt.setSpeed(NORMAL_MOTORSPEED);
    rightmt.setSpeed(NORMAL_MOTORSPEED);
    delay(1000);
    leftmt.run(FORWARD);
    rightmt.run(FORWARD);
}

Move siri_sense(Sensor *sensors)
{
    sensors->fwalldist = siri_readsonic(USS_FRONT_ECHO, USS_TRIGGER);
    sensors->lwalldist = siri_readsonic(USS_LEFT_ECHO, USS_TRIGGER);
    sensors->rwalldist = siri_readsonic(USS_RIGHT_ECHO, USS_TRIGGER);

    if (sensors->fwalldist > DIST_FRONT_FAR)
        sensors->fstate = CLEAR;
    else if (sensors->fwalldist < DIST_FRONT_CLS)
        sensors->fstate = DANGER;
    else
        sensors->fstate = OPTIMAL;

    if (sensors->rwalldist > DIST_SIDE_FAR)
        sensors->rstate = CLEAR;
    else if (sensors->rwalldist < DIST_SIDE_CLS)
        sensors->rstate = DANGER;
    else
        sensors->rstate = OPTIMAL;

    if (sensors->lwalldist > DIST_SIDE_FAR)
        sensors->lstate = CLEAR;
    else if (sensors->lwalldist < DIST_SIDE_CLS)
        sensors->lstate = DANGER;
    else
        sensors->lstate = OPTIMAL;
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

    return ttoinch(dtime);
}

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
    
    front_state = sensors.fstate;
    side_state = (hand == RIGHT) ? sensors.rstate : sensors.lstate;

    if (front_state == CLEAR) {
        if (side_state == CLEAR) {
            DUMP_STATES;
            siri_move(turning);
        } else if (side_state == OPTIMAL) {
            DUMP_STATES;
            siri_move(UP);
        } else if (side_state == DANGER) {
            DUMP_STATES;
            siri_move(turning+2);
        } else {
            DUMP_STATES;
            siri_move(NULLMOVE);
        }
    } else if (front_state == OPTIMAL) {
        DUMP_STATES;
    	siri_move(turning+2);
    } else if (front_state == DANGER) {
        DUMP_STATES;
    	siri_move(DOWN);
   	} 
    
//    Serial.print("UltraSonic readings:: ");
//    Serial.print("Front: "); Serial.print(sensors.fwalldist); Serial.print("\t");
//    Serial.print("Left: "); Serial.print(sensors.lwalldist); Serial.print("\t");
//    Serial.print("Right: "); Serial.print(sensors.rwalldist); Serial.println("\t");
}

Move siri_move(Move mov)
{
    switch (mov) {
    case UP:
        Serial.println("Moving UP");
        left_drive = right_drive = current_drive = UP;
        leftmt.run(FORWARD);
        rightmt.run(FORWARD);
        break;
    case DOWN:
        Serial.println("Moving Down");
        left_drive = right_drive = current_drive = DOWN;
        leftmt.run(BACKWARD);
        rightmt.run(BACKWARD);
        break;
    case LEFT:
        Serial.println("Turning Left");
        if (left_drive == UP) {
            leftmt.run(BACKWARD);
        } else {
            leftmt.run(FORWARD);
        }
        delay(20);
        leftmt.run(RELEASE);
        left_drive = FREE;
        current_drive = LEFT;
        break;
     case RIGHT:
        Serial.println("Turning Right");
        if (right_drive == UP) {
            rightmt.run(BACKWARD);
        } else {
            rightmt.run(FORWARD);
        }
        delay(20);
        rightmt.run(RELEASE);
        right_drive = FREE;
        current_drive = RIGHT;
        break;
     case HRDLEFT:
        Serial.println("Rotate Left");
        if (left_drive == UP) {
            leftmt.run(BACKWARD);
            left_drive = DOWN;
        } else {
            leftmt.run(FORWARD);
            left_drive = UP;
        }
        current_drive = HRDLEFT;
        break;
     case HRDRIGHT: 
        Serial.println("Rotate Right");
        if (right_drive == UP) {
            rightmt.run(BACKWARD);
            right_drive = DOWN;
        } else {
            rightmt.run(FORWARD);
            right_drive = UP;
        }
        current_drive = HRDRIGHT;
        break;
     case UPLEFT:
     case UPRIGHT:
     case DOWNLEFT:
     case DOWNRIGHT:
        // to implement
        break;
     default:
        if (left_drive != FREE && right_drive != FREE) {
            Serial.println("Releasing Motors");
            leftmt.run(RELEASE);
            rightmt.run(RELEASE);
            left_drive = right_drive = FREE;
        } 
        break;
    }

    return mov;
}


 
