/*
Code for testing APDS9960 on Arduino/ESP8266

Pre-initial release for test Sensor proccess and integrate in 'ESP Easy' plug-in
Operation non-blocking and no pin interrupts required.
In two words: POOL MODE

 Developed with Atom & PlatfomrIO.
 José María Pérez. @pepsilla

 MIT License
 Copyright (c) 2017 @pepsilla
*/
#include <Wire.h>
#include <SerialCommand.h>

#define CHIP_ADDR 0x39
#define i2C_LEN 32
#define DEBUG 0
#define arduinoLED 2

//Function declarations
void readDataRegisters();
void clearAlsInterrupt();
void clearProximityInterrupt();
void getProximityData();
void getAlsData();
void cpstatAutoGain();
void getGestureData();
//void echoesVariables();
void showRegisters();
void echoesProximityVariables();
void echoesGestureVariables();
void echoesFlags();
void echoesTimes();

void CHIP_on();
void CHIP_off();
void proximityOn();
void proximityOff();
void alsOn();
void alsOff();
void sayHello();
void ppulse();
void pgain();
void pplen();
void ldrive();
void pilt();
void piht();
void ppers();
void again();
void poolDelay();
void gestureOn();
void gestureOff();
void gmode();
void unrecognized(const char *);
void initSerialCommands();

//Global variables declarations
uint8_t i2cBuffer[256] =
{
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
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

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
  pinMode(arduinoLED, OUTPUT);
  //Serial up
  Serial.begin(460800);
  // Wire -I2C- up
  Wire.begin(4,5);

  //Add commands to SerialCommand
  initSerialCommands();
}


/*
** LOOP PROCCESS
** Read chip registers and filter values
*/
void loop() {

  //Begin format data serial
  if(!_showI2cBuffer)Serial.println(F("APDS9960@pepsilla_TestLab"));
  if(!_showI2cBuffer)Serial.println(F("{"));
  //First Object for rapid mode identification
  if(!_showI2cBuffer)echoesFlags();
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
  if(!_showI2cBuffer)echoesProximityVariables();
  if(!_showI2cBuffer)echoesGestureVariables();
  //Dump registers data and time proccess if needed
  if(_showI2cBuffer)showRegisters();
  //Ends output serial data in this loop
  //And include times for control processes.
  if(!_showI2cBuffer)echoesTimes();
  if(!_showI2cBuffer)Serial.println(F("}"));

  delay(_poolDelay);
  //if(!_showI2cBuffer)Serial.print(F("{APDS9960_pepsillaTestLab>\""));
  sCmd.readSerial(); // We don't do much, just process serial commands
  //if(!_showI2cBuffer)Serial.println(F("\"}"));
  //delay(500);
}

//All i2c utility functions:

//Proximity interrupt:
// Process proximity interrupt if flag is true.
void clearProximityInterrupt()
{
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0xe5);
  Wire.write(0);
  Wire.endTransmission();
}
//ALS interrupt
// Process Light and Colour interrupt if flag is true.
void clearAlsInterrupt()
{
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0xe6);
  Wire.write(0);
  Wire.endTransmission();
}

//Point to register 0 to read in 32 bytes blocks.
void readDataRegisters()
{
  uint8_t value = 0;
  uint8_t counter = 0;
  uint8_t valuesRequested = 0;

  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0);
  if( Wire.endTransmission() != 0 ) {
      Serial.println("ERROR ON CHIP");
      return;
  }
  // 8 loops * 32 bytes in each loop = 0xFF registers readed.
  for(counter=0;counter<8;counter++)
  {
    valuesRequested = Wire.requestFrom(CHIP_ADDR,32);
  #if DEBUG
    Serial.print(valuesRequested);
    Serial.print("\t");
    Serial.println(point_Buffer);
  #endif
    while(Wire.available()){
      i2cBuffer[point_Buffer] = Wire.read();
      point_Buffer++;
  #if DEBUG
      Serial.print(".");
  #endif
      if(point_Buffer>0xff)break;
    }
  #if DEBUG
    Serial.println(point_Buffer);
  #endif
    // If load execed time proc IP Stack, yield. Uncomment if necessary.
    //yield();
  }
}


