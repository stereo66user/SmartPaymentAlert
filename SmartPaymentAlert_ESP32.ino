/*
  SmartPaymentAlert - ESP32 receiver starter
  Board: ESP32 Dev Module
  Libraries: built-in WiFi.h and WebServer.h only

  IMPORTANT:
  This is a communication/test receiver, NOT proof of a real bank payment.
  It accepts a message only when the shared test token matches.
  Do not use this endpoint as payment verification.
*/

#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "GRANDMA_HOTSPOT_NAME";
const char* WIFI_PASSWORD = "GRANDMA_HOTSPOT_PASSWORD";

// Change this to a long random value. Use the same token in the sender app.
const char* DEVICE_TOKEN = "CHANGE_THIS_TO_A_LONG_RANDOM_TOKEN";

WebServer server(80);
const int STATUS_LED = 2; // Many ESP32 DevKit boards use GPIO2 for onboard LED.

void blinkStatus() {
  pinMode(STATUS_LED, OUTPUT);
  for (int i = 0; i < 3; i++) {
    digitalWrite(STATUS_LED, HIGH);
    delay(150);
    digitalWrite(STATUS_LED, LOW);
    delay(150);
  }
}

void handleHealth() {
  server.send(200, "text/plain", "SmartPaymentAlert ESP32 is online");
}

void handleNotify() {
  if (!server.hasArg("token") || server.arg("token") != DEVICE_TOKEN) {
    server.send(403, "text/plain", "Forbidden");
    return;
  }

  if (!server.hasArg("amount")) {
    server.send(400, "text/plain", "Missing amount");
    return;
  }

  String amount = server.arg("amount");
  amount.trim();

  // Basic input validation: accept digits and one optional decimal point only.
  if (amount.length() == 0 || amount.length() > 10) {
    server.send(400, "text/plain", "Invalid amount");
    return;
  }

  bool valid = true;
  int dots = 0;
  for (size_t i = 0; i < amount.length(); i++) {
    char c = amount[i];
    if (c == '.') dots++;
    else if (c < '0' || c > '9') valid = false;
  }
  if (!valid || dots > 1) {
    server.send(400, "text/plain", "Invalid amount format");
    return;
  }

  Serial.println();
  Serial.println("----- TEST ALERT RECEIVED -----");
  Serial.print("Amount text: ");
  Serial.println(amount);
  Serial.println("NOTE: This is only a received message, not bank-verified payment.");
  Serial.println("--------------------------------");

  blinkStatus();
  server.send(200, "text/plain", "Alert received by ESP32");
}

void setup() {
  Serial.begin(115200);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  Serial.println("\nSmartPaymentAlert ESP32 starting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to hotspot");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 30000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWi-Fi connection failed. Check SSID/password and restart.");
    return;
  }

  Serial.println("\nWi-Fi connected.");
  Serial.print("ESP32 local IP: ");
  Serial.println(WiFi.localIP());

  server.on("/health", HTTP_GET, handleHealth);
  server.on("/notify", HTTP_GET, handleNotify);
  server.begin();

  Serial.println("HTTP receiver ready.");
  Serial.println("Test: http://ESP32_IP/health");
  Serial.println("Alert endpoint: /notify?amount=152&token=YOUR_TOKEN");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
  } else {
    Serial.println("Wi-Fi disconnected; reconnecting...");
    WiFi.reconnect();
    delay(1000);
  }
}
