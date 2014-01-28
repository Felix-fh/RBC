void demo2(){
  //multicolor test
  for (int i=0;i<(colcount);i++){
      
      selatch_out(0xFFFFFFFF);
      setcolorRGB(RGBtab[i]);
      delay(1000);
     
    }
    setcolorRGB(RGBtab[0]);
}

void demo3(){
  //multicolor test2
  unsigned long time = millis()+5000;
  while (millis()< time){
    unsigned long temp=(0x00000001);
    int capI=0;
    for (int i=0; i<31;i++){
      capI=i % colcount;
      setcolorRGB(RGBtab[0]);
      selatch_out(temp);    
      setcolorRGB(RGBtab[capI]);
      temp = temp << 1;
      delay(1);
    }
  }
  
}


/*
void demo(){
  unsigned long rolling1=(0x01010101);
  unsigned long bitstream=(0);
  
  Serial.println("setting color");
  setcolorRGB(100,0,0);
  Serial.println("setting pattern1");
  selatch_out(0xFFFFFFFF);
  delay(500);
  Serial.println("setting color");
  setcolorRGB(0,0,100);
  Serial.println("setting pattern2");
  selatch_out(0xFFFFFFFF);
  delay(500);
  Serial.println("setting color");
  setcolorRGB(0,100,0);
  delay(500);
  selatch_out(0);
  delay(1000);
  
  for (int i=0; i<7; i++){
    setcolorRGB(RGBtab[i+1].r,RGBtab[i+1].b,RGBtab[i+1].g);
    selatch_out(rolling1 << i);
    delay(250);
  }
  setcolorRGB(128,128,128);
  delay(500);
  for (int i=0; i<=9; i++){ 
    bitstream=0;
    bitstream= (unsigned long)BCD_7s[i] << 24;
    bitstream=bitstream | (unsigned long)BCD_7s[i] << 16;
    bitstream=bitstream | (unsigned long)BCD_7s[i] << 8;
    bitstream=bitstream | BCD_7s[i];
    //setcolorRGB(0,0,0);
    selatch_out(bitstream);
    setcolorRGB(128,128,128);
    delay(500);
  }
  Serial.println("off");
  setcolorRGB(0,0,0);
  }
  */