//Proximity data value ready
// Get proximity data when proximity data is valid
void getProximityData()
{
  uint8_t value = 0;
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x9c);
  Wire.endTransmission();
  uint8_t readValues = Wire.requestFrom(CHIP_ADDR, 1);
  if(readValues==1)
  {
    while(Wire.available())value=Wire.read();
  }
  if(!_showI2cBuffer)
  {
    Serial.print(F("\tPROXIMITY_DATA{PDATA:"));
    Serial.print(value);
    Serial.println(F("},"));
  }

}

//ALS DATA READY:
// Get Light and colour data when is valid
void getAlsData()
{
  uint16_t regVals = 0;
  uint8_t readValues = 0;
  uint8_t loByte = 0;
  uint8_t hiByte = 0;

  if(!_showI2cBuffer)Serial.print(F("\tALS_DATA{CLEAR:"));
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x94);
  Wire.endTransmission();
  readValues = Wire.requestFrom(CHIP_ADDR, 2);
  if(readValues==2)
  {
    loByte = Wire.read();
    hiByte = Wire.read();

    regVals = hiByte;
    regVals <<= 8;
    regVals |= loByte;
    //Serial.print(regVals);
  }
  if(!_showI2cBuffer)Serial.print(regVals);
  if(!_showI2cBuffer)Serial.print(F(",RED:"));
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x96);
  Wire.endTransmission();
  readValues = Wire.requestFrom(CHIP_ADDR, 2);
  if(readValues==2)
  {
    loByte = Wire.read();
    hiByte = Wire.read();

    regVals = hiByte;
    regVals <<= 8;
    regVals |= loByte;
    //Serial.print(regVals);
  }
  if(!_showI2cBuffer)Serial.print(regVals);
  if(!_showI2cBuffer)Serial.print(F(",GREEN:"));
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x98);
  Wire.endTransmission();
  readValues = Wire.requestFrom(CHIP_ADDR, 2);
  if(readValues==2)
  {
    loByte = Wire.read();
    hiByte = Wire.read();

    regVals = hiByte;
    regVals <<= 8;
    regVals |= loByte;
    //Serial.print(regVals);
  }
  if(!_showI2cBuffer)Serial.print(regVals);
  if(!_showI2cBuffer)Serial.print(F(",BLUE:"));
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x98);
  Wire.endTransmission();
  readValues = Wire.requestFrom(CHIP_ADDR, 2);
  if(readValues==2)
  {
    loByte = Wire.read();
    hiByte = Wire.read();

    regVals = hiByte;
    regVals <<= 8;
    regVals |= loByte;
    if(!_showI2cBuffer)Serial.print(regVals);
    if(!_showI2cBuffer)Serial.println(F("},"));
  }
}

//Check if gesture valid
// Get Gesture data when is valid.
void getGestureData()
{
  Wire.beginTransmission(CHIP_ADDR);
    Wire.write(0xaB);
    Wire.write(i2cBuffer[0xaB]&0b11111110);
  Wire.endTransmission();
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0xfc);
  Wire.endTransmission();
  uint8_t readValues = Wire.requestFrom(CHIP_ADDR, 4);
  if(readValues==4)
  {
    uint8_t upValue = Wire.read();
    uint8_t downValue = Wire.read();
    uint8_t leftValue = Wire.read();
    uint8_t rightValue = Wire.read();
    if(!_showI2cBuffer)
    {
      Serial.print(F("\t{GESTURE_DATA:{UP:"));
      Serial.print(upValue);
      Serial.print(F(",DOWN:"));
      Serial.print(downValue);
      Serial.print(F(",LEFT:"));
      Serial.print(leftValue);
      Serial.print(F(",RIGHT:"));
      Serial.print(rightValue);
      Serial.println("}");
    }
  }
}

//Check CPSAT for set AGAIN value decreased.
// Result an auto gain control if als data overflow.
void cpstatAutoGain()
{
  uint8_t courrentGain = 0;
  courrentGain = i2cBuffer[0x8f] & 0b00000011;
  if(courrentGain>0)
  {
    courrentGain --;
    i2cBuffer[0x8f] &= 0b11111100;
    i2cBuffer[0x8f] |= courrentGain;
    Wire.beginTransmission(CHIP_ADDR);
    Wire.write(0x8f);
    Wire.write(i2cBuffer[0x8f]);
    Wire.endTransmission();
  }
}

//Echoes Flags
void echoesFlags()
{
  String controlFlags = "\"";
  if(i2cBuffer[0x80]&1)controlFlags += F("PON,");
  if(i2cBuffer[0x80]&2)controlFlags += F("AEN,");
  if(i2cBuffer[0x80]&4)controlFlags += F("PEN,");
  if(i2cBuffer[0x80]&8)controlFlags += F("WEN,");
  if(i2cBuffer[0x80]&16)controlFlags += F("AIEN,");
  if(i2cBuffer[0x80]&32)controlFlags += F("PIEN,");
  if(i2cBuffer[0x80]&64)controlFlags += F("GEN,");
  if(i2cBuffer[0x8d]&2)controlFlags += F("WLONG,");
  if(i2cBuffer[0x90]&128)controlFlags += F("PSIEN,");
  if(i2cBuffer[0x90]&64)controlFlags += F("CPSIEN,");
  if(i2cBuffer[0x93]&128)controlFlags += F("CPSAT,");
  if(i2cBuffer[0x93]&64)controlFlags += F("PGSAT,");
  if(i2cBuffer[0x93]&32)controlFlags += F("PINT,");
  if(i2cBuffer[0x93]&16)controlFlags += F("AINT,");
  if(i2cBuffer[0x93]&4)controlFlags += F("GINT,");
  if(i2cBuffer[0x93]&2)controlFlags += F("PVALID,");
  if(i2cBuffer[0x93]&1)controlFlags += F("AVALID,");
  if(i2cBuffer[0x9F]&32)controlFlags += F("PCMP,");
  if(i2cBuffer[0x9F]&16)controlFlags += F("SAI,");
  if(i2cBuffer[0x9F]&8)controlFlags += F("PMSK_U,");
  if(i2cBuffer[0x9F]&4)controlFlags += F("PMSK_D,");
  if(i2cBuffer[0x9F]&2)controlFlags += F("PMSK_L,");
  if(i2cBuffer[0x9F]&1)controlFlags += F("PMSK_R,");
  if(i2cBuffer[0xAB]&4)controlFlags += F("GFIFO_CLR,");
  if(i2cBuffer[0xAB]&2)controlFlags += F("GIEN,");
  if(i2cBuffer[0xAB]&1)controlFlags += F("GMODE,");
  if(i2cBuffer[0xAF]&2)controlFlags += F("GFOW,");
  if(i2cBuffer[0xAF]&1)controlFlags += F("GVALID,");
  controlFlags += F("--\",");
  Serial.print(F("\tFLAGS:"));
  Serial.println(controlFlags);
}

//Echoes time this->processing variables.
void echoesTimes()
{
  Serial.println(F("\tTIMES:"));
  Serial.println(F("\t{"));
  Serial.print(F("\t\ti2c_time:\""));
  Serial.print(i2cTime_proc/1000);
  Serial.println(F("ms.\","));
  Serial.print(F("\t\tChip_ID:\"0x"));
  if(i2cBuffer[0x92]<16)Serial.print("0");
  Serial.print(i2cBuffer[0x92],16);
  time_proc = micros()-time_in;
  Serial.println(F("\","));
  Serial.print(F("\t\tproc_time:"));
  Serial.println(time_proc/1000);
  Serial.println(F("\t}"));
}

