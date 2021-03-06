// I2C Scanner
// Written by Nick Gammon
// Date: 20th April 2011

#include <Wire.h>

const int sclPin = A5;
const int sdaPin = A4;

void setup() {

  Serial.begin (115200);
  byte error;

  // Leonardo: wait for serial port to connect
  while (!Serial)
  {
  }

  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin(sdaPin, sclPin);
    Wire.setClock(400000); //Increase I2C data rate to 400kHz

  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    error = Wire.endTransmission ();
    if (error == 0)
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      //delay (1);  // maybe unneeded?
     } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}  // end of setup

void loop() {}
