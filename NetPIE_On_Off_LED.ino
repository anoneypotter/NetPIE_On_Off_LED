#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

// constants won't change. They're used here to 
// set pin numbers:
#define D0 16  // USER LED Wake
#define D1 5  
#define D2 4
#define D3 0   // FLASH
#define D4 2   // TXD1
#define D5 14
#define D6 12
#define D7 13  // RXD2  Serial.swap(); after Serial.begin();. 
#define D8 15  // TXD2  Serial.swap(); after Serial.begin();. 
#define D9 3   // RXD0
#define D10 1  // TXD0

#define buttonPin  D1     // the number of the pushbutton pin
#define ledPin  D0        // the number of the LED pin

const char* ssid     = "Your WiFi name";
const char* password = "Your WiFi Password";

#define APPID       "App id"
#define GEARKEY     "App Key"
#define GEARSECRET  "App Secret"
#define SCOPE       ""

WiFiClient client;
AuthClient *authclient;

MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) 
{
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  char strState[msglen];
  for (int i = 0; i < msglen; i++) 
  {
    strState[i] = (char)msg[i];
    Serial.print((char)msg[i]);
  }
  Serial.println();

  String stateStr = String(strState).substring(0, msglen);

  if (stateStr == "ON") 
  {
    digitalWrite(ledPin, LOW);
    microgear.chat("controllerplug", "ON");
  } else if (stateStr == "OFF") 
  {
    digitalWrite(ledPin, HIGH);
    microgear.chat("controllerplug", "OFF");
  }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) 
{
  Serial.println("Connected to NETPIE...");
  microgear.setName("pieplug");
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) 
{
    Serial.print("Found new member --> ");
    for (int i=0; i<msglen; i++)
    {
        Serial.print((char)msg[i]);
    }
    Serial.println();  
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) 
{
    Serial.print("Lost member --> ");
    for (int i=0; i<msglen; i++)
    {
      Serial.print((char)msg[i]);
    }
    Serial.println();
}

void setup() 
{
    /* Event listener */
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(PRESENT,onFoundgear);
    microgear.on(ABSENT,onLostgear);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
       delay(250);
       Serial.print(".");
    }

    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //uncomment the line below if you want to reset token -->
    microgear.resetToken();
    microgear.init(GEARKEY,GEARSECRET,SCOPE);
    microgear.connect(APPID);

    pinMode(ledPin,OUTPUT);
    digitalWrite(D0,HIGH); // Turn off LED
}


void loop() 
{
  if (microgear.connected()) 
  {
    microgear.loop();
    Serial.println("connect...");
  } 
  else 
  {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
  delay(1000);
}