//Echoes Proximity data variables
void echoesProximityVariables()
{
  uint8_t value = 0;
  //Other registers on dependencies
  Serial.println(F("\tPROXIMITY_VARIABLES"));
  Serial.println(F("\t{"));
  Serial.print(F("\t\tPPLEN:\""));
  value = i2cBuffer[0x8e]&0b11000000;
  value >>= 6;
  switch(value)
  {
    case 0:
      Serial.print(F("4us\""));
      break;
    case 1:
      Serial.print(F("8us\""));
      break;
    case 2:
      Serial.print(F("16us\""));
      break;
    case 3:
      Serial.print(F("32us\""));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("PPULSE:"));
  value = i2cBuffer[0x8e]&0b00111111;
  Serial.print(value);
  Serial.print(F(","));
  Serial.print(F("LDRIVE:\""));
  value = i2cBuffer[0x8f]&0b11000000;
  value >>= 6;
  switch(value)
  {
    case 0:
      Serial.print(F("100mA\""));
      break;
    case 1:
      Serial.print(F("50mA\""));
      break;
    case 2:
      Serial.print(F("25mA\""));
      break;
    case 3:
      Serial.print(F("12.5mA\""));
      break;
  }
  Serial.println(F(","));
  Serial.print(F("\t\tPGAIN:\""));
  value = i2cBuffer[0x8f]&0b00001100;
  value >>= 2;
  switch(value)
  {
    case 0:
      Serial.print(F("1x\""));
      break;
    case 1:
      Serial.print(F("2x\""));
      break;
    case 2:
      Serial.print(F("4x\""));
      break;
    case 3:
      Serial.print(F("8x\""));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("AGAIN:\""));
  value = i2cBuffer[0x8f]&0b00000011;
  switch(value)
  {
    case 0:
      Serial.print(F("1x\""));
      break;
    case 1:
      Serial.print(F("4x\""));
      break;
    case 2:
      Serial.print(F("16x\""));
      break;
    case 3:
      Serial.print(F("64x\""));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("PILT:"));
  Serial.print(i2cBuffer[0x89]);
  Serial.print(F(","));
  Serial.print(F("PIHT:"));
  Serial.print(i2cBuffer[0x8B]);

  Serial.print(F(","));
  Serial.print(F("PPERS:"));
  value = i2cBuffer[0x8C]&11110000;
  value >>= 4;
  Serial.println(value);
  Serial.println(F("\t},"));
}

