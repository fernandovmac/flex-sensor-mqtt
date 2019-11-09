
#include <Arduino.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiSSLClient.h>
#include <PubSubClient.h>

//#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "sb02";        // your network SSID (name)
char pass[] = "IVeNtiptiC";    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status
const char* mqtt_server = "10.0.10.10"; //broker address

// WiFiSSLClient wifiSSLClient;
WiFiClient wifiClient;
PubSubClient client(wifiClient);


const int forcePin = A0;//FSR read
const int flexPin = A1;//Flex Sensor read
int pressure = 0;
int flex = 0;


long lastForceMsg = 0;
long lastFlexMsg = 0;
char msgForce[50];
char msgFlex [50];


void callback(char* topic, byte* payload, unsigned int length) //Broker Callback
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    client.publish("topic/test", "Hello world!");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
}

void reconnect() {
  // Loop until we're reconnected to the broker
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ATWINC1500Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected to broker!");
      // Once connected, publish an announcement...
      client.publish("topic/test", "Im alive!");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  WiFi.setPins(8,7,4,2); //Enabling the WiFi Pins of the board

  Serial.begin(9600);
  // while (!Serial) {
  //   ; // wait for serial port to connect. Needed for native USB port only
  // }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network!");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}


void loop() {
  // check the network connection once every 10 seconds:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now1 = millis();
  if (now1 - lastForceMsg  > 500) {
    pressure = analogRead(forcePin);
    flex = analogRead(flexPin); //Read and store analog value from Force Sensitive Resistance

    lastForceMsg = now1;
    //lastFlexMsg = now2;
     if (pressure || flex > 200) {
      Serial.print("Publish message: ");
      Serial.println("BANG!");
      client.publish("outPressure", "BANG!");
     }
  }
}
