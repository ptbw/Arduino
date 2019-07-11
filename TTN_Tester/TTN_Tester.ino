/*******************************************************************************
   Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman

   Permission is hereby granted, free of charge, to anyone
   obtaining a copy of this document and accompanying files,
   to do whatever they want with them without any restriction,
   including, but not limited to, copying, modification and redistribution.
   NO WARRANTY OF ANY KIND IS PROVIDED.

   This example sends a valid LoRaWAN packet with payload "Hello,
   world!", using frequency and encryption settings matching those of
   the The Things Network.

   This uses ABP (Activation-by-personalisation), where a DevAddr and
   Session keys are preconfigured (unlike OTAA, where a DevEUI and
   application key is configured, while the DevAddr and session keys are
   assigned/generated in the over-the-air-activation procedure).

   Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
   g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
   violated by this sketch when left running for longer)!

   To use this sketch, first register your application and device with
   the things network, to set or generate a DevAddr, NwkSKey and
   AppSKey. Each device should have their own unique values for these
   fields.

   Do not forget to define the radio type correctly in config.h.

   This is the software of teh small TTN Tester in the 3-D box

 *******************************************************************************/

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
//#include <credentials.h>

#define CFG_eu868

// LoRaWAN NwkSKey, network session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.

#ifdef CREDENTIALS
static const PROGMEM u1_t NWKSKEY[16] = NWKSKEY1;
static const PROGMEM u1_t APPSKEY[16] = APPSKEY1;
static const PROGMEM u4_t DEVADDR = DEVADDR1;
#else
static const u1_t NWKSKEY[16] = { 0x0C, 0xAD, 0x38, 0xE9, 0x39, 0x10, 0xA0, 0x6D, 0xA6, 0xF5, 0x95, 0xC4, 0xC5, 0xBD, 0x0A, 0xA8 };
static const u1_t APPSKEY[16] = { 0x8C, 0x9E, 0x0B, 0xA8, 0xC3, 0xC8, 0x71, 0xA8, 0x8A, 0x45, 0xB2, 0xA6, 0x76, 0xE2, 0x6E, 0x8B };
static const u4_t DEVADDR = 0x26011128;
#endif

unsigned long entry = millis();

#define GREENLED 3
#define REDLED 8
#define SFSWITCH 4
#define RUNPIN 5

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

byte counter = 0;
static osjob_t sendjob;
bool switchState = true, oldSwitchState = false;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 20;

// Pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 9,
  .dio = {2, 6, 7},
};

void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      break;
    case EV_RFU1:
      Serial.println(F("EV_RFU1"));
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;

    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      Serial.println(LMIC.dataLen);
      if (LMIC.dataLen) {
        // data received in rx slot after tx
        uint8_t downlink[LMIC.dataLen];
        memcpy(&downlink, &(LMIC.frame + LMIC.dataBeg)[0], LMIC.dataLen);
        // Turn on/off fan if we get the magic number
        if ( downlink[0] == 0x31 ) {
          // digitalWrite(FanPin, HIGH);
        }
        else {
          // digitalWrite(FanPin, LOW);
        }

        Serial.print(F("Data Received: "));
        Serial.write(LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
        Serial.println();

      }
      // Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      digitalWrite(REDLED, HIGH);
      break;
    /*
      case EV_TXCOMPLETE:
      Serial.print((millis() - entry) / 1000);  Serial.print(" ");
      entry = millis();
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK)
      Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
      Serial.println(F("Received "));
      Serial.println(LMIC.dataLen);
      Serial.println(F(" bytes of payload"));
      }
      // Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      digitalWrite(REDLED, HIGH);
      break;
    */
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
    default:
      Serial.println(F("Unknown event"));
      break;
  }
}

void do_send(osjob_t* j) {
  byte buffer[32];
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    // Prepare upstream data transmission at the next possible time.
    String message = "" + String(counter);
    message.getBytes(buffer, message.length() + 1);
    counter++;
    Serial.println("Sending: " + message);
    LMIC_setTxData2(1, (uint8_t*) buffer, message.length() , 0);
    Serial.println(F(" Packet queued"));
    digitalWrite(REDLED, LOW);
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
  Serial.begin(115200);
  while (!Serial){
  }
  Serial.println(F("Starting"));

  pinMode(GREENLED, OUTPUT);
  pinMode(REDLED, OUTPUT);
  pinMode(SFSWITCH, INPUT_PULLUP);
  pinMode(RUNPIN, INPUT_PULLUP);
  digitalWrite(REDLED, HIGH);
  digitalWrite(GREENLED, HIGH);

#ifdef VCC_ENABLE
  // For Pinoccio Scout boards
  pinMode(VCC_ENABLE, OUTPUT);
  digitalWrite(VCC_ENABLE, HIGH);
  delay(1000);
#endif

  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  // Set static session parameters. Instead of dynamically establishing a session
  // by joining the network, precomputed session parameters are be provided.
#ifdef PROGMEM
  // On AVR, these values are stored in flash and only copied to RAM
  // once. Copy them to a temporary buffer here, LMIC_setSession will
  // copy them into a buffer of its own again.
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
#else
  // If not running an AVR with PROGMEM, just use the arrays directly
  LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
#endif

#if defined(CFG_eu868)
  Serial.println("European Channels");
  for (int i = 1; i <= 8; i++) LMIC_disableChannel(i);
#elif defined(CFG_us915)
  // NA-US channels 0-71 are configured automatically
  // but only one group of 8 should (a subband) should be active
  // TTN recommends the second sub band, 1 in a zero based count.
  // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json
  LMIC_selectSubBand(1);
#endif

  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;

  // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
  if (digitalRead(SFSWITCH) == HIGH) {
    Serial.println("SF7");
    LMIC_setDrTxpow(DR_SF7, 14);
  }
  else  {
    Serial.println("SF10");
    LMIC_setDrTxpow(DR_SF10, 14);
  }

  // Start job
  do_send(&sendjob);
}

void loop() {
  while (digitalRead(RUNPIN) == HIGH) {
    digitalWrite(GREENLED, HIGH);
    delay(10);
  }
  switchState = digitalRead(SFSWITCH);
  if (switchState != oldSwitchState) {
    delay(100);
    // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
    if (digitalRead(SFSWITCH) == HIGH) {
      Serial.println("SF7");
      LMIC_setDrTxpow(DR_SF7, 14);
    }
    else  {
      Serial.println("SF10");
      LMIC_setDrTxpow(DR_SF10, 14);
    }
    oldSwitchState = switchState;
  }
  digitalWrite(GREENLED, LOW);
  os_runloop_once();

}