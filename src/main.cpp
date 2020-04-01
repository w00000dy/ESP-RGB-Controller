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
String data;
reaction rainbow;
reaction ota;

CRGB leds[NUM_LEDS];
AsyncWebServer server(80);

const char *ssid = "";
const char *password = "";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/html", "Nicht gefunden!");
}

void rainbowLED() {
    if (x >= 256) {
        x = 1;
    } else {
        x++;
    }
    fill_rainbow(leds, NUM_LEDS, x);
    FastLED.show();
}

void otaHandle() { ArduinoOTA.handle(); }

void start() {
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

    SPIFFS.begin();                           // Filesystem mounten
    File f = SPIFFS.open("/test.html", "r");  // Datei zum lesen öffnen
    if (!f) {
        Serial.println("file open failed");
    }
    data = f.readString();  // Inhalt der Textdatei wird gelesen...
    Serial.println("Inhalt der geöffneten Datei:");
    Serial.println(data);  // ... und wieder ausgegeben
    f.close();             // Wir schließen die Datei

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", data);
    });

    server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
        rainbow = app.repeat(10, rainbowLED);
        request->send(200, "text/html", "an");
    });

    server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
        app.disable(rainbow);
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        request->send(200, "text/html", "aus");
    });

    server.onNotFound(notFound);

    server.begin();
}

Reactduino app(start);