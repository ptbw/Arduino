//IoT LoRa Node Shield ABP Example
//By Ryan Walmsley May 2019
//(C) Pi Supply 2019
//Based off RAK Wireless Code

#include "RAK811.h"
#include "SoftwareSerial.h"
#define WORK_MODE LoRaWAN   //  LoRaWAN or LoRaP2P
#define JOIN_MODE ABP    //  OTAA or ABP
#if JOIN_MODE == OTAA
String DevEui = "008666220C73EFAA";
String AppEui = "70B3D57ED001E9ED";
String AppKey = "A3D1CC6B04761B9DAF6DFC0F05F5579D";
#else JOIN_MODE == ABP
String NwkSKey = "3074C545137A6663F173B3DEDB325CEB"; // Fill this out
String AppSKey = "310A5CC877697B69D43CA969F978B03C"; // Fill this out
String DevAddr = "26011FD5"; // Fill this out
#endif
#define TXpin 11   // Set the virtual serial port pins
#define RXpin 10
#define DebugSerial Serial
SoftwareSerial RAKSerial(RXpin,TXpin);    // Declare a virtual serial port
char* buffer = "72616B776972656C657373";
bool InitLoRaWAN(void);
RAK811 RAKLoRa(RAKSerial,DebugSerial);

int RESET_PIN = 12;


void setup() {
 //Define Reset Pin
 pinMode(RESET_PIN, OUTPUT);
 //Setup Debug Serial on USB Port
 DebugSerial.begin(9600);
 while(DebugSerial.read()>= 0) {}
 while(!DebugSerial);
 //Print debug info
 DebugSerial.println("StartUP");
 DebugSerial.println("Reset");
 //Reset the RAK Module
 digitalWrite(RESET_PIN, LOW);   // turn the pin low to Reset
 digitalWrite(RESET_PIN, HIGH);    // then high to enable
 DebugSerial.println("Success");
 RAKSerial.begin(9600); // Arduino Shield
 delay(100);
 DebugSerial.println(RAKLoRa.rk_getVersion());
 delay(200);
 DebugSerial.println(RAKLoRa.rk_getBand());
 delay(200);

 while (!InitLoRaWAN());

}
bool InitLoRaWAN(void)
{
  RAKLoRa.rk_setWorkingMode(WORK_MODE);
  RAKLoRa.rk_recvData();
  RAKLoRa.rk_recvData();
  if ( RAKLoRa.rk_recvData() == "OK")
  {
    if (RAKLoRa.rk_initABP(DevAddr, NwkSKey, AppSKey))
    {
      Serial.println("You init ABP parameter is OK!");
      if (RAKLoRa.rk_joinLoRaNetwork(JOIN_MODE))
      {
        Serial.println("You join Network success!");
        return true;
      }
    }
  }
  return false;
}

void loop() {
  int packetsflag = 1; // 0: unconfirmed packets, 1: confirmed packets
  if (RAKLoRa.rk_sendData(packetsflag, 1, buffer))
  {
    for (unsigned long start = millis(); millis() - start < 90000L;)
    {
      String ret = RAKLoRa.rk_recvData();
      if (ret.startsWith(STATUS_TX_COMFIRMED) || ret.startsWith(STATUS_TX_UNCOMFIRMED))
      {
        DebugSerial.println("Send data ok!");
        delay(5000);
        return;
      }
    }
    DebugSerial.println("Send data error!");
    while(1);
  }
}
