/* 
webButton - A button that triggers a dweet event.
*/

#include <ESP8266WiFi.h>

//wifimanager:
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

const int HTTP_TIMEOUT = 500;
// Host
const char* host = "dweet.io";

const int buttonPin = D6;
const int ledPin = D5;

#include <Ticker.h>
Ticker ticker;


void fade(){
//variables for fading (static means they retain their values between runs)
static unsigned int fadeVal;
static bool fadeDir; //true=rising, false=falling

//change directions at endpoints
if(fadeVal==0) fadeDir=true;
else if (fadeVal>=1020) fadeDir=false;

analogWrite(ledPin, fadeVal); 

//Serial.print("Fadeval: ");
//Serial.println(fadeVal);

if(fadeDir) fadeVal+=10;
else fadeVal-=10;

}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());

  ticker.attach_ms(5,fade);
}


void setup()
{
	// Serial
  Serial.begin(115200);
  Serial.println("wake..");

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  analogWrite(ledPin, 0); //light on.

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
  if (!wifiManager.autoConnect("WebButton")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    alarm();

  }
  
  ticker.detach();
  analogWrite(ledPin, 0);

}

int timer;

void loop() 
{

  timer=millis();


  while(digitalRead(buttonPin)){ //wait for buttonpress.

	yield();

  }
  
  // Logging data to cloud
  Serial.print("Connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  client.setTimeout(HTTP_TIMEOUT);

  	bool connectionRetrycount=0;
    bool connected=false;

    while(!connected)
    {
    	connected=client.connect(host, httpPort);

    	if(connected) break;

    	connectionRetrycount++;
    	if (connectionRetrycount==3) alarm();
    
    	Serial.println("connection failed when pushing button. retry in 5..");
    	delay(5000);
    	
    }


    int requestRetrycount=0;
    bool ok;
 
    while(!ok)
    {

    	client.print(String("GET /dweet/for/chomskyMailCall?mail=yes") /*+ "&vBattTX=" + String(vBatt)*/ + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  				delay(10);

	    ok = client.find("\"mail\":\""); //dump until buttonstate

	    if(ok) break;

	    requestRetrycount++;

	    if(requestRetrycount==3) alarm();

	    delay(5000);
    	
    }
    

  String buttonState = client.readStringUntil('\"');
  Serial.print("ButtonState: ");
  Serial.println(buttonState);

  if(buttonState.equals("yes")){
 
 	ticker.attach_ms(5,fade);
    timer=millis();
 

   Serial.println("sucsessfully signaled buttonpress... 5 seconds till return to normal.");

   while(millis()<timer+5000) yield();      

   ticker.detach();
   analogWrite(ledPin, 0);


	}
  
  else alarm();
  



    

}

void alarm()
{
	Serial.println("alarm!");
	//FAILSAFE??
	while(1)
	{
	digitalWrite(ledPin, HIGH);
	delay(50);
	digitalWrite(ledPin, LOW);
	delay(450);
	}
}