//Echoes Gesture data variables
void echoesGestureVariables()
{
  uint8_t value = 0;
  //Other registers on dependencies
  Serial.println(F("\tGESTURE_VARIABLES"));
  Serial.println(F("\t{"));
  Serial.print(F("\t\tGPENTH:"));
  value = i2cBuffer[0xa0];
  Serial.print(value);
  Serial.print(F(","));
  Serial.print(F("GPEXTH:"));
  value = i2cBuffer[0xa1];
  Serial.print(value);
  Serial.print(F(","));
  Serial.print(F(","));
  Serial.print(F("GFIFOTH:\""));
  value = i2cBuffer[0xA2]&0b11000000;
  value >>= 6;
  switch(value)
  {
    case 0:
      Serial.print(F("af1d\""));
      break;
    case 1:
      Serial.print(F("af4d\""));
      break;
    case 2:
      Serial.print(F("af8d\""));
      break;
    case 3:
      Serial.print(F("af16d\""));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("GEXMSX:\""));
  value = i2cBuffer[0xa2]&0b00111100;
  value >>= 2;
  switch(value)
  {
    case 0:
      Serial.print(F("ALL_included\""));
      break;
    case 1:
      Serial.print(F("R_excluded\""));
      break;
    case 2:
      Serial.print(F("L_excluded\""));
      break;
    case 4:
      Serial.print(F("D_excluded\""));
      break;
    case 6:
      Serial.print(F("L&D_excluded\""));
      break;
    case 5:
      Serial.print(F("ALL_excluded\""));
      break;
    default:
      Serial.print(F("---\""));
      break;
  }
  Serial.println(F(","));
  Serial.print(F("\t\tGEXPERS:\""));
  value = i2cBuffer[0xa2]&0b00000011;
  switch(value)
  {
    case 0:
      Serial.print(F("1st\""));
      break;
    case 1:
      Serial.print(F("2nd\""));
      break;
    case 2:
      Serial.print(F("4th\""));
      break;
    case 3:
      Serial.print(F("7th\""));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("GGAIN:\""));
  value = i2cBuffer[0xA3]&0b01100000;
  value >>= 5;
  switch(value)
  {
    case 0:
      Serial.print(F("1x\""));
      break;
    case 1:
      Serial.print(F("2x\""));
      break;
    case 2:
      Serial.print(F("4x\""));
      break;
    case 3:
      Serial.print(F("8x\""));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("GLDRIVE:\""));
  value = i2cBuffer[0xa3]&0b00011000;
  value >>= 3;
  switch(value)
  {
    case 0:
      Serial.print(F("100mA\""));
      break;
    case 1:
      Serial.print(F("50mA\""));
      break;
    case 2:
      Serial.print(F("25mA\""));
      break;
    case 3:
      Serial.print(F("12.5mA\""));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("GWTIME:\""));
  value = i2cBuffer[0xa3]&00000111;
  switch(value)
  {
    case 0:
      Serial.print(F("0ms\""));
      break;
    case 1:
      Serial.print(F("2.8ms\""));
      break;
    case 2:
      Serial.print(F("5.6ms\""));
      break;
    case 3:
      Serial.print(F("8.4ms\""));
      break;
    case 4:
      Serial.print(F("14.0ms\""));
      break;
    case 5:
      Serial.print(F("22.4ms\""));
      break;
    case 6:
      Serial.print(F("30.8ms\""));
      break;
    case 7:
      Serial.print(F("39.2ms\""));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("LEDBOOST:\""));
  value = i2cBuffer[0x90]&00110000;
  value >>= 4;
  switch(value)
  {
    case 0:
      Serial.print(F("100%\""));
      break;
    case 1:
      Serial.print(F("150%\""));
      break;
    case 2:
      Serial.print(F("200%\""));
      break;
    case 3:
      Serial.print(F("300%\""));
      break;
  }
  Serial.println(F(","));
  Serial.print(F("\t\tGOFFSET_U:"));
  value = i2cBuffer[0xA4];
  Serial.print(value);
  Serial.print(F(","));
  Serial.print(F("GOFFSET_D:"));
  value = i2cBuffer[0xA5];
  Serial.print(value);
  Serial.print(F(","));
  Serial.print(F("GOFFSET_L:"));
  value = i2cBuffer[0xA7];
  Serial.print(value);
  Serial.print(F(","));
  Serial.print(F("GOFFSET_R:"));
  value = i2cBuffer[0xA9];
  Serial.print(value);
  Serial.println(F(","));
  Serial.print(F("\t\tGPULSE:"));
  value = i2cBuffer[0xA6]&0b00111111;
  Serial.print(value);
  Serial.print(F(","));
  Serial.print(F("GPLEN:\""));
  value = i2cBuffer[0xA6]&0b11000000;
  value >>= 6;
  switch(value)
  {
    case 0:
      Serial.print(F("4us\""));
      break;
    case 1:
      Serial.print(F("8us\""));
      break;
    case 2:
      Serial.print(F("16us\""));
      break;
    case 3:
      Serial.print(F("32us\""));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("GDIMS:\""));
  value = i2cBuffer[0xAa]&0b00000011;
  switch(value)
  {
    case 0:
      Serial.print(F("U/D&LR_valid\""));
      break;
    case 1:
      Serial.print(F("U/D_only\""));
      break;
    case 2:
      Serial.print(F("L/R_only\""));
      break;
    case 3:
      Serial.print(F("U/D&LR_valid\""));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("GFLVL:"));
  value = i2cBuffer[0xAe];
  Serial.print(value);
  Serial.println();
  Serial.println(F("\t},"));
}

//Dump registers data and time proccess
void showRegisters()
{

  point_Buffer = 0;
  uint8_t counterFile, counterRow,value;
  //Serial.println(F("\tDUMP_REGISTERS"));
  //Serial.println(F("\t{"));
  for(counterFile = 0;counterFile<16;counterFile++){
    //Serial.print(F("\t\t"));
    Serial.print(counterFile); //,16);
    Serial.print(":");
    for(counterRow=0;counterRow<16;counterRow++){
      uint8_t pointerValue = counterFile*16+counterRow;
      uint8_t pointedValue = i2cBuffer[pointerValue];
      //if(pointedValue<16)Serial.print("0");
      Serial.print(pointedValue); //,16);
      if(counterRow<15)Serial.print(",");
    }
    Serial.println(); //F("\""));
  }
  //Serial.println(F("\t}"));
}





