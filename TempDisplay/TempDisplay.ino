#include <Esplora.h>
#include <TFT.h>
#include <SPI.h>
 
char tempCelsius[3];
char tempFahrenheit[3];
 
void setup()
{
//setup the LCD
EsploraTFT.begin();
EsploraTFT.background(0,0,0);
EsploraTFT.stroke(255,255,255);
EsploraTFT.setTextSize(2);
EsploraTFT.text("Celsius",0,0);
EsploraTFT.text("Fahrenheit",0,60);
}
 
void loop()
{
// read the temperature sensor in Celsius, then Fahrenheit:
String celsius = String(Esplora.readTemperature(DEGREES_C));
String fahrenheit = String(Esplora.readTemperature(DEGREES_F));
celsius.toCharArray(tempCelsius, 3);
fahrenheit.toCharArray(tempFahrenheit, 3);
EsploraTFT.stroke(255,255,255);
EsploraTFT.text(tempCelsius, 0, 30);
EsploraTFT.text(tempFahrenheit, 0, 90);
// wait a second before reading again:
delay(1000);
EsploraTFT.stroke(0,0,0);
EsploraTFT.text(tempCelsius, 0, 30);
EsploraTFT.text(tempFahrenheit, 0, 90);
}
