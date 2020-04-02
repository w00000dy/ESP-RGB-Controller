#define FASTLED_ESP8266_RAW_PIN_ORDER

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>
#include <Reactduino.h>

#define NUM_LEDS 88
#define DATA_PIN 13
#define CLOCK_PIN 14

int x = 1;
bool lightActive = false;
bool rainbowActive = false;
String response;
reaction rainbow;
reaction ota;

CRGB leds[NUM_LEDS];
AsyncWebServer server(80);

const char* ssid = "";
const char* password = "";

void
notFound(AsyncWebServerRequest* request)
{
    request->send(404, "text/html", "Nicht gefunden!");
}

void
rainbowLED()
{
    if (x >= 256) {
        x = 1;
    } else {
        x++;
    }
    fill_rainbow(leds, NUM_LEDS, x);
    FastLED.show();
}

void
otaHandle()
{
    ArduinoOTA.handle();
}

void
start()
{
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");

    ArduinoOTA.begin();
    ota = app.repeat(5, otaHandle);

    Serial.print("IP Addresse: ");
    Serial.println(WiFi.localIP());

    SPIFFS.begin(); // mount filesystem

    // main web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server.on("/css/materialize.min.css", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/css/materialize.min.css", "text/css");
    });

    server.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/css/style.css", "text/css");
    });

    server.on("/js/materialize.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/js/materialize.min.js", "text/javascript");
    });

    server.on("/js/syncer.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/js/syncer.js", "text/javascript");
    });

    // web controller for rgb strip
    server.on("/effect", HTTP_POST, [](AsyncWebServerRequest* request) {
        if (request->hasParam("light", true)) { // light
            response = request->getParam("light", true)->value();
            FastLED.show();
            if (response == "true") {
                // Disable old effect
                rainbowActive = false;
                app.disable(rainbow);
                // Enable new effect
                lightActive = true;
                fill_solid(leds, NUM_LEDS, CRGB::White);
                FastLED.show();
                request->send(200, "text/plain", response);
            } else if (response = "false") {
                // Disable effect
                lightActive = false;
                fill_solid(leds, NUM_LEDS, CRGB::Black);
                FastLED.show();
                request->send(200, "text/plain", response);
            } else {
                request->send(200, "text/plain", "Error! Invalid parameter. (light)");
            }
        } else if (request->hasParam("rainbow", true)) { // rainbow
            response = request->getParam("rainbow", true)->value();
            if (response == "true") {
                // Disable old effect
                lightActive = false;
                // Enable new effect
                rainbowActive = true;
                rainbow = app.repeat(10, rainbowLED);
                request->send(200, "text/plain", response);
            } else if (response = "false") {
                // Disable effect
                rainbowActive = false;
                app.disable(rainbow);
                fill_solid(leds, NUM_LEDS, CRGB::Black);
                FastLED.show();
                request->send(200, "text/plain", response);
            } else {
                request->send(200, "text/plain", "Error! Invalid parameter. (rainbow)");
            }
        } else {
            request->send(200, "text/plain", "Error! No parameters found.");
        }
    });

    server.on("/sync", HTTP_POST, [](AsyncWebServerRequest* request) {
        if (request->hasParam("effect", true)) { // light
            response = request->getParam("effect", true)->value();
            if (response == "light") {
                if (lightActive == true) {
                    request->send(200, "text/plain", "true");
                } else {
                    request->send(200, "text/plain", "false");
                }
            } else if (response == "rainbow") {
                if (rainbowActive == true) {
                    request->send(200, "text/plain", "true");
                } else {
                    request->send(200, "text/plain", "false");
                }
            } else {
                request->send(200, "text/plain", "Error! Invalid parameter. (effect)");
            }
        } else {
            request->send(200, "text/plain", "Error! No parameters found.");
        }
    });

    server.onNotFound(notFound);

    server.begin();
}

Reactduino app(start);