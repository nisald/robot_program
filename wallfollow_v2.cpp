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

/* steering maneuvars */
typedef enum {
    UP, DOWN,
    LEFT, RIGHT,
    HRDLEFT, HRDRIGHT,
    // UPLEFT, UPRIGHT,
    // DOWNLEFT, DOWNRIGHT
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

/* Sensor output */
Sensor sense = { 0.0, 0.0, 0.0, INIT, INIT, INIT };

/* Front and side states of the wall */
State front_state = INIT;
State side_state = INIT;

/* Front and side distants to the wall */
double front_dist = 0.0;
double side_dist = 0.0;





