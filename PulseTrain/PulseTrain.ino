// include the Esplora library
#include <Esplora.h>
#include <TFT.h>
#include <SPI.h>
 

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store the last time the pulse was updated

// Variables will change :
int pulseState = LOW;                  // ledState used to set the pulse

long interval = 1000;              // interval at which to blink (milliseconds)

char intervalChar[6];

void setup() {
  //setup the LCD
  EsploraTFT.begin();
  EsploraTFT.background(0,0,0);
  EsploraTFT.stroke(255,255,255);
  EsploraTFT.setTextSize(2);
  EsploraTFT.text("Interval",0,0);

  Serial.begin(9600);

  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // read the sensor into a variable
  int slider = Esplora.readSlider();

  Serial.print(slider,HEX);
  Serial.println("!");

  // convert the sensor readings to light levels
  byte bright  = slider/4;

  Serial.print(bright);
  Serial.println("!");
  
  // write the light levels to the Red LED
  Esplora.writeRed(bright);

  interval = 200 + ( 0x3FF - slider);
  String temp = String(interval);
  temp.toCharArray(intervalChar,6);
  EsploraTFT.stroke(255,255,255);
  EsploraTFT.text(intervalChar, 0, 30);

  // add a small delay to keep the LED from flickering:
  delay(10);

  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (pulseState == LOW) {
      pulseState = HIGH;
    } else {
      pulseState = LOW;
    }


    digitalWrite(11, pulseState);
    digitalWrite(13, pulseState);
  }
  EsploraTFT.stroke(0,0,0);
  EsploraTFT.text(intervalChar, 0, 30);
}
