#include <Wire.h>
#include <SerialCommand.h>

//extern uint8_t i2cBuffer;
extern SerialCommand sCmd;
extern uint8_t arduinoLED;
extern uint8_t i2cBuffer[];
extern uint8_t CHIP_ADDR;
extern boolean _showI2cBuffer;
extern int _poolDelay;

void unrecognized(const char *command);
void setSerialCommands();
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

//Set Serial Commands.
void setSerialCommands()
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
      aNumber <<= 6;
      aNumber |= (i2cBuffer[0x8e]&0b11000000);
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
  Wire.beginTransmission(CHIP_ADDR);
  Wire.write(0x80);
  Wire.write((i2cBuffer[0x80]&0b10111111)|0b01000000);
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
