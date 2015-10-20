/*

A simple getting started ESP8266 Project with ATCommands.

This reads the data of a BMP sensor and spreads the word on thingspeak.

Based on code originally posted by :
Mahesh Venkitachalam,electronut.in

Support of BMP and 2 fields added by Rohit Gupta,rohitg.in

*/



#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SoftwareSerial.h>
#include <stdlib.h>
int ledPin = 13;
// replace with your channel's thingspeak API key
String apiKey = "HeyThatsMineGetYOurs";

// connect 10 to TX of Serial USB
// connect 11 to RX of serial USB
SoftwareSerial ser(10, 11); // RX, TX
Adafruit_BMP085 bmp;
// the loop 
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);    

 // enable software serial
  ser.begin(9600);
  
  // reset ESP8266
  ser.println("AT+RST");
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
}


void loop() {
  
  // blink LED on board
  digitalWrite(ledPin, HIGH);   
  delay(200);               
  digitalWrite(ledPin, LOW);

  float temp = bmp.readTemperature();
  float pres= bmp.readPressure()/1000.0;
  // convert to string
  char buf[16];
  char buff2[16];
  String strTemp = dtostrf(temp, 4, 1, buf);
  String strPress = dtostrf(pres, 4, 1, buff2);
  Serial.println(strTemp);
  Serial.println(strPress);
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  ser.println(cmd);
   
  if(ser.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(strTemp);
  getStr +="&field2=";
  getStr += String(strPress);
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ser.println(cmd);
Serial.println(getStr);
  if(ser.find(">")){
    ser.print(getStr);
  }
  else{
    ser.println("AT+CIPCLOSE");
    // alert user
    Serial.println("AT+CIPCLOSE");
  }
    
  //  60 sec delay between updates
  delay(60000);  
}
