#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "secrets.h"

#define LED_BUILTIN 0

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, LOGIN, PASSWD);

Adafruit_MQTT_Publish SendUid = Adafruit_MQTT_Publish(&mqtt,"UIDScanned");

PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 10;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(1000);  // wait 1 second
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}


// Built in led is lit when off!
void flash(int times)
{
  for( int i=0;i<times;i++)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
    digitalWrite(LED_BUILTIN, HIGH);  
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);

  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    flash(1);    
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  Serial.println("Hello!");
  digitalWrite(LED_BUILTIN, HIGH);
  nfc.begin();
  
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while(1);    
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);  
  
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
    
  Serial.println("Waiting for an ISO14443A card");
  flash(5);
  
}

void loop() {
  uint8_t data[17];                         // Array to store block data during reads
  data[16] = 0;

  // Keyb on NDEF and Mifare Classic should be the same
  uint8_t keyuniversal[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  
  // put your main code here, to run repeatedly:
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  char UID[32];
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  
  if (success) {
   
    nfc.mifareclassic_ReadDataBlock (8, data);
    nfc.PrintHexChar(data, 16);

    for (uint8_t i=0; i < 16; i++) 
    {
      Serial.print(data[i]);
    }
    Serial.println("");
    
    digitalWrite(LED_BUILTIN, LOW);
    sprintf(UID,"%02X%02X%02X%02X",uid[0],uid[1],uid[2],uid[3]);
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) 
    {
      Serial.print(" 0x");Serial.print(uid[i], HEX); 
    }
    Serial.println("");   

//    MQTT_connect();
//    Serial.print(F("\nSending UID "));
//    Serial.print(UID);
//    Serial.print("...");
//    if (!SendUid.publish(UID)) {
//      Serial.println(F("Failed"));     
//    } else {
//      Serial.println(F("OK!"));
//    }  
    // Wait 1 second before continuing
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);        
  }
  else
  {
    flash(2);
  }
}
