#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp8266.h> 
#include "DHT.h"

#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer; 

int led1 = D5;
int led2 = D6;
int led3 = D7;

char buf[20];
float t, h;
int led1_s, led2_s, led3_s;

char auth[] = "57924b8d171c44ef95ae65e7ebff0894";   //Key From Blynk
char ssid[] = "xxxx";                          //Wifi Name
char pass[] = "xxxx";                           //Wifi Password

void measure_dht(){
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (!isnan(h) || !isnan(t)) {
    sprintf(buf, "Temp: %02d C", (int)t);
    lcd.setCursor(0, 0);
    lcd.print(buf);
    sprintf(buf, " Hum: %02d", (int)h);
    lcd.setCursor(0, 1);
    lcd.print(buf);
    lcd.setCursor(9, 1);
    lcd.print("%");
  }
}

BLYNK_CONNECTED(){
    Blynk.syncAll();
}

BLYNK_READ(V0){
  if (!isnan(h) || !isnan(t)) {
    Blynk.virtualWrite(V0, (int)t);
  }
}
BLYNK_READ(V1){
  if (!isnan(h) || !isnan(t)) {
    Blynk.virtualWrite(V1, (int)h);
  }
}
BLYNK_WRITE(V2){
  led1_s = param.asInt();
}
BLYNK_WRITE(V3){
  led2_s = param.asInt();
}
BLYNK_WRITE(V4){
  led3_s = param.asInt();
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin();
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, measure_dht);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
 
}

void loop() {
  Blynk.run();
  timer.run();
  
  if(led1_s == 1){
    digitalWrite(led1, HIGH);
  }
  if(led1_s == 0){
    digitalWrite(led1, LOW);
  }
  if(led2_s == 1){
    digitalWrite(led2, HIGH);
  }
  if(led2_s == 0){
    digitalWrite(led2, LOW);
  }
  if(led3_s == 1){
    digitalWrite(led3, HIGH);
  }
  if(led3_s == 0){
    digitalWrite(led3, LOW);
  }
  
  Serial.print(t); Serial.print(" ");
  Serial.print(h); Serial.print(" ");
  Serial.print(led1_s); Serial.print(" ");
  Serial.print(led2_s); Serial.print(" ");
  Serial.print(led3_s); Serial.print(" ");
  Serial.println();
}
