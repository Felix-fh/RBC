// This File should have the State functions as content


void TI_Init(){ // initialise TI State
Serial.println("TI_Init");
}



void Set_Input(){ // accepts current value as new value and stores in runtime Data
Serial.println("Set_Input");
}

void Inc_Input(){ // increments the current input Value to next valid value (wraparound)
Serial.println("Inc_Input");
}

void Dec_Input(){ // decrements the current input Value to next valid value (wraparound)
Serial.println("Dec_Input");
}


void AL_Init(){ // initialise AL State
Serial.println("AL_Init");
}

void St_Idle(){ // idle State function
Serial.println("St_Idle");

update_RBC();

//colorSet();
//SetColor(currentColor[0];currentColor[1];currentColor[2]);

}
