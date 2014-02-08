

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

//Im Betrieb unnütze Ausgaben über UART
#define DEBUG

//Konfiguration
//Aktuell statisch, später könnte man Taster dazubauen, das macht das Programm allerdings deutlich komplexer.
//Nur: ist dir das dann nicht zu unnerdig? Ist es nicht viel cooler, das Gerät umzuprogrammieren, anstatt auf Tasten zu drücken?
//Ich finde jedenfalls diese Anleitungen für Konfigurationen von irgendwas halbwegs Komplexem mit drei Tasten abartig.
//Für den Laien ist das sicher besser, als "Besorg dir einen Compiler, hier hast du den Quellcode",
//aber für unsereins ist die Lage völlig anderes.
const int Weckstunden=6;
const int Weckminuten=30;
const int Bettstunden=22;
const int Bettminuten=5;

const int PeriodeInMinuten=5;

//Aktuell steht Vollrot für die Einschlafzeit, Vollblau für die Aufwachzeit und alles dazwischen für alles dazwischen
const int Minimalrot=255;
const int Maximalrot=0;
const int Minimalgruen=0;
const int Maximalgruen=0;
const int Minimalblau=0;
const int Maximalblau=255;
//Ende der Konfiguration

const int Weckzeit=Weckstunden*60+Weckminuten;
const int Bettzeit=Bettstunden*60+Bettminuten;
const int Mitternacht=24*60;


void setup() {                
pinMode(SI, OUTPUT);
pinMode(SCLK, OUTPUT);
pinMode(RCLK, OUTPUT);

  #ifdef DEBUG
  Serial.begin(9600);
  Serial.println("READY");
  delay(500);
   #endif

setTime(21,05,00,29,05,1977);
 
//setSyncProvider(DCF77);
//setSyncInterval(86400);

}


void loop() {

  ItsMagic(hour(now()),minute(now()));
 delay(100);
 #ifdef DEBUG
  // beschleunigungsmodus
   adjustTime(100);
 #endif
  
}
