// ESP32 Project main file
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ---------- Wi-Fi ----------
const char* ssid = "DK-Crib 2.4GHz"; 
const char* password = "dannykylie6103";

// RGB LED pins
const int RED_PIN   = 18;
const int GREEN_PIN = 5;
const int BLUE_PIN  = 17;

// PWM channels for each color
const int RED_CH   = 0;
const int GREEN_CH = 1;
const int BLUE_CH  = 2;

const int PWM_FREQ = 5000;
const int PWM_RES  = 8; // 8-bit resolution → 0-255

int currentDuty = 0;         // brightness value 0-255

WebServer server(80);

void setColor(String color, int duty) {
  currentDuty = duty; // Update global brightness

  if (color == "Red") {
    ledcWrite(RED_CH, currentDuty);
    ledcWrite(GREEN_CH, 0);
    ledcWrite(BLUE_CH, 0);
  } else if (color == "Green") {
    ledcWrite(RED_CH, 0);
    ledcWrite(GREEN_CH, currentDuty);
    ledcWrite(BLUE_CH, 0);
  } else if (color == "Blue") {
    ledcWrite(RED_CH, 0);
    ledcWrite(GREEN_CH, 0);
    ledcWrite(BLUE_CH, currentDuty);
  } else if (color == "White") {
    ledcWrite(RED_CH, currentDuty);
    ledcWrite(GREEN_CH, currentDuty);
    ledcWrite(BLUE_CH, currentDuty);
  } else if (color == "Yellow") {
    ledcWrite(RED_CH, currentDuty);
    ledcWrite(GREEN_CH, currentDuty / 2); // Dim green for yellowish
    ledcWrite(BLUE_CH, 0);
  } else if (color == "Cyan") {
    ledcWrite(RED_CH, 0);
    ledcWrite(GREEN_CH, currentDuty);
    ledcWrite(BLUE_CH, currentDuty);
  } else if (color == "Purple") {
    ledcWrite(RED_CH, currentDuty);
    ledcWrite(GREEN_CH, 0);
    ledcWrite(BLUE_CH, currentDuty);
  } else { // Off or unknown color
    ledcWrite(RED_CH, 0);
    ledcWrite(GREEN_CH, 0);
    ledcWrite(BLUE_CH, 0);
  } 
}


// ---------- HTTP request handler ----------
void handleSet() {
  String color = server.arg("color");             // e.g. "Red"
  int brightness = server.arg("brightness").toInt(); // 0-100%

  // Map 0-100% brightness to 0-255 PWM duty
  int duty = map(brightness, 0, 100, 0, 255);

  // Update LED
  setColor(color, duty); // <-- changed: now updates in real time

  Serial.printf("Color: %s, Brightness: %d\n", color.c_str(), brightness);
  server.send(200, "text/plain", "OK");
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);  

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP()); // <-- note this IP for Python GUI

  // Setup PWM for LED
  ledcSetup(RED_CH, PWM_FREQ, PWM_RES);
  ledcSetup(GREEN_CH, PWM_FREQ, PWM_RES);
  ledcSetup(BLUE_CH, PWM_FREQ, PWM_RES);

  ledcAttachPin(RED_PIN, RED_CH);
  ledcAttachPin(GREEN_PIN, GREEN_CH);
  ledcAttachPin(BLUE_PIN, BLUE_CH);


  // Setup web server route
  server.on("/set", handleSet);
  server.begin();
}

// ---------- Main loop ----------
void loop() {
  server.handleClient(); // Handle incoming HTTP requests
}

