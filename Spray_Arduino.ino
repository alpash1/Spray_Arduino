#include <ArduinoJson.h>

//Checks whether the serial message has been fully transcoded
boolean messageReceived = false;
char json[200];
StaticJsonBuffer<200> jsonBuffer;

//The pins of the spray valves
int sprayGas = 13;  
int sprayPilot = 12;

int sprayAlkali = 7;
int sprayRinse = 5;
int sprayAir = 3;

//The number of layers
int noOfLayers = 0;

//The spray lengths
int sprayTimeQD = 400;
int sprayTimeAlkali = 1000;
int sprayTimeRinse = 4000;
int sprayTimeAir = 10000;

//The delays between sprays
int delayGas_Pilot = 100;
int delayQD_Alkali = 3000;
int delayAlkali_Rinse = 1000;
int delayRinse_Air = 500;
int delayAir_QD = 1000;

void setup() {
  //Setup the serial connection
  Serial.begin(9600);  

  // initialize digital spray pins as outputs.
  pinMode(sprayGas, OUTPUT);
  pinMode(sprayPilot, OUTPUT);
  pinMode(sprayAlkali, OUTPUT);
  pinMode(sprayRinse, OUTPUT);
  pinMode(sprayAir, OUTPUT);
}


void loop() {
  // print the string when a newline arrives:
  if (messageReceived) {
    Serial.print(json);
    
    JsonObject& root = jsonBuffer.parseObject(json);
    
    setSettings(root);
    
    for (int i=0; i < noOfLayers; i++) runSprayProgram(); 

    //Set the message received to false:
    messageReceived = false;
  }  
}

//Set the settings via the Raspberry Pi
void setSettings(JsonObject& jsonSettings) {
  noOfLayers= jsonSettings["noOfLayers"];
  sprayTimeQD = jsonSettings["durationQD"];
  delayQD_Alkali = jsonSettings["pauseQD"];
  sprayTimeAlkali = jsonSettings["durationAlkali"];
  delayAlkali_Rinse = jsonSettings["pauseAlkali"];
  sprayTimeRinse = jsonSettings["durationWash"];
  delayRinse_Air = jsonSettings["pauseWash"];
  sprayTimeAir = jsonSettings["durationAir"];
  delayAir_QD = jsonSettings["pauseAir"];
}

//Run the spray program
void runSprayProgram() {
  sprayQD(sprayGas, sprayPilot, sprayTimeQD);
  delay(delayQD_Alkali);
  spray(sprayAlkali, sprayTimeAlkali);
  delay(delayAlkali_Rinse);
  spray(sprayRinse, sprayTimeRinse);
  delay(delayRinse_Air);
  spray(sprayAir, sprayTimeAir);
  delay(delayAir_QD);
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
   Serial.readBytesUntil('@', json, 200);
   messageReceived = true;
  }
}
