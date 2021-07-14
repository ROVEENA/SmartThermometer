// ---------------------------------------------------------------- //
// Arduino Ultrasoninc Sensor HC-SR04
// Re-writed by Arbi Abdul Jabbaar
// Using Arduino IDE 1.8.7
// Using HC-SR04 Module
// Tested on 17 September 2019
// ---------------------------------------------------------------- //


#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <M5StickC.h>
#include <Wire.h>

const char *ssid = "Luminous";
const char *password = "9844184603";

const int echoPin = 33;
const int trigPin = 32;

// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

// Your Domain name with URL path or IP address with path
String iftttApiKey = "TVtgGIslSXdpBfkuTXYOn";
String event = "RED_Zone";



uint16_t result;
float temperature;
volatile bool buttonPressed = false;
volatile int presses = 0;

void IRAM_ATTR buttonEvent() {
  buttonPressed = true;
  presses++;
}


void setup() {
   M5.begin();
   M5.Lcd.print("Distance");
   M5.Lcd.fillScreen(GREEN);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(115200); // // Serial Communication is starting with 9600 of baudrate speed
 

   Wire.begin(0,26);
  Serial.begin(115200);
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(0, 40);
  
  WiFi.begin(ssid, password);

}
void loop() {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
//  Serial.print("duration: ");
//  Serial.println(duration);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  if(distance>2 && distance<15)
  {
   Wire.beginTransmission(0x5A);          // Send Initial Signal and I2C Bus Address
  Wire.write(0x07);                      // Send data only once and add one address automatically.
  Wire.endTransmission(false);           // Stop signal
  Wire.requestFrom(0x5A, 2);             // Get 2 consecutive data from 0x5A, and the data is stored only.
  result = Wire.read();                  // Receive DATA
  result |= Wire.read() << 8;            // Receive DATA
  
  temperature = result * 0.02 - 273.15;
  
 // M5.Lcd.fillRect(0,40,120,100,BLACK);
  M5.Lcd.setCursor(0, 40);
  
  

  if(temperature >=30)
  {
    buttonPressed = false;
    sendButtonTrigger(temperature);
    delay(500);
      M5.Lcd.fillScreen(RED);
  M5.Lcd.setTextColor(BLACK);
    
    }else
    {
      M5.Lcd.fillScreen(GREEN);
  M5.Lcd.setTextColor(BLACK);
      }
M5.Lcd.print(temperature);
  Serial.println(temperature);
    
//  if (buttonPressed) {
//    buttonPressed = false;
//    sendButtonTrigger();
//    delay(500);
//  }
  delay(500);
  M5.update();   
     }
    
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
//   M5.Lcd.print("Distance");
//    M5.Lcd.print(distance);
//  delay(500);
  Serial.println(" cm");
}


void sendButtonTrigger(float temp) {
   if (WiFi.status() == WL_CONNECTED) {
    String serverPath = "https://maker.ifttt.com/trigger/" + event + "/with/key/" + iftttApiKey + "?value1=" + temp;

    String jsonBuffer = httpGETRequest(serverPath.c_str());
    Serial.println(jsonBuffer);
    JSONVar myObject = JSON.parse(jsonBuffer);

    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }
    Serial.print("JSON object = ");
    Serial.println(myObject);
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  }

  String httpGETRequest(const char* serverName) {
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
