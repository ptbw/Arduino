
#if 0
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);
#else

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
#endif

const int ledPin =  LED_BUILTIN;// the number of the LED pin

void setup(void) {
    while (!Serial) {
     }

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    Serial.begin(9600);
    Serial.println("NDEF Reader");
    nfc.begin();
}

void loop(void) {

    digitalWrite(ledPin, HIGH);
    Serial.println("\nScan a NFC tag\n");
    if (nfc.tagPresent())
    {
        digitalWrite(ledPin, LOW);
        NfcTag tag = nfc.read();
        tag.print();
    }    
    delay(5000);
}
