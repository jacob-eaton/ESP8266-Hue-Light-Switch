#include <ESP8266WiFi.h> 
#include <SPI.h>

int buttonState = 0;
int oldButtonState = 0;
int cnt = 0;
int group = 1;
int oldOnOffState = 2;
boolean onOffState = true;
String command = "";
String state = "";

const char hueHubIP[] = "";
const char hueUsername[] = "";
const char* ssid = "";
const char* pass = "";
const int hueHubPort = 80;
const int buttonPin = 2;

WiFiClient client;

void getHue() {
  if (client.connect(hueHubIP, hueHubPort)) {
    client.print("GET /api/");
    client.print(hueUsername);
    client.print("/groups/");
    client.print(group);  
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(hueHubIP);
    client.println("Content-type: application/json");
    client.println("keep-alive");
    client.println();
    while (client.connected()) {
      if (client.available()) {
        client.findUntil("\"any_on\":", "\0");
        state = client.readStringUntil('}');
        if (state == "true") {
          onOffState = true;
          Serial.println("true");
        } else {
          onOffState = false;
          Serial.println("false");
        }
        Serial.print("getHue  ");
        Serial.print(state);
        Serial.print(" - ");
        Serial.print(onOffState);
        Serial.println();
        break;
      }
    }
    client.stop();
  }
  
  if (onOffState == true) { 
    command = "{\"on\": false}";
  } else { 
    command = "{\"on\": true}";
  }
}

void setHue(String command) {
  if (client.connect(hueHubIP, hueHubPort)) {
    client.print("PUT /api/");
    client.print(hueUsername);
    client.print("/groups/"); 
    client.print(group); 
    client.println("/action HTTP/1.1");
    client.println("keep-alive");
    client.print("Host: ");
    client.println(hueHubIP);
    client.print("Content-Length: ");
    client.println(command.length());
    client.println("Content-Type: text/plain;charset=UTF-8");
    client.println();
    client.println(command);
    client.stop(); 
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();  
  Serial.println("  ____  _   _  ____ ");
  Serial.println(" (_  _)( )_( )( ___)");
  Serial.println(" -_)(   ) _ (  )__) ");
  Serial.println("(____) (_) (_)(____)");
  Serial.println();
  WiFi.begin(ssid, pass);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");
  pinMode(buttonPin,INPUT);
  getHue();
  oldOnOffState = onOffState;
  cnt = 0;
  while (oldOnOffState == onOffState && cnt <= 99) {
    setHue(command);
    getHue();
    ++cnt;
  }
  Serial.print("Command sent after ");
  Serial.print(cnt);
  Serial.print(" tries.\n");
}

void loop() {
  buttonState = digitalRead(buttonPin);
  if (oldButtonState != buttonState && buttonState == LOW) {
    getHue();
    oldOnOffState = onOffState;
    cnt = 0;
    while (oldOnOffState == onOffState && cnt <= 99) {
      setHue(command);
      getHue();
      ++cnt;
    }
    Serial.print("Command sent after ");
    Serial.print(cnt);
    Serial.print(" tries.\n");
  }
  oldButtonState = buttonState;
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nConnected to WiFi.");
  }
}
