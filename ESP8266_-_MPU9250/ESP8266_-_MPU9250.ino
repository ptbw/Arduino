#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "quaternionFilters.h"
#include "MPU9250.h"

///////////////////////////////////////////////////////////////////
//Setup for the Accelerometer
///////////////////////////////////////////////////////////////////
#define declination 0  //http://www.ngdc.noaa.gov/geomag-web/#declination . This is the declinarion in the easterly direction in degrees.  
#define calibrateMagnetometer false //Setting requires requires you to move device in figure 8 pattern when prompted over serial port.  Typically, you do this once, then manually provide the calibration values moving forward.
MPU9250 myIMU;
//IMUWriter writer(kbits_256, 1, 64, 0x50);  //These are the arguments needed for extEEPROM library.  See their documentation at https://github.com/JChristensen/extEEPROM
IMUResult magResult, accResult, gyroResult, orientResult;

///////////////////////////////////////////////////////////////////
//Debug information
///////////////////////////////////////////////////////////////////
#define serialDebug true  // Set to true to get Serial output for debugging
#define baudRate 115200

///////////////////////////////////////////////////////////////////
//Determines how often we sample and send data
///////////////////////////////////////////////////////////////////
#define samplingRateInMillis 50

const char *ssid = "Riley";
const char *password = "01473405876";

ESP8266WebServer server ( 80 );

const int led = LED_BUILTIN;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(baudRate);
  Serial.println("Starting....");

  //Start IMU.  Assumes default SDA and SCL pins D2,D1 respectively.
  myIMU.begin();
   
  byte c;
  do
  {
    c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
    if (c != 0x73)
    {
      Serial.println("Failed to communicate with MPU9250");
      Serial.print("WHO_AM_I returned ");
      Serial.println(c, HEX);
      delay(500);
    }
  } while (c != 0x73);

  Serial.println("Successfully communicated with MPU9250");


  // Calibrate gyro and accelerometers, load biases in bias registers, then initialize MPU.
  myIMU.calibrate();  
  myIMU.init();
  if (calibrateMagnetometer)
    myIMU.magCalibrate();
  else
    myIMU.setMagCalibrationManually(-72.28, 81.37, -188.54);    //Set manually with the results of magCalibrate() if you don't want to calibrate at each device bootup.
                                                                //Note that values will change as seasons change and as you move around globe.  These values are for Ipswich UK in June.

  Serial.println("Accelerometer ready");

//  WiFi.begin(ssid,password);
//  client.setServer(server, port);

  accResult.setName("acc");
  gyroResult.setName("gyro");
  magResult.setName("mag");
  orientResult.setName("orien");
}

uint32_t lastSample = 0;
void loop() {
  // put your main code here, to run repeatedly:

  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {
    myIMU.readAccelData(&accResult);
    myIMU.readGyroData(&gyroResult);
    myIMU.readMagData(&magResult);
  }

  // Must be called before updating quaternions!
  myIMU.updateTime();
  MahonyQuaternionUpdate(&accResult, &gyroResult, &magResult, myIMU.deltat);
  //MadgwickQuaternionUpdate(&accResult, &gyroResult, &magResult, myIMU.deltat);
  readOrientation(&orientResult, declination);    


  if (millis() - lastSample > samplingRateInMillis)
  {
    lastSample = millis();
    
    if (serialDebug)
    {
      //accResult.printResult();
      //gyroResult.printResult();
      //magResult.printResult();
      // yaw,pitch,roll
      orientResult.printResult();
    }

    myIMU.sumCount = 0;
    myIMU.sum = 0;

  }  

}
