/* Zustaende */
const int STATE_COUNT=4;
typedef enum {IDLE,/*0*/TI,/*1*/AL,/*2*/COL,/*3*/ } state;


/* Eingaben */
const int EVENT_COUNT=5;
typedef enum{NIL,/*0*/MENU,/*1*/ENTER,/*2*/PLUS,/*3*/MINUS/*4*/ } event;

/* Ausgaben - als Funktionszeiger*/

typedef void (*action)();

/* Transition */

typedef struct {state nextState;action actionToDo; } stateElement;

 // Struct für einen Alarmzeitpunkt 
 // h stunde  m minute 
 // d tag an dem der Alarm ausgeführt wird bitcodiert bit 0 ist sonntag
typedef struct {int h; int m; int d;} alarmEvent; 

typedef struct {int r; int g; int b;} RGB;
