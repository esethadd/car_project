
#include <OBD2UART.h>
#include <LiquidCrystal.h>

const int rs = 4, e = 6, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
bool hasReconnected = false;

LiquidCrystal lcd(rs, e, d4, d5, d6, d7);
COBD obd;

//preparing lcd screen for data
void setScreen(){
  //clear lcd screen and prepare to print data selected
  lcd.clear();

  //RPM
  lcd.setCursor(0,0); // lcd is |||||||||||||||| x2
  lcd.print("RPM:");

  //Throttle Position
  lcd.setCursor(8,0);
  lcd.print("THR:");

  //Coolant Temperature
  lcd.setCursor(0,1);
  lcd.print("CT:");

  //Speed
  lcd.setCursor(7,1);
  lcd.print("MPH:");
}

void setup() { 

  lcd.begin(16,2); //sets up LCD's columns and rows -- 16 columns 2 rows
  lcd.setCursor(1,0);
  
  //if the OBD hasn't hit an error previously, display connecting. In other words, first time connection
  if(hasReconnected == false){
  lcd.print("Connecting....");
  }
  obd.begin(); //start serial communication
  while(obd.init() == false);//Keep attempting to intialize OBD connection until successful 
  setScreen(); 
}

//reconnect when obd crashes
void reconnect() {
  hasReconnected = true;

  //Clear lcd screen and print in the middle
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Reconnecting..");

  //keep attempting to reconnect. If tried more than 10 times stop
  for (uint16_t i = 0;obd.init() == true; i++) {
    if (i == 10) {
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Reconnection");
      lcd.setCursor(5,1);
      lcd.print("Failed");
      delay(3000);
      
      setup();
      break;
    }
    delay(3000);
  }
}

//print values onto lcd screen
void printValues(byte PID, int val) {
  switch(PID){
    case(PID_RPM):
      lcd.setCursor(4,0);
      lcd.print(val);
    break;

    case(PID_COOLANT_TEMP):
      lcd.setCursor(3,1);
      lcd.print(val);
    break;

    case(PID_SPEED):
      lcd.setCursor(11,1);
      lcd.print(val);
    break;

    case(PID_THROTTLE):
      lcd.setCursor(12,0);
      lcd.print(val);
    break;

  }
}

void loop() {
  //PIDs to be read
  static byte index = 0;
  static byte PIDs[] = {PID_RPM, PID_COOLANT_TEMP, PID_SPEED, PID_THROTTLE};
  byte PID = PIDs[index];
  int val;

  //if OBD has a value for specified PID, print these values
  if(obd.readPID(PID, val)){
    printValues(PID, val);
  }
  index = (index + 1) % sizeof(PIDs); //increment index til sizeOf array then reset

  //if OBD crashes, restart
  if(obd.errors >= 2){
    reconnect();
  }


}

