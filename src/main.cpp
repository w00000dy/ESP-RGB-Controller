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
String indexHTMLString;
String materializeCSSString;
String styleCSSString;
String materializeJSString;
String syncerJSString;
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

    SPIFFS.begin();                                                     // mount filesystem

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

    server.on("/on", HTTP_GET, [](AsyncWebServerRequest* request) {
        rainbow = app.repeat(10, rainbowLED);
        request->send(200, "text/html", "an");
    });

    server.on("/off", HTTP_GET, [](AsyncWebServerRequest* request) {
        app.disable(rainbow);
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        request->send(200, "text/html", "aus");
    });

    server.onNotFound(notFound);

    server.begin();
}

Reactduino app(start);