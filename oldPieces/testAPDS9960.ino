#include <Wire.h>
#include <SerialCommand.h>

#define CHIP_ADDR 0x39
#define i2C_LEN 32
#define DEBUG 0
#define arduinoLED 2

uint8_t i2cBuffer[256]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t point_Buffer = 0;
float time_in;
float time_proc;
float i2cTime_proc;

boolean _showI2cBuffer = false;
int _poolDelay = 500;

SerialCommand sCmd; // The demo SerialCommand object

/*
** SETUP PROCCESS.
** SERIAL UP, WIRE UP, ADD SERIAL COMMANDS.
*/
void setup() {
  //Serial up
  Serial.begin(460800);
  // Wire -I2C- up
  Wire.begin(4,5);

  //Add commands to SerialCommand
  setSerialCommands();
}

/*
** LOOP PROCCESS
** Read chip registers and filter values
*/
void loop() {


  //Begin format data serial
  Serial.println(F("APDS9960@pepsilla_TestLab"));
  Serial.println(F("{"));
  //Get time in for mettering propousses
  time_in = micros();
  //Read 0xff chip registers in 8x32 bytes blocks
  readDataRegisters();
  //Get time proccess in I2C read operation and resest time in loop
  i2cTime_proc = micros()-time_in;
  time_in = micros();

  //Proximity interrupt:
  // Process proximity interrupt if flag is true.
  if(i2cBuffer[0x93]&32)clearProximityInterrupt();

  //ALS interrupt
  // Process Light and Colour interrupt if flag is true.
  if(i2cBuffer[0x93]&16)clearAlsInterrupt();

  //Proximity data value ready
  // Get proximity data when proximity data is valid
  if(i2cBuffer[0x93]&2)getProximityData();

  //ALS DATA READY:
  // Get Light and colour data when is valid
  if(i2cBuffer[0x93]&1)getAlsData();

  //Check CPSAT for set AGAIN value decreased.
  if(i2cBuffer[0x93]&128)cpstatAutoGain();

  //Check if gesture valid
  // Get Gesture data when is valid.
  if(i2cBuffer[0xaf]&1)getGestureData();

  //Echoes Proximity data variables
  echoesVariables();
  //Dump registers data and time proccess if needed
  if(_showI2cBuffer)showRegisters();
  //Ends output serial data in this loop
  Serial.println(F("}"));

  delay(_poolDelay);
  Serial.print(F("{SERIAL:'"));
  sCmd.readSerial(); // We don't do much, just process serial commands
  Serial.println(F("'}"));
  //delay(500);
}
