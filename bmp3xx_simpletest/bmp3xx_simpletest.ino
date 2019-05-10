/***************************************************************************
  This is a library for the BMP3XX temperature & pressure sensor

  Designed specifically to work with the Adafruit BMP388 Breakout
  ----> http://www.adafruit.com/products/3966

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp; // I2C
//Adafruit_BMP3XX bmp(BMP_CS); // hardware SPI
//Adafruit_BMP3XX bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

double minimum = 100;
double maximum = 0;

double alt_filtered = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  //Serial.println("BMP388 test");

  if (!bmp.begin(0x77,&Wire,D1,D2)) {
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  //bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  Serial.println("18.0  22.0");
}

void loop() {
  if (! bmp.performReading()) {
    //Serial.println("Failed to perform reading :(");
    return;
  }
  //Serial.print("Temperature = ");
  //Serial.print(bmp.temperature);
  //Serial.println(" *C");

  //Serial.print("Pressure = ");
  //Serial.print(bmp.pressure / 100.0);
  //Serial.println(" hPa");

  //Serial.print("Approx. Altitude = ");
  //Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  //Serial.println(" m");

  double alt = bmp.readAltitude(SEALEVELPRESSURE_HPA);

  alt_filtered = filter( alt, alt_filtered, 1000 );  

  if( alt > 18.0 && alt < 22.0 )
    Serial.println(alt);

//  if( alt > 16 && alt < 24 ) 
//  {
//
//    if( alt < minimum )
//      minimum = alt;
//      
//    if( alt > maximum )
//      maximum = alt;
//  
//    double height = maximum - minimum;
//    
//    Serial.println(height); 
//  }
  
  //Serial.println();
  delay(100);  
}


double filter(double lengthOrig, double currentValue, int filter) {
  double lengthFiltered =  (lengthOrig + (currentValue * filter)) / (filter + 1);
  return lengthFiltered;  
}
