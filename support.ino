

//hier alle funktionen, das Mainfile war zu unübersichtlich

void HAL(int R, int G, int B, int Stunden, int Minuten)
{
int hhour; // 10 digit of hour
int lhour; // 1 digit of houe
int hmin; // 10 digit of minute
int lmin; // 1 digit of minute
unsigned long bitstream;
lhour= Stunden % 10;
hhour = (Stunden-lhour)/10;

lmin= Minuten % 10;
hmin = (Minuten-lmin)/10;

  #ifdef DEBUG
  Serial.print("Time:");
  Serial.print(hhour,DEC);
  Serial.print(lhour,DEC);
  Serial.print(hmin,DEC);
  Serial.println(lmin,DEC);
  #endif

if (hhour!=0) bitstream= (unsigned long)BCD_7s[hhour] << 24;
bitstream=bitstream | (unsigned long)BCD_7s[lhour] << 16;
// if (hmin!=0) 
bitstream=bitstream | (unsigned long)BCD_7s[hmin] << 8;
bitstream=bitstream | (unsigned long)BCD_7s[lmin];
// doppelpunkt
bitstream=bitstream|(unsigned long)0x00008000;

// bitmuster raus
selatch_out(bitstream);
// Farbe Raus
  analogWrite(GREENpwm,G);
  analogWrite(BLUEpwm,B);
  analogWrite(REDpwm,R);


}

void ItsMagic(int Stunden, int Minuten)
{
   #ifdef DEBUG
   Serial.print(Stunden);
   Serial.print(":");
   Serial.println(Minuten);
   #endif 
  int Warnlevel;
  int Jetzt=Stunden*60+Minuten;
  boolean Nachts=((Jetzt>Bettzeit)||(Jetzt<Weckzeit));
  //Warnlevel wird linear interpoliert zwischen Bettzeit und Weckzeit
  if (Nachts)
  {
    //Der Teil ist codetechnisch unschön, aber meine Simulation meint, der funzt
    int Korrektur=Mitternacht-Bettzeit;
    int KorrigierteWeckzeit=Weckzeit+Korrektur;
    int KorrigiertesJetzt=Jetzt+Korrektur;
    if (KorrigiertesJetzt>=Mitternacht)
      KorrigiertesJetzt=KorrigiertesJetzt-Mitternacht;
    Warnlevel=map(KorrigiertesJetzt,0,KorrigierteWeckzeit,0,255);
   #ifdef DEBUG
   Serial.println("Nachts");
   #endif 
  }
  else
  {
   Warnlevel=map(Jetzt,Weckzeit,Bettzeit,255,0);
   #ifdef DEBUG
   Serial.println("Tags");
   #endif 
  }
   #ifdef DEBUG
   Serial.println(Warnlevel);
   #endif
   //Farbwerte werden linear interpoliert
  int R=map(Warnlevel,0,255,Minimalrot,Maximalrot);
  int G=map(Warnlevel,0,255,Minimalgruen,Maximalgruen);
  int B=map(Warnlevel,0,255,Minimalblau,Maximalblau);
  #ifdef DEBUG
   Serial.print(R);
   Serial.print(",");
   Serial.print(G);
   Serial.print(",");
   Serial.println(B);
   #endif 
  HAL(R,G,B,Stunden,Minuten);
}

/*
time_t DCF77()
{
  time_t time;

 time=now();  // fill later wird durch DCF77 library ersetzt 
return time;
}
*/


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