//All pieces of SerialCommand:

//Set Serial Commands.
void initSerialCommands()
{
  sCmd.addCommand("ON",    CHIP_on);
  sCmd.addCommand("OFF",   CHIP_off);
  sCmd.addCommand("PON",   proximityOn);
  sCmd.addCommand("POFF",  proximityOff);
  sCmd.addCommand("AON",   alsOn);
  sCmd.addCommand("AOFF",  alsOff);
  sCmd.addCommand("HELLO", sayHello);
  sCmd.addCommand("PPULSE",ppulse);
  sCmd.addCommand("PGAIN", pgain);
  sCmd.addCommand("PPLEN", pplen);
  sCmd.addCommand("LDRIVE",ldrive);
  sCmd.addCommand("PILT",  pilt);
  sCmd.addCommand("PIHT",  piht);
  sCmd.addCommand("PPERS", ppers);
  sCmd.addCommand("AGAIN", again);
  sCmd.addCommand("DELAY", poolDelay);
  sCmd.addCommand("GON",   gestureOn);
  sCmd.addCommand("GOFF",  gestureOff);
  sCmd.addCommand("GMODE",  gmode);
  sCmd.setDefaultHandler(unrecognized);
}

// Just another command not included in
// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
  Serial.println(F("ON\tPower On CHIP"));
  Serial.println(F("OFF\tPower Off CHIP"));
  Serial.println(F("PON\tProximity mode ON"));
  Serial.println(F("POFF\tProximity mode OFF"));
  Serial.println(F("AON\tAmbient Light Sensor ON"));
  Serial.println(F("AOFF\tAmbient Light Sensor OFF"));
  Serial.println(F("HELLO\t Dump Chip registers and times"));
  Serial.println(F("PPULSE\tProximity pulse count"));
  Serial.println(F("PGAIN\tProximity gain control"));
  Serial.println(F("PPLEN\tPrpximity Pulse Length"));
  Serial.println(F("LDRIVE\tLed Drive Strength"));
  Serial.println(F("PILT\tProximity Interrupt Threshold Low"));
  Serial.println(F("PIHT\tProximity Interrupt Threshold High"));
  Serial.println(F("PPERS\tProximity Interrupt Persistance"));
  Serial.println(F("AGAIN\tALS and Color Gain Control"));
  Serial.println(F("DELAY\t*** DELAY TIME IN LOOP"));
  Serial.println(F("GON\tGesture mode ON"));
  Serial.println(F("GOFF\tGesture mode OFF"));
  Serial.println(F("GMODE\t0/1 Reset/Set GMODE bit"));
  delay(10000);
}
// ON command
void CHIP_on() {
  Serial.println("CHIP on");
  digitalWrite(arduinoLED, LOW);
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x80);
  Wire.write((i2cBuffer[0x80]&0b11111110)|1);
  Wire.endTransmission();
}
//OFF command
void CHIP_off() {
  Serial.println("CHIP off");
  digitalWrite(arduinoLED, HIGH);
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x80);
  Wire.write(i2cBuffer[0x80]&0b11111110);
  Wire.endTransmission();
}

