/* 
mailCall - Checks dweet.io buttonpress, and lights up WS2812 LEDs if event has been triggered
*/

#include <ESP8266WiFi.h>

//wifimanager:
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

// WiFi settings
//const char* ssid = "robottoAP";
//const char* password = "dillerdiller";
const int HTTP_TIMEOUT = 500;
// Host
const char* host = "dweet.io";


#include <WS2812FX.h>

#define LED_COUNT 1
#define LED_PIN D2
#define BUTTON_PIN D1
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);


// Time to sleep (in seconds):
static int sleepTime = 60000;

#include <Ticker.h>
Ticker ticker;

void tick()
{
  ws2812fx.service();
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP().toString());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());

  
}

void setup() 
{

    pinMode(BUTTON_PIN,INPUT_PULLUP);
  // Serial
  Serial.begin(115200);
  Serial.println("wake..");

  ws2812fx.init();
  ws2812fx.setBrightness(64);
  ws2812fx.setSpeed(100);
  ws2812fx.setMode(FX_MODE_BREATH);
  ws2812fx.setColor(0x0000FF);
  ws2812fx.start();


  ticker.attach(0.1, tick); //async task to handle 2812FX service routine, 
  
    // Change MAC to 5c:cf:7f:01:8d:31 (gadget)  SSID: AU-Gadget PW: augadget
    uint8_t mac[6] {0x5c, 0xcf, 0x7f, 0x01, 0x8d, 0x31};
    wifi_set_macaddr(STATION_IF, mac);
    // wifi_set_macaddr(SOFTAP_IF, mac);
    
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  wifiManager.setConnectTimeout(30); //try to connect to known wifis for a long time before defaulting to AP mode

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("mailCall")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    alarm();
  }
  
  Serial.println("");
  Serial.print("WiFi connected. IPV4:");
  
  // Print the IP address
  Serial.println(WiFi.localIP().toString());
  
}

void loop() 
{

    // Logging data to cloud
    Serial.print("Connecting to ");
    Serial.println(host);
    
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed to webserver");
       
         alarm();
  
    }
    
    // This will send the request to the server
    client.print(String("GET /get/latest/dweet/for/chomskyMailCall") + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");
    delay(200);
    
   
    //char endOfHeaders[] = "\r\n\r\n";
    client.setTimeout(HTTP_TIMEOUT);
    //bool ok = client.find(endOfHeaders);
    
    bool ok = client.find("\"mail\":\""); //dump until buttonstate
  
    if (!ok) {
      Serial.println("No response or invalid response!"); //the button was probably not pressed.
  
      delay(sleepTime);
      return; //reset loop.
    }
  
  
    String mailState = client.readStringUntil('\"');
  
    Serial.print("Mail: ");
    Serial.println(mailState);
  
    delay(200);
  

  
      if(mailState.equals("yes")){
  
  
      Serial.println("MAILCALL!!");
      
      ticker.detach();
	  
	  ws2812fx.setBrightness(255);
      ws2812fx.setSpeed(255);
      ws2812fx.setColor(0xFFFFFF);
      ws2812fx.setMode(FX_MODE_BREATH);
      
      while(digitalRead(BUTTON_PIN)==HIGH) { //wait for someone to pick up the mail
            delay(10);
            yield();
        	ws2812fx.service();
              }  

    
              Serial.println("disarming mailcall");
              //release the button.
      
              //WiFiClient releaseClient;
              if (!client.connect(host, httpPort)) {
              Serial.println("connection failed to webserver");
              
              alarm();
      
                }
                client.print(String("GET /dweet/for/chomskyMailCall?mail=no") + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" + 
                     "Connection: close\r\n\r\n");
                delay(10);

                ws2812fx.setBrightness(128);
                ws2812fx.setSpeed(100);
                ws2812fx.setMode(FX_MODE_BREATH);
                ws2812fx.setColor(0x0000FF);
                ticker.attach(0.1, tick); //async task to handle 2812FX service routine, 
  
      return; //reset loop
        
      }
  
      
    delay(sleepTime); //should be enough with 60000

}

void alarm()
{
	  Serial.println("ALARM! awaiting reset");
	  ticker.detach();
	  ws2812fx.setBrightness(255);
  	  ws2812fx.setSpeed(255);
      ws2812fx.setColor(0x00FF00);
      
      while(digitalRead(BUTTON_PIN)==HIGH) {
        	delay(10);
      		ws2812fx.service();
          }  
     Serial.println("RESETTING!");
      ESP.reset();
}
