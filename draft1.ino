#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <M5StickC.h>
#include <Wire.h>

const char *ssid = "Luminous";
const char *password = "9844184603";
const int BIG_BUTTON = 37;
volatile bool buttonPressed = false;
volatile int presses = 0;

uint16_t result;
float temperature;

// Your Domain name with URL path or IP address with path
String iftttApiKey = "TVtgGIslSXdpBfkuTXYOn";
String event = "RED_Zone";

void IRAM_ATTR buttonEvent() {
  buttonPressed = true;
  presses++;
}

void setup() {
  M5.begin();
  Wire.begin(0,26);
  Serial.begin(115200);
  pinMode(BIG_BUTTON, INPUT);
  attachInterrupt(BIG_BUTTON, buttonEvent, FALLING);
  Serial.begin(115200);
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(0, 40);
  
  WiFi.begin(ssid, password);
}
void loop() {
  Wire.beginTransmission(0x5A);          // Send Initial Signal and I2C Bus Address
  Wire.write(0x07);                      // Send data only once and add one address automatically.
  Wire.endTransmission(false);           // Stop signal
  Wire.requestFrom(0x5A, 2);             // Get 2 consecutive data from 0x5A, and the data is stored only.
  result = Wire.read();                  // Receive DATA
  result |= Wire.read() << 8;            // Receive DATA
  
  temperature = result * 0.02 - 273.15;
  
  M5.Lcd.fillRect(0,40,120,100,BLACK);
  M5.Lcd.setCursor(0, 40);
  
  M5.Lcd.print(temperature);
  Serial.println(temperature);

  if(temperature >=37)
  {
    buttonPressed = false;
    sendButtonTrigger(temperature);
    delay(500);
    
    }
//  if (buttonPressed) {
//    buttonPressed = false;
//    sendButtonTrigger();
//    delay(500);
//  }
  delay(5000);
  M5.update();
}

void sendButtonTrigger(float temp) {
  // Send an HTTP GET request
  // Check WiFi connection status
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
