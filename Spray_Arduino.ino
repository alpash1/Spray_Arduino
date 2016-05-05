//The pins of the spray valves
int sprayGas = 13;  
int sprayPilot = 12;

int sprayAlkali = 7;
int sprayRinse = 5;
int sprayAirblade = 3;

//The spray lengths
int sprayTimeQD = 400;
int sprayTimeAlkali = 1000;
int sprayTimeRinse = 4000;

//The delays between sprays
int delayGas_Pilot = 100;
int delayQD_Alkali = 3000;
int delayAlkali_Rinse = 1000;
int delayRinse_Air;

void setup() {
  // initialize digital spray pins as outputs.
  pinMode(sprayGas, OUTPUT);
  pinMode(sprayPilot, OUTPUT);
  pinMode(sprayAlkali, OUTPUT);
  pinMode(sprayRinse, OUTPUT);
  pinMode(sprayAirblade, OUTPUT);
}


void loop() {
 runSprayProgram(); 
}


//Run the spray program
void runSprayProgram() {
  sprayQD(sprayGas, sprayPilot, sprayTimeQD);
  delay(delayQD_Alkali);
  spray(sprayAlkali, sprayTimeAlkali);
  delay(delayAlkali_Rinse);
  spray(sprayRinse, sprayTimeRinse);
  delay(delayRinse_Air);
}


//Turns on the nozzle for the quantum dots, taking into account the pilot air line
void sprayQD(int sprayAirName, int sprayPilotName, int sprayTime) {
  digitalWrite(sprayAirName, HIGH);
  delay(delayGas_Pilot);
  spray(sprayPilotName, sprayTime);
  digitalWrite(sprayAirName, LOW);
}

//Turns a spray on for a particular amount of time
void spray(int sprayName, int onTime) {
  digitalWrite(sprayName, HIGH);
  delay(onTime);
  digitalWrite(sprayName, LOW);
}
