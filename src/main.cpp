#define FASTLED_ESP8266_RAW_PIN_ORDER

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>
#include <Reactduino.h>

#define NUM_LEDS 88
#define DATA_PIN 13
#define CLOCK_PIN 14

const int fn = 400;
const int hn = 200;
const int qn = 100;
const int f5 = 698;
const int c6 = 1047;
const int b5 = 988;
const int gh5 = 831;
const int ch6 = 1109;
const int dh6 = 1245;

int WeAreNumberOne[] = {
    f5, fn + hn,
    c6, hn,
    b5, qn,
    c6, qn,
    b5, qn,
    c6, qn,
    b5, hn,
    c6, hn,
    gh5, fn,
    f5, fn + hn,
    f5, hn,
    gh5, hn,
    c6, hn,
    ch6, fn,
    gh5, fn,
    ch6, fn,
    dh6, fn,
    c6, hn,
    ch6, hn,
    c6, hn,
    ch6, hn,
    c6, fn};

int x = 1;
int i = 0;
uint64 nextPlay = 0;
bool lightActive = false;
bool rainbowActive = false;
bool fireActive = false;
bool weAreNumberOneActive = false;
String response;
reaction rainbow;
reaction fire;
reaction music;

CRGB leds[NUM_LEDS];
AsyncWebServer server(80);

const char* ssid = "";
const char* password = "";

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING 65

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

// reverse the direction of the fire
bool gReverseDirection = true;

void fireLED() {
    // Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS];

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_LEDS; i++) {
        heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = NUM_LEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING) {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < NUM_LEDS; j++) {
        CRGB color = HeatColor(heat[j]);
        int pixelnumber;
        if (gReverseDirection) {
            pixelnumber = (NUM_LEDS - 1) - j;
        } else {
            pixelnumber = j;
        }
        leds[pixelnumber] = color;
    }
    FastLED.show();
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

// asynchronous music
void playWeAreNumberOne() {
    if (millis() >= nextPlay) {
        if (i < (sizeof(WeAreNumberOne) / sizeof(int))) {
            tone(15, WeAreNumberOne[i]);
            nextPlay = millis() + WeAreNumberOne[i + 1];
            i = i + 2;
        } else {
            noTone(15);
            nextPlay = millis() + 1000;
            i = 0;
        }
    }
}

void notFound(AsyncWebServerRequest* request) {
    request->send(404, "text/html", "Nicht gefunden!");
}

void start() {
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    pinMode(15, OUTPUT);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");

    Serial.print("IP Addresse: ");
    Serial.println(WiFi.localIP());

    SPIFFS.begin();  // mount filesystem

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
        if (request->hasParam("light", true)) {  // light
            response = request->getParam("light", true)->value();
            if (response == "true") {
                // Disable old effect
                rainbowActive = false;
                app.disable(rainbow);
                fireActive = false;
                app.disable(fire);
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
        } else if (request->hasParam("rainbow", true)) {  // rainbow
            response = request->getParam("rainbow", true)->value();
            if (response == "true") {
                // Disable old effect
                lightActive = false;
                fireActive = false;
                app.disable(fire);
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
        } else if (request->hasParam("fire", true)) {  // fire
            response = request->getParam("fire", true)->value();
            if (response == "true") {
                // Disable old effect
                lightActive = false;
                rainbowActive = false;
                app.disable(rainbow);
                // Enable new effect
                fireActive = true;
                fire = app.repeat(30, fireLED);
                request->send(200, "text/plain", response);
            } else if (response = "false") {
                // Disable effect
                fireActive = false;
                app.disable(fire);
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

    // music functionality
    server.on("/music", HTTP_POST, [](AsyncWebServerRequest* request) {
        if (request->hasParam("weAreNumberOne", true)) {  // We are number one
            response = request->getParam("weAreNumberOne", true)->value();
            if (response == "true") {
                // Start playing music
                weAreNumberOneActive = true;
                music = app.repeat(10, playWeAreNumberOne);
                request->send(200, "text/plain", response);
            } else if (response = "false") {
                // Disable music
                weAreNumberOneActive = false;
                app.disable(music);
                noTone(15);
                request->send(200, "text/plain", response);
            } else {
                request->send(200, "text/plain", "Error! Invalid parameter. (weAreNumberOne)");
            }
        } else {
            request->send(200, "text/plain", "Error! No parameters found.");
        }
    });

    // this is for synchronizing the buttons on multiple devices
    server.on("/sync", HTTP_POST, [](AsyncWebServerRequest* request) {
        if (request->hasParam("effect", true)) {
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
            } else if (response == "fire") {
                if (fireActive == true) {
                    request->send(200, "text/plain", "true");
                } else {
                    request->send(200, "text/plain", "false");
                }
            }
        } else if (request->hasParam("music", true)) {
            response = request->getParam("music", true)->value();
            if (response == "weAreNumberOne") {
                if (weAreNumberOneActive == true) {
                    request->send(200, "text/plain", "true");
                } else {
                    request->send(200, "text/plain", "false");
                }
            }
        } else {
            request->send(200, "text/plain", "Error! Invalid parameter. (sync)");
        }
    });

    server.onNotFound(notFound);

    server.begin();
}

Reactduino app(start);