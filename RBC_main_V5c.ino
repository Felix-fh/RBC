

/* Aufbau der Schieberegister:

SI

1a 1b 1c 1d 1e 1f 1g 1h

2a 2b 2c 2d 2e 2f 2g 2h

3a 3b 3c 3d 3e 3f 3g 3h

4a 4b 4c 4d 4e 4f 4g 4h

a-g sind mit den Segmenten der entsprechenden Ziffern verbunden an
Ausgang h sind dann die sonderzeichen Doppelpunkte/Alarmpunkt und Blinker verbunden.
Segmentdefintion http://www.dieelektronikerseite.de/Pics/Lections/7-Segment-Anzeige%20-%20Sichtbare%20Binaerzahlen%20S02.GIF

1 ist zehnerstunde
2 einerstunde
3 zehnerminute
4 einerminute

*/

#include <Time.h>  //http://playground.arduino.cc/Code/Time
#include <TimeAlarms.h>
#include "./RBC_header.h"
// #include "./RBC_demo"


// BCD_7s enthält die segmentmuster für die Ziffern 0-9
//so kann die Ziffer die geschrieben werden soll einfach als index auf das array verwendet werden
//                        0    1    2    3    4    5    6    7    8    9 
const int BCD_7s[10]= {0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0X7B};

//ein paar vordefinierte Farben, prinzipiell alles nur vollmischungen, mal schauen ob auch noch zwischenwerte sinnvoll sind.
// Farben nur mit wertigkeiten 0, 1, 2 werden dann von routinen gemäß helligkeit hochgerechnet
const RGB RGBtab[13]={{0,0,0},{0,0,8},{0,8,0},{8,0,0},{0,8,8},{8,0,8},{8,8,0},{8,8,8},{8,8,4},{8,4,8},{4,8,8},{8,4,4},{4,4,8}};
const int colcount=13; // wieviele Farben sind definiert

//unvollständig //
// ein paar vordefinierte anzeigetexte
const unsigned long SETchar= 0 ;    // SET
const unsigned long ALchar=   0 ;  // AL
const unsigned long COLchar=  0 ;   // COL
// DAY OF Week                  Sun / Mon / Tue / Wen / Thu / Fri / Sat
const unsigned long DOWchar[7]={0x5F} ;     

// pin zuweisungen
const int SCLK = 2; // PIN für SCLK
const int SI = 0; // PIN Für SI
const int RCLK =1; // PIN für RCLK

const int REDpwm = 20; // PIN rot PWM
const int GREENpwm = 21; // PIN Für grün PWM
const int BLUEpwm =22; // PIN für blau PWM

//const int RED = 5; // PIN rot 
//const int GREEN = 4; // PIN Für grün
//const int BLUE =3; // PIN für blau


// Globale variablen
state currentState;  // aktueller State
state nextState;  // nächster State
event currentEvent;  // aktuelles Event

long last_state_change=0; // wird verwendet um timeout methodik in states zu verwenden.

long timeout=5000; // timeout für states in ms

alarmEvent alarm1;
int prewarning=30; // Value for start of colour change in min before alarm time
int preActive=0;   // 1 if currently in prewarning stage
int alarmActive=0; // 1 if alarm time has been reached

RGB colorIdle = RGBtab[3]; // Color for Idle
RGB colorAlarm = RGBtab[4]; // Color for alarm
RGB currentColor =RGBtab[1]; // Startingcolor

RGB colorDelta; // color difference
float colorStepR; // color step
float colorStepG;
float colorStepB;
/*
time_t DCF77()
{
  time_t time;

 time=now();  // fill later wird durch DCF77 library ersetzt 
return time;
}
*/

int inputdb(){
  int reading=0;
  int previous=0;
  int out = 0;
  long time = 0; // the last time the output pin was toggled
  long debounce = 200;   // the debounce time, increase if the output flickers

  
  reading = digitalRead(0);

  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    
    time = millis();    
  }

 return out; 

  //previous = reading;

}


void selatch_out(unsigned long output)
{ // die Bits im übergabewert werden in die Schieberegister geladen.
// aus noch nicht geklärten gründen gibt es probleme wenn die PWM für die Katoden aktiv ist wärend die bits geschrieben werden...
  Serial.print("selatch_out: ");
  Serial.println(output,HEX);
  
    for (int i=0; i < 31; i++) //für jedes bit des longs
    {
      digitalWrite(SCLK, LOW); // Schiebeclock LOW
      if bitRead(output, i) digitalWrite(SI, HIGH); // SI auf HIGH wenn ites Bit 1 ist
        else digitalWrite(SI, LOW); // auf low wenn nicht
      delayMicroseconds(1); // kurzes delay.. man weis ja nie
      digitalWrite(SCLK, HIGH); // Schiebeclock auf HIGH
      delayMicroseconds(1); // wieder kurz warten
      digitalWrite(SCLK, LOW); // Schiebeclock LOW
    }
  digitalWrite(RCLK, LOW); //übernahmecklock auf LOW
  delayMicroseconds(1); // wieder kurz warten1
  digitalWrite(RCLK, HIGH); //übernahmecklock auf HIGH, die Daten gehen raus.
}


unsigned long timeformat(time_t time)
{ // Funktion errechnet aus einer Zeit das Bitmuster für die Segmente
// verwendet Funktionen aus time.h
int hhour; // 10 digit of hour
int lhour; // 1 digit of houe
int hmin; // 10 digit of minute
int lmin; // 1 digit of minute
unsigned long bitstream;

lhour= hour(time) % 10;
hhour = (hour(time)-lhour)/10;

lmin= minute(time) % 10;
hmin = (minute(time)-lmin)/10;

Serial.print("Time:");
Serial.print(hhour,DEC);
Serial.print(lhour,DEC);
Serial.print(hmin,DEC);
Serial.println(lmin,DEC);

if (hhour!=0) bitstream= (unsigned long)BCD_7s[hhour] << 24;
bitstream=bitstream | (unsigned long)BCD_7s[lhour] << 16;
// if (hmin!=0) 
bitstream=bitstream | (unsigned long)BCD_7s[hmin] << 8;
bitstream=bitstream | (unsigned long)BCD_7s[lmin];

return bitstream;
}

void setcolorRGB(RGB color){
 
  int rd = color.r;
  int gn = color.g;
  int bl = color.b;
  Serial.print("setColorRGB :");
  Serial.print(rd, HEX);
  Serial.print(" ");
  Serial.print(gn, HEX);
  Serial.print(" ");
  Serial.println(bl, HEX);
  analogWrite(GREENpwm,gn);
  analogWrite(BLUEpwm,bl);
  analogWrite(REDpwm,rd);
}



void update_RBC(){ // update clockface
Serial.println("update_RBC");
unsigned long segments=0;
time_t time;
int remAL; // remaining time to Alarm
time=now();


segments=timeformat(time);
Serial.print("Timeformat segments");
Serial.println(segments,HEX);
// decide if dot to be displayed
// dot is only displayed if at least one alarm is valid for today
if ((1 << weekday(time)-1)&& alarm1.d)
  segments=segments|(unsigned long)0x00000080;

// double dot
segments=segments|(unsigned long)0x00008000;

Serial.print("dot segments");
Serial.println(segments,HEX);


// check and set appropiate color

// calculate remaining minutes until alarm
remAL=(alarm1.h*60+alarm1.m)-(hour(time)*60+minute(time));

Serial.print("Remaining until alarm");
Serial.println(remAL,DEC);


// check if prewarning time is reached
if (preActive =0){
  if (remAL<=prewarning){
    preActive=1;
    Serial.print("prewarning activated");
  }
  else setcolorRGB(colorIdle);
}


// if prewarning active select color
if (preActive =1){
  
  // currentColor.r=  

  // muss nur gemacht werden wenn Farben geändert werden.
  //Calculate difference of colors
  colorDelta.r=colorIdle.r-colorAlarm.r;
  colorDelta.g=colorIdle.g-colorAlarm.g;
  colorDelta.b=colorIdle.b-colorAlarm.b;

  Serial.print("ColorDelta :");
  Serial.print(colorDelta.r, HEX);
  Serial.print(" ");
  Serial.print(colorDelta.g, HEX);
  Serial.print(" ");
  Serial.println(colorDelta.b, HEX);

  // muss nur gemacht werden wenn Farben geändert werden.
  // segment in steps
  colorStepR=colorDelta.r/prewarning;
  colorStepG=colorDelta.g/prewarning;
  colorStepB=colorDelta.b/prewarning;
  
  Serial.print("ColorStep :");
  Serial.print(colorStepR, DEC);
  Serial.print(" ");
  Serial.print(colorStepG, DEC);
  Serial.print(" ");
  Serial.println(colorStepB, DEC);


  currentColor.r=colorIdle.r+colorStepR*(prewarning-remAL);
  currentColor.g=colorIdle.g+colorStepG*(prewarning-remAL);
  currentColor.b=colorIdle.b+colorStepB*(prewarning-remAL);
}


// activate special segments

// insert here
 
selatch_out(segments); // output segments to latches
setcolorRGB(currentColor); // set color
}


void Mooredummy(){
//dummy funktio
}

void IDLE_funct(){
}


 /* Maschine - Zustandsueberfuehrungstabelle */
const stateElement stateMatrix[STATE_COUNT][EVENT_COUNT + 1] = {
{{IDLE, St_Idle}, {TI, TI_Init},{IDLE, St_Idle},{IDLE, St_Idle},{IDLE, St_Idle}},
{{IDLE, Mooredummy}, {AL, AL_Init},{TI, Set_Input},{TI, Inc_Input},{TI, Dec_Input}},
{{IDLE, Mooredummy}, {COL, Mooredummy},{AL, Mooredummy},{AL, Mooredummy},{AL, Mooredummy}},
{{IDLE, Mooredummy}, {TI, Mooredummy},{COL, Mooredummy},{COL, Mooredummy},{COL, Mooredummy}}
} ;


/* Zustandsueberfuehrung */

void stateEval(){  
Serial.println("stateEval");
/* Waehle naechsten Zustand */  
  stateElement stateEvaluation = stateMatrix[currentState][currentEvent];  

/* Event (Eingabe) wird resettet */  
  currentEvent = NIL;  

/* Naechster Zustand wird als aktueller uebernommen */   
  currentState = stateEvaluation.nextState;  

/* Transition-Funktion  (MealyOutput) wird ausgefuehrt */  
  (*stateEvaluation.actionToDo)();  

/* Zustands-Funktion (MooreOutput) wird ausgewaehlt*/  
  stateEvaluation = stateMatrix[currentState][EVENT_COUNT];  

/* und ausgefuehrt */  
  (*stateEvaluation.actionToDo)();

}


void setup() {                
pinMode(SI, OUTPUT);
pinMode(SCLK, OUTPUT);
pinMode(RCLK, OUTPUT);

//pinMode(RED, OUTPUT);
//pinMode(GREEN, OUTPUT);
//pinMode(BLUE, OUTPUT);

Serial.begin(9600);
Serial.println("READY");
delay(500);
//setSyncProvider(DCF77);
//setSyncInterval(86400);

setTime(06,05,00,29,05,1977);

alarm1.h=6;
alarm1.m=30;
alarm1.d=62; // 00111110 all days except sundays and saturdays
}


void loop() {
/* 
 selatch_out(timeformat(now()));
 delay(1000);
 setTime(24,24,00,29,05,1977);
 selatch_out(timeformat(now()));
 delay(1000);
 if (timeStatus()==0) {}
 */
 //Serial.println("calling DEMO");
 //demo();
 Serial.println("calling DEMO2");
 demo2();
 //demo3();
 
 Serial.println("Loop");
 //stateEval();
 //St_Idle();
 delay(1000);
 Serial.println("Loop2");
 
  
}
