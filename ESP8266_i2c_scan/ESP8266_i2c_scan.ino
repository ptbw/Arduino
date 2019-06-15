// --------------------------------------
// i2c_scanner
//
// Use Wemos D1 and Mini board file 
//

#include <Wire.h>


const int sclPin = D1;
const int sdaPin = D2;

//const int sclPin = 22; // was 5;
//const int sdaPin = 21; // was 4;

void setup()
{
  Wire.begin(sdaPin, sclPin);
  //Wire.begin();

  Serial.begin(115200);
  // Wait for serial port to connect
  while (!Serial) 
  {
  }
  Serial.println("\nI2C Scanner");
}


void loop()
{
  byte error, address;
  int nDevices;
  
  Serial.println("Scanning...");
  
  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
        Serial.print(address,HEX);
        Serial.println(" !");      
        nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknow error at address 0x");
      if (address<16) 
        Serial.print("0");
        Serial.println(address,HEX);
    } 
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
  
  delay(1000); // wait 5 seconds for next scan
} 
