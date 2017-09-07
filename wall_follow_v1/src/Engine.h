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
} Sensors;



 