//PON command
void proximityOn()
{
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x80);
  Wire.write((i2cBuffer[0x80]&0b11011011)|4);
  Wire.endTransmission();
}
//POFF command
void proximityOff()
{
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x80);
  Wire.write(i2cBuffer[0x80]&0b11011011);
  Wire.endTransmission();
}
//AON command
void alsOn()
{
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x80);
  Wire.write((i2cBuffer[0x80]&0b11101101)|2);
  Wire.endTransmission();
}
//AOFF command
void alsOff()
{
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x80);
  Wire.write(i2cBuffer[0x80]&0b11101101);
  Wire.endTransmission();
}
//HELLO command
void sayHello() {
  _showI2cBuffer = !_showI2cBuffer;
  Serial.println(_showI2cBuffer);
}
//DELAY command
void poolDelay() {
  int aNumber;
  char *arg;

  //Serial.println("We're in processCommand");
  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    _poolDelay = aNumber;
  }
}
//PPULSE command
void ppulse() {
  int aNumber;
  char *arg;

  //Serial.println("We're in processCommand");
  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    //Serial.print("First argument was: ");
    //Serial.println(aNumber);
    if(aNumber<64)
    {
      aNumber |= (i2cBuffer[0x8e]&0b11000000);
      Wire.beginTransmission(CHIP_ADDR);
      Wire.write(0x8e);
      Wire.write(aNumber);
      Wire.endTransmission();
    }
  }
}
//PPLEN command
void pplen() {
  int aNumber;
  char *arg;

  //Serial.println("We're in processCommand");
  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    //Serial.print("First argument was: ");
    //Serial.println(aNumber);
    if(aNumber<4)
    {
      uint8_t value;
      aNumber <<= 6;
      value = i2cBuffer[0x8e]&0b00111111;
      aNumber |= value;
      Wire.beginTransmission(CHIP_ADDR);
      Wire.write(0x8e);
      Wire.write(aNumber);
      Wire.endTransmission();
    }
  }
}
//LDRIVE command
void ldrive() {
  int aNumber;
  char *arg;

  //Serial.println("We're in processCommand");
  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    //Serial.print("First argument was: ");
    //Serial.println(aNumber);
    if(aNumber<4)
    {
      aNumber <<=6;
      aNumber |= (i2cBuffer[0x8f]&0b00111111);
      Wire.beginTransmission(CHIP_ADDR);
      Wire.write(0x8f);
      Wire.write(aNumber);
      Wire.endTransmission();
    }
  }
}
//PGAIN command
void pgain() {
  int aNumber;
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    if(aNumber<4)
    {
      aNumber <<=2;
      aNumber |= (i2cBuffer[0x8f]&0b11110011);
      Wire.beginTransmission(CHIP_ADDR);
      Wire.write(0x8f);
      Wire.write(aNumber);
      Wire.endTransmission();
    }
  }
}
//AGAIN command
void again() {
  int aNumber;
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    if(aNumber<4)
    {
      aNumber |= (i2cBuffer[0x8f]&0b11111100);
      Wire.beginTransmission(CHIP_ADDR);
      Wire.write(0x8f);
      Wire.write(aNumber);
      Wire.endTransmission();
    }
  }
}
//PILT command
void pilt() {
  int aNumber;
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    Wire.beginTransmission(CHIP_ADDR);
      Wire.write(0x89);
      Wire.write(aNumber);
    Wire.endTransmission();
  }
}
//PILH command
void piht() {
  int aNumber;
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    Wire.beginTransmission(CHIP_ADDR);
      Wire.write(0x8B);
      Wire.write(aNumber);
    Wire.endTransmission();
  }
}
//PPERS command
void ppers() {
  int aNumber;
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    if(aNumber<16)
    {
      aNumber <<= 4;
      aNumber |= i2cBuffer[0x8c]&0b00001111;
      Wire.beginTransmission(CHIP_ADDR);
        Wire.write(0x8C);
        Wire.write(aNumber);
      Wire.endTransmission();
    }
  }
}
//GON command
void gestureOn()
{
  //alsOff();
  uint8_t value = 0;
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x80);
  value = i2cBuffer[0x80]&0b10111111;
  value |= 0b01000000;
  Wire.write(value);
  Wire.endTransmission();
}
//GOFF command
void gestureOff()
{
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x80);
  Wire.write((i2cBuffer[0x80]&0b10111111));
  Wire.endTransmission();
  //alsOn();
}
//GMODE command
void gmode()
{
  int aNumber;
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    if(aNumber == 0)
    {
      Wire.beginTransmission(CHIP_ADDR);
        Wire.write(0xaB);
        Wire.write(i2cBuffer[0xaB]&0b11111110);
      Wire.endTransmission();
    }
    else
    {
      Wire.beginTransmission(CHIP_ADDR);
        Wire.write(0xaB);
        Wire.write(i2cBuffer[0xaB]|1);
      Wire.endTransmission();
    }
  }
}
