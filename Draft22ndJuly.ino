// ---------------------------------------------------------------- //
// IOT Group 3 - Smart Thermometer
// Team members - Sachin Dukale, Rashmi Dusane, Roveena Lobo, Mohit Yeware
// Description - Automated Temperature Detection system
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

// Api key for 'ThingSpeak'
String thingSpeakApiKey = "46TVPD2Z6BOIYXSR";

uint16_t result;
float temperature;
volatile bool buttonPressed = false;
volatile int presses = 0;

void IRAM_ATTR buttonEvent() 
{
  buttonPressed = true;
  presses++;
}

// Setup method
void setup() 
{
  M5.begin();
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(115200); // // Serial Communication is starting with 9600 of baudrate speed
  Wire.begin(0,26);
  //M5.Lcd.fillScreen(BLACK);
 // M5.Lcd.setTextColor(WHITE);     
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0,0);
  //M5.Lcd.println("Please place your hand below");

  
  WiFi.begin(ssid, password);
}

void loop() 
{
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  
  if(distance > 2 && distance < 15)
  {
    Wire.beginTransmission(0x5A);          // Send Initial Signal and I2C Bus Address
    Wire.write(0x07);                      // Send data only once and add one address automatically.
    Wire.endTransmission(false);           // Stop signal
    Wire.requestFrom(0x5A, 2);             // Get 2 consecutive data from 0x5A, and the data is stored only.
    result = Wire.read();                  // Receive DATA
    result |= Wire.read() << 8;            // Receive DATA
    
    temperature = result * 0.02 - 273.15; // Convert temperature in `C
   
    if(temperature >= 28)
    {
      buttonPressed = false;
      eventTrigger(temperature);
      //delay(1000);
      M5.Lcd.fillScreen(RED);
      M5.Lcd.setTextColor(BLACK);
      M5.Lcd.setCursor(0, 5);
      //M5.Lcd.print("You aren't allowed to enter");
      M5.Lcd.print("Please don't enter");
    } 
    else
    {
      
      M5.Lcd.fillScreen(GREEN);
      M5.Lcd.setTextColor(BLACK);
      M5.Lcd.setCursor(0, 5);
      M5.Lcd.print("You're allowed to enter");
    }
      //M5.Lcd.fillScreen(BLACK);
      
      M5.Lcd.setCursor(0, 50);
      M5.Lcd.print("Temp:");
      //M5.Lcd.print(temperature);
    //  M5.Lcd.print("`C");


      M5.Lcd.printf("%0.1f%cC", temperature, (char)000);
      
      Serial.println(temperature);
      
      delay(3000);
      M5.Lcd.fillScreen(BLACK);
      M5.update();   
  } //End if(distance)
  else
  { 
     
       M5.Lcd.setTextColor(WHITE);
       M5.Lcd.setCursor(0, 0);
       M5.Lcd.setTextSize(2);
       M5.Lcd.println("Please place your hand for 2 seconds");
  }  
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}


// Function for IFTTT and ThingSpeak
void eventTrigger(float temp) {
   if (WiFi.status() == WL_CONNECTED) 
   {
      String serverPath = "https://maker.ifttt.com/trigger/" + event + "/with/key/" + iftttApiKey + "?value1=" + temp;
      String path = "https://api.thingspeak.com/update?api_key=" + thingSpeakApiKey + "&field2=" + temp;
  
      Serial.println(path);
  
      String analyticsOutput = httpGETRequest(path.c_str());
  
      String jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") 
      {
        Serial.println("Parsing input failed!");
        return;
      }
      Serial.print("JSON object = ");
      Serial.println(myObject);
   } 
   else 
   {
      Serial.println("WiFi Disconnected");
   }
}

// Http get request method
String httpGETRequest(const char* serverName) 
{
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) 
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else 
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return payload;
}
