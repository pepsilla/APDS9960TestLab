#include <Wire.h>

//Proximity interrupt:
// Process proximity interrupt if flag is true.
void clearProximityInterrupt
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
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x9c);
  Wire.endTransmission();
  uint8_t readValues = Wire.requestFrom(CHIP_ADDR, 1);
  if(readValues==1)
  {
    while(Wire.available())value=Wire.read();
  }
  Serial.print(F("\tPROXIMITY_DATA{PDATA:"));
  Serial.print(value);
  Serial.println(F("},"));


}

//ALS DATA READY:
// Get Light and colour data when is valid
void getAlsData()
{
  uint16_t regVals = 0;
  uint8_t readValues = 0;
  uint8_t loByte = 0;
  uint8_t hiByte = 0;

  Serial.print(F("\tALS_DATA{CLEAR:"));
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
  Serial.print(regVals);
  Serial.print(F(",RED:"));
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
  Serial.print(regVals);
  Serial.print(F(",GREEN:"));
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
  Serial.print(regVals);
  Serial.print(F(",BLUE:"));
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
    Serial.print(regVals);
    Serial.println(F("},"));
  }
}

//Check if gesture valid
// Get Gesture data when is valid.
void getGestureData()
{
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


//Echoes Proximity data variables
void echoesVariables()
{
  uint8_t value = 0;
  //Other registers on dependencies
  Serial.print(F("\tPROXIMITY_VARIABLES{"));
  Serial.print(F("PPLEN:'"));
  value = i2cBuffer[0x8e]&0b11000000;
  value >>= 6;
  switch(value)
  {
    case 0:
      Serial.print(F("4us'"));
      break;
    case 1:
      Serial.print(F("8us'"));
      break;
    case 2:
      Serial.print(F("16us'"));
      break;
    case 3:
      Serial.print(F("32us'"));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("PPULSE:"));
  value = i2cBuffer[0x8e]&0b00111111;
  Serial.print(value);
  Serial.print(F(","));
  Serial.print(F("LDRIVE:'"));
  value = i2cBuffer[0x8f]&0b11000000;
  value >>= 6;
  switch(value)
  {
    case 0:
      Serial.print(F("100mA'"));
      break;
    case 1:
      Serial.print(F("50mA'"));
      break;
    case 2:
      Serial.print(F("25mA'"));
      break;
    case 3:
      Serial.print(F("12.5mA'"));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("PGAIN:'"));
  value = i2cBuffer[0x8f]&0b00001100;
  value >>= 2;
  switch(value)
  {
    case 0:
      Serial.print(F("1x'"));
      break;
    case 1:
      Serial.print(F("2x'"));
      break;
    case 2:
      Serial.print(F("4x'"));
      break;
    case 3:
      Serial.print(F("8x'"));
      break;
  }
  Serial.print(F(","));
  Serial.print(F("AGAIN:'"));
  value = i2cBuffer[0x8f]&0b00000011;
  switch(value)
  {
    case 0:
      Serial.print(F("1x'"));
      break;
    case 1:
      Serial.print(F("4x'"));
      break;
    case 2:
      Serial.print(F("16x'"));
      break;
    case 3:
      Serial.print(F("64x'"));
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
  Serial.print(value);
  Serial.println(F("}"));
}

//Dump registers data and time proccess
void showRegisters()
{
  String controlFlags = "[";
  point_Buffer = 0;
  uint8_t counterFile, counterRow,value;
  Serial.println(F("\tDUMP_REGISTERS"));
  Serial.println(F("\t{"));
  for(counterFile = 0;counterFile<16;counterFile++){
    Serial.print(F("\t\t"));
    Serial.print(counterFile,16);
    Serial.print(":[");
    for(counterRow=0;counterRow<16;counterRow++){
      uint8_t pointerValue = counterFile*16+counterRow;
      uint8_t pointedValue = i2cBuffer[pointerValue];
      if(pointedValue<16)Serial.print("0");
      Serial.print(pointedValue,16);
      if(counterRow<15)Serial.print(",");
    }
    Serial.println("],");
    switch(counterFile)
    {
      case 8:
          if(i2cBuffer[0x80]&1)controlFlags += F("PON,");
          if(i2cBuffer[0x80]&2)controlFlags += F("AEN,");
          if(i2cBuffer[0x80]&4)controlFlags += F("PEN,");
          if(i2cBuffer[0x80]&8)controlFlags += F("WEN,");
          if(i2cBuffer[0x80]&16)controlFlags += F("AIEN,");
          if(i2cBuffer[0x80]&32)controlFlags += F("PIEN,");
          if(i2cBuffer[0x80]&64)controlFlags += F("GEN,");
          if(i2cBuffer[0x8d]&2)controlFlags += F("WLONG,");
          break;
       case 9:
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
          break;
       case 10:
          if(i2cBuffer[0xAB]&4)controlFlags += F("GFIFO_CLR,");
          if(i2cBuffer[0xAB]&2)controlFlags += F("GIEN,");
          if(i2cBuffer[0xAB]&1)controlFlags += F("GMODE,");
          if(i2cBuffer[0xAF]&2)controlFlags += F("GFOW,");
          if(i2cBuffer[0xAF]&1)controlFlags += F("GVALID,");
          break;
    }
  }
  //controlFlags += controlFlags.length();
  controlFlags += F("--],");
  Serial.print(F("\t\tFLAGS:"));
  Serial.println(controlFlags);
  Serial.print(F("\t\tTIMES:{"));
  Serial.print("i2c_time:'");
  Serial.print(i2cTime_proc/1000);
  Serial.print("ms.',Chip_ID:'0x");
  if(i2cBuffer[0x92]<16)Serial.print("0");
  Serial.print(i2cBuffer[0x92],16);
  time_proc = micros()-time_in;
  Serial.print("',proc_time:");
  Serial.print(time_proc/1000);
  Serial.println(F("}"));
  Serial.println(F("\t}"));
}
