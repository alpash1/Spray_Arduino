#include <ArduinoJson.h>

/*The Arduino pins which control each of the spray valves*/
const int pinSprayGas = 10;  
const int pinSprayPilot = 12;
const int pinSprayAlkali = 7;
const int pinSprayRinse = 5;
const int pinSprayAir = 2;


/*Variables for handling JSON message arrival*/
char json[200];
StaticJsonBuffer<200> jsonBuffer;

//Checks whether the serial message has been fully transcoded
boolean messageReceived = false;


/*Variables for the spray settings, with initial default values set*/

//The number of layers to be sprayed (one layer per spray cycle)
int noOfLayers = 0;

//The length of spraying time for each nozzle, in milliseconds
int sprayTimeQD = 400;
int sprayTimeAlkali = 1000;
int sprayTimeRinse = 4000;
int sprayTimeAir = 10000;

//The delay between the gas line switching on and the pilot line switching on, in milliseconds
int delayGas_Pilot = 100;

//The delay between the end of one spray and the start of the next, in milliseconds
int delayQD_Alkali = 3000;
int delayAlkali_Rinse = 1000;
int delayRinse_Air = 500;
int delayAir_QD = 1000;

/*--------------------------------------------------------------------------*/

//Initialises the Arduino for serial communication and configures the pins
void setup() {
  //Setup the serial connection
  Serial.begin(9600);
  Serial.print("hello");  

  //Initialise digital spray pins as outputs
  pinMode(pinSprayGas, OUTPUT);
  pinMode(pinSprayPilot, OUTPUT);
  pinMode(pinSprayAlkali, OUTPUT);
  pinMode(pinSprayRinse, OUTPUT);
  pinMode(pinSprayAir, OUTPUT);
}


//Enables Arduino to reset itself
void(* resetFunc) (void) = 0;

//The main loop of the Arduino
void loop() {
  
  if (messageReceived) {
    //Prints the message received to Serial
    Serial.print(json);

    //Parses the message into a JSON object
    JsonObject& root = jsonBuffer.parseObject(json);

    //Extracts the settings from the JSON object and sets the spray settings based on the info
    setSettings(root);

    //Runs the spray cycle until the desired number of layers has been reached
    for (int i=0; i < noOfLayers; i++) runSprayProgram(); 

    //Sets the message received to false
    messageReceived = false;

    //Resets the Arduino as it can hang here
    resetFunc();
  }  
}


//Extracts the spray settings received over serial from the JSON object, and sets the Arduino spray settings based on them
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

//Runs the spray program (ie a single spray cycle)
void runSprayProgram() {
  sprayQD(pinSprayGas, pinSprayPilot, sprayTimeQD);
  delay(delayQD_Alkali);
  spray(pinSprayAlkali, sprayTimeAlkali);
  delay(delayAlkali_Rinse);
  spray(pinSprayRinse, sprayTimeRinse);
  delay(delayRinse_Air);
  spray(pinSprayAir, sprayTimeAir);
  delay(delayAir_QD);
}


//Turns on the nozzle for the quantum dots, taking into account the pilot air line
void sprayQD(int pinSprayName, int pinSprayPilotName, int sprayTime) {
  digitalWrite(pinSprayName, HIGH);
  delay(delayGas_Pilot);
  spray(pinSprayPilotName, sprayTime);
  digitalWrite(pinSprayName, LOW);
}

//Turns a spray on for a particular amount of time
void spray(int pinSprayName, int onTime) {
  digitalWrite(pinSprayName, HIGH);
  delay(onTime);
  digitalWrite(pinSprayName, LOW);
}


/*----------------------------------------------------
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
   //Stops reading the message when an @ symbol is read
   Serial.readBytesUntil('@', json, 200);

   //Sets the messageReceived variable to true so that the main program loop is executed
   messageReceived = true;
  }
}
