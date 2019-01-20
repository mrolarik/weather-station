#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor

#define DHTTYPE DHT11   // DHT 11
#define DHTPin D3
DHT dht(DHTPin, DHTTYPE);

//const int lamp = 12;     //D6 = GPIO 12
#define lamp D6
//#define LED D6
#define lampOnline D7


// Update these with values suitable for your network.
const char* ssid = "Mrolarik";
const char* password = "olarik_home";
const char* mqtt_server = "203.158.199.14"; //IP Address or Domain ของ MQTT Server
const char mqtt_username[50] = "admin"; //Username ของ MQTT Server
const char mqtt_password[50] = "1234"; //Password ของ MQTT Server
const int mqtt_port = 11883;

const char node_mcu_id[50] = "Faculty-of-Informatics-ITxxx"; // node_id
String ipaddress;

WiFiClient espClient;
PubSubClient client(espClient);

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;


void setup_wifi() {
   delay(10);
   // We start by connecting to a WiFi network
   Serial.println();
   Serial.print("Connecting to ");
   Serial.println(ssid);
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
   }

   randomSeed(micros());
   Serial.println("");
   Serial.println("WiFi connected");
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());

   digitalWrite(lampOnline, HIGH);

   ipaddress = WiFi.localIP().toString();
}

void callback(char* topic, byte* message, unsigned int length) {
   Serial.print("Message arrived [");
   Serial.print(topic);
   Serial.print("] ");
   String messageTemp;
   
   for (int i = 0; i < length; i++) {
      Serial.print((char)message[i]);
      messageTemp += (char)message[i];
   }
   Serial.println();
   
   if(String(topic)==String("room/lamp")){
    Serial.print("Changing Room lamp to ");
    if(messageTemp == "on"){
      digitalWrite(lamp, HIGH);
      Serial.print("On");
    }
    else if(messageTemp == "off"){
      digitalWrite(lamp, LOW);
      Serial.print("Off");
    }
   }
   Serial.println();
}

void reconnect() {
   // Loop until we’re reconnected
   while (!client.connected()) {
      Serial.print("Attempting MQTT connection…");
      if(client.connect("ESP_Office")) {
        Serial.println("connected");
        client.subscribe("room/lamp");
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println("try again in 5 seconds");
        delay(5000);
      }
    }
}

void setup() {
  //pinMode(LED, OUTPUT);
  pinMode(lamp, OUTPUT);
  pinMode(lampOnline, OUTPUT);
  dht.begin();
  Serial.println("humidity and temperature \n");
  delay(1000);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
   
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
  if(!client.connected()) {
    reconnect();
  }
  if(!client.loop()) {
    client.connect("ESP8266Client");
  }
    
  now = millis();

  if(now-lastMeasure > 5000) {
    lastMeasure = now;
    //float h = dht.readHumidity();
    //float t = dht.readTemperature();
    //float f = dht.readTemperature(true);

    if(!isnan(h) || !isnan(t)) {
      //Serial.println("Failed to read from DHT sensor!");
      //return;
     Serial.print("Humidity: ");
     Serial.print(h);
     Serial.print(" %\t Temperature: ");
     Serial.print(t);
     Serial.println("C ");
    

    // Computes temperature values in Celsius
    float hic = dht.computeHeatIndex(t, h, false);
    static char temperatureTemp[7];
    dtostrf(hic, 6, 2, temperatureTemp);

    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);

    // Publishes temperature and humidity values
    client.publish("room/temperature", String(t).c_str());
    client.publish("room/humidity", String(h).c_str());
    client.publish("room/nodeid", node_mcu_id);
    client.publish("room/nodeipaddress", String(ipaddress).c_str());
    
    }
    //Serial.println("Humidity: ");
    //Serial.print(h);
    //Serial.print(" %\t Temperature: ");
    //Serial.print(t);
    //Serial.print(" *C ");
    //Serial.print(f);
    //Serial.print(" *F\t Heat index: ");
    //Serial.print(hic);
    //Serial.println(" *C ");
   
  }
}
