#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11

#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE);

// Update these with values suitable for your network.
const char* ssid = "xxx";
const char* password = "xxx";
const char* mqtt_server = "111.111.111.111"; //IP Address or Domain ของ MQTT Server
const char mqtt_username[50] = "admin"; //Username ของ MQTT Server
const char mqtt_password[50] = "1234"; //Password ของ MQTT Server
const int mqtt_port = 1883;

const char node_mcu_id[50] = "Faculty-of-Informatics-ITxxx"; // node_id
String ipaddress;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[100];
int value = 0;

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

   ipaddress = WiFi.localIP().toString();
}

void callback(char* topic, byte* payload, unsigned int length) {
   Serial.print("Message arrived [");
   Serial.print(topic);
   Serial.print("] ");
   for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
   }
   Serial.println();
   
   // Switch on the LED if an 1 was received as first character
   if ((char)payload[0] == '1') {
      digitalWrite(BUILTIN_LED, LOW); // Turn the LED on (Note that LOW is the voltage level
      // but actually the LED is on; this is because
      // it is acive low on the ESP-01)
   } else {
      digitalWrite(BUILTIN_LED, HIGH); // Turn the LED off by making the voltage HIGH
   }
}

void reconnect() {
   // Loop until we’re reconnected
   while (!client.connected()) {
      Serial.print("Attempting MQTT connection…");
      // Create a random client ID
      String clientId = "ESP8266Client-";
      clientId += String(random(0xffff), HEX);
      // Attempt to connect
      if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) { //ทำการ Connect โดยใช้ Username&Password
         Serial.println("connected");
         // Once connected, publish an announcement…
         client.publish("nodemcu/client", "hello world"); //ส่งข้อความไปยัง Topic ชื่อ "mynew/test"
         // … and resubscribe
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
   dht.begin();
   Serial.println("humidity and temperature \n\n");
   delay(1000);
  
   pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
   Serial.begin(115200);
   setup_wifi();
   client.setServer(mqtt_server, mqtt_port);
   client.setCallback(callback);
}

void loop() {

   float h = dht.readHumidity();
   float t = dht.readTemperature();    
   
   if (!client.connected()) {
      reconnect();
   }
   client.loop();
   
   long now = millis();
    if (now - lastMsg > 5000) {
       lastMsg = now;
       ++value;
       
       // /*   
       if(!isnan(t)||!isnan(h)){      
          Serial.print("Current humidity = ");
          Serial.print(h);
          Serial.print("%  ");
          Serial.print("temperature = ");
          Serial.print(t); 
          Serial.println("C  ");
       }
       delay(800);   
       // */       

       snprintf (msg, 75, "%s,%s,%ld,%.2f,%.2f", node_mcu_id,String(ipaddress).c_str(),value,t,h);
       //snprintf (msg, 75, "%s,%ld,%.2f,%.2f", node_mcu_id,value,t,h);
       Serial.print("Publish message: ");
       Serial.println(msg);
       client.publish("nodemcu/client", msg);
   }   
}
