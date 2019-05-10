#include <TwitterApi.h>

/*******************************************************************
 *  An Example of getting data back from the TwitterAPI            *
 *  https://github.com/witnessmenow/arduino-twitter-api            * 
 *                                                                 *
 *  Written by Brian Lough                                         *
 *******************************************************************/

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//------- Replace the following! ------
char ssid[] = "PhilW";       // your network SSID (name)
char password[] = "Ardvark12";  // your network key

// Normally we would use these to generate the bearer token but its not working yet :/
// Use steps on the readme to generate the Bearer Token

#define BEARER_TOKEN "AAAAAAAAAAAAAAAAAAAAADJK%2BQAAAAAApkUnsVPSq%2Fp7ReWMN4bAF4oFes8%3D3k2u5odxhpyhqrDY9SQAei02uLbccNIP44Z42Tbe0v7BKyKCvn"

//AAAAAAAAAAAAAAAAAAAAADJK%2BQAAAAAApkUnsVPSq%2Fp7ReWMN4bAF4oFes8%3D3k2u5odxhpyhqrDY9SQAei02uLbccNIP44Z42Tbe0v7BKyKCvn
//Using curl to get bearer token
// curl -u "$CONSUMER_KEY:$CONSUMER_SECRET" \
//    --data 'grant_type=client_credentials' \
//    'https://api.twitter.com/oauth2/token'

WiFiClientSecure client;
TwitterApi api(client);

unsigned long api_mtbs = 15000; //mean time between api requests
unsigned long api_lasttime = 0;   //last time api request has been done
bool firstTime = true;

//Inputs

String screenName = "ptbw2000";
String hashtag = "#ColourMyHat";
bool haveBearerToken = false;

#define PIN            D5
#define NUMPIXELS      21

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define RED   pixels.Color(150,0,0)
#define GREEN pixels.Color(0,150,0)
#define BLUE  pixels.Color(0,0,150)
#define WHITE  pixels.Color(150,150,150)
#define BLACK  pixels.Color(0,0,0)

void setup() {

  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  pixels.begin(); // This initializes the NeoPixel library.

  bool led = true;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    if(led) pixels.setPixelColor(NUMPIXELS - 1, RED );
    else pixels.setPixelColor(NUMPIXELS - 1, BLACK );
    pixels.show(); // This sends the updated pixel color to the hardware.
    led = !led;
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  

  api.setBearerToken(BEARER_TOKEN);
  haveBearerToken = true;  
  getTwitterHashTag(hashtag);
  
}

void setColour(uint32_t first, uint32_t second, uint32_t third )
{
   Serial.print( first );
   Serial.print(" ");
   Serial.print( second );
   Serial.print(" ");
   Serial.println( third );  

//   for(int i = 0; i< NUMPIXELS; i += 3 )
//   {
//      pixels.setPixelColor(i, first );
//      pixels.setPixelColor(i+1, second );
//      pixels.setPixelColor(i+2, third );
//   } 

   // Using 3 neo pixel rings 12, 8 and 1 pixel in each
   for(int i = 0; i< 12; i++ )
   {
      pixels.setPixelColor(i, first );
   }
   for(int i = 12; i< 20; i++ )
   {
      pixels.setPixelColor(i, second );
   }
   pixels.setPixelColor(20, third );

   pixels.show(); // This sends the updated pixel color to the hardware.   
}

void getTwitterHashTag(String hashtag)
{
    Serial.println("Finding last " + hashtag);
    String responseString = api.getHashTag(hashtag);
    Serial.println(responseString);
    
    // Locate '"text":'
    int start = responseString.indexOf("\"text\":");
    int finish = responseString.indexOf(",\"truncated\":");
    if( start != -1 && finish != -1) {
      String text = responseString.substring( start + 8, finish -1 );
      text.toLowerCase();
      Serial.println(text);

      int red = text.indexOf("red");
      int blue = text.indexOf("blue");
      int green = text.indexOf("green");
      int white = text.indexOf("white");

      Serial.print( red );
      Serial.print(" ");
      Serial.print( blue );
      Serial.print(" ");
      Serial.print( green );
      Serial.print(" ");
      Serial.println( white );  

      if( blue == -1 && green == -1 && red != -1) setColour( RED, RED, RED );
      else if( red == -1 && green == -1 && blue != -1) setColour( BLUE, BLUE, BLUE );
      else if( blue == -1 && red == -1  && green != -1) setColour( GREEN, GREEN, GREEN );
      else if( red ==-1 && blue == -1 && green == -1 && white != -1) setColour( WHITE, WHITE, WHITE );
      else if( red ==-1 && blue == -1 && green == -1 && white == -1) setColour( BLACK, BLACK, BLACK );

      else if(red < blue && red < green && green < blue) setColour( RED, GREEN, BLUE );
      else if(red < blue && red < green && blue < green) setColour( RED, BLUE, GREEN );
      
      else if(blue < red && blue < green  && red < green) setColour( BLUE, RED, GREEN );
      else if(blue < red && blue < green  && green < red) setColour( BLUE, GREEN, RED );
      
      else if(green < red && green < blue && red < blue) setColour( GREEN, RED, BLUE );
      else if(green < red && green < blue && blue < red) setColour( GREEN, BLUE, RED );
           
    }
}

void loop() {
  if(haveBearerToken){
    if ((millis() > api_lasttime + api_mtbs))  {
      getTwitterHashTag(hashtag);
      api_lasttime = millis();
    }
  }
}
