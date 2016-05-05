//Checks whether the serial message has been fully transcoded
boolean stringComplete = false;
String inputString = "";

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
  //Setup the serial connection
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);  

  // initialize digital spray pins as outputs.
  pinMode(sprayGas, OUTPUT);
  pinMode(sprayPilot, OUTPUT);
  pinMode(sprayAlkali, OUTPUT);
  pinMode(sprayRinse, OUTPUT);
  pinMode(sprayAirblade, OUTPUT);
}


void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    setSettings(inputString);
    Serial.println(delayRinse_Air);
    runSprayProgram(); 
    // clear the string:
    inputString = "";
    stringComplete = false;
  }  
}

//Set the settings via the Raspberry Pi
void setSettings(String settingsString) {
  sprayTimeQD = settingsString[0] - '0';
  delayQD_Alkali = settingsString[1] - '0';
  sprayTimeAlkali = settingsString[2] - '0';
  delayAlkali_Rinse = settingsString[3] - '0';
  sprayTimeRinse = settingsString[4] - '0';
  delayRinse_Air = settingsString[5] - '0';  
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


/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

