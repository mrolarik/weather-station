#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include "DHT.h"



//#define dht_dpin 0
//DHT dht(dht_dpin, DHTYPE);

// Update these with values suitable for your network.
const char* ssid = "olarik";
const char* password = "abcd1234";
const char* mqtt_server = "203.158.199.14"; //IP Address or Domain ของ MQTT Server
const char mqtt_username[50] = "admin"; //Username ของ MQTT Server
const char mqtt_password[50] = "1234"; //Password ของ MQTT Server
const int mqtt_port = 11883;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
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
  //dht.begin();
  
 pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
 Serial.begin(115200);
 setup_wifi();
 client.setServer(mqtt_server, mqtt_port);
 client.setCallback(callback);
}
void loop() {

//float h = dht.readHumidity();
//float t = dht.readTemperature();
//Serial.print("Current humidity = ");
//Serial.rint(h);
//Serial.print("% ");
//Serial.print("temperature = ");
//Serial.println("C ");
//delay(800);
  
if (!client.connected()) {
 reconnect();
 }
 client.loop();
long now = millis();
 if (now - lastMsg > 20000) {
 lastMsg = now;
 ++value;
 snprintf (msg, 75, "hello world #%ld", value);
 Serial.print("Publish message: ");
 Serial.println(msg);
 client.publish("nodemcu/client", msg);
 }
}
