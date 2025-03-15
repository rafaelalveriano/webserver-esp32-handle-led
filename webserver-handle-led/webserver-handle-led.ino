#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "../config.h"
#define LED_PIN 15

AsyncWebServer server(80);

void handleLed(const char *status) {
  pinMode(LED_PIN, OUTPUT);
  if (strcmp(status, "on") == 0) {
    digitalWrite(LED_PIN, HIGH);
    Serial.print("LED [ON] 💡 \n");
    return;
  }
  digitalWrite(LED_PIN, LOW);
  Serial.print("LED [OFF] 💤  \n");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
  Serial.print("Access url: http://");
  Serial.println(WiFi.localIP().toString());
  delay(1000);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "On Fire 🔥");
  });
  server.on(
    "/led", HTTP_POST, [](AsyncWebServerRequest *request) {
      Serial.println("Start handle led");
    },
    NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      StaticJsonDocument<200> jsonDoc;
      DeserializationError error = deserializeJson(jsonDoc, data, len);
      if (error) {
        Serial.println("Erro parse JSON!");
        request->send(400, "application/json", "{\"erro\":\"invalid Body JSON\"}");
        return;
      }
      const char *status = jsonDoc["status"];
      Serial.printf("Status: %s\n", status);
      if (strcmp(status, "on") != 0 && strcmp(status, "off") != 0) {
        request->send(200, "application/json", "not found status");
        return;
      }

      handleLed(status);
      request->send(200, "application/json", "{\"status\":\"ok\"}");
    });
  server.begin();
}

void loop() {
}