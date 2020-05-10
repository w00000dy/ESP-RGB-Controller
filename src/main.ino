#define FASTLED_ESP8266_RAW_PIN_ORDER

//  _        _   _                             _              
// | |      (_) | |                           (_)             
// | |       _  | |__    _ __    __ _   _ __   _    ___   ___ 
// | |      | | | '_ \  | '__|  / _` | | '__| | |  / _ \ / __|
// | |____  | | | |_) | | |    | (_| | | |    | | |  __/ \__ \
// |______| |_| |_.__/  |_|     \__,_| |_|    |_|  \___| |___/

#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <FastLED.h>

#define NUM_LEDS 88
#define DATA_PIN 13
#define CLOCK_PIN 14

#define EFFECT_DISABLE -1
#define MUSIC_DISABLE -2
#define EFFECT_LIGHT 0
#define EFFECT_COLOR 1
#define EFFECT_RAINBOW 2
#define EFFECT_FIRE 3
#define EFFECT_RANDOM 4
#define NOTIFICATION_MESSAGE 5
#define MUSIC_WE_ARE_NUMBER_ONE 6

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
    0, 0,
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

uint8 x = 1;
uint i = 0;
int count = 0;
uint8 red;
uint8 green;
uint8 blue;
ulong nextShow = 0;
ulong nextPlay = 0;
int8 lastEffect;
bool lightActive = false;
bool colorActive = false;
bool rainbowActive = false;
bool fireActive = false;
bool randomActive = false;
bool notificationMessage = false;
bool weAreNumberOneActive = false;
String response;
String json;

CRGB leds[NUM_LEDS];
AsyncWebServer server(80);
DNSServer dnsServer;
AsyncWiFiManager wifiManager(&server, &dnsServer);
char HOSTNAME[] = "ESP-RGB-Controller-";

// map-function for double
double mapd(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setActive(int8 effect) {
    lightActive = false;
    colorActive = false;
    rainbowActive = false;
    fireActive = false;
    randomActive = false;
    notificationMessage = false;
    switch (effect) {
        case EFFECT_LIGHT:
            lightActive = true;
            Serial.println("Enable light");
            break;

        case EFFECT_COLOR:
            colorActive = true;
            Serial.println("Enable color");
            break;

        case EFFECT_RAINBOW:
            rainbowActive = true;
            Serial.println("Enable rainbow");
            break;

        case EFFECT_FIRE:
            fireActive = true;
            Serial.println("Enable fire");
            break;

        case EFFECT_RANDOM:
            randomActive = true;
            Serial.println("Enable random");
            break;

        case NOTIFICATION_MESSAGE:
            notificationMessage = true;
            Serial.println("Enable Notification message");
            break;

        default:
            Serial.println("Disable all effects");
            break;
    }
}

int getActive() {
    if (lightActive == true) {
        return EFFECT_LIGHT;
    } else if (colorActive == true) {
        return EFFECT_COLOR;
    } else if (rainbowActive == true) {
        return EFFECT_RAINBOW;
    } else if (fireActive == true) {
        return EFFECT_FIRE;
    } else if (randomActive == true) {
        return EFFECT_RANDOM;
    } else if (notificationMessage == true) {
        return NOTIFICATION_MESSAGE;
    } else {
        return EFFECT_DISABLE;
    }
}

void setMusic(int8 music) {
    weAreNumberOneActive = false;
    switch (music) {
        case MUSIC_WE_ARE_NUMBER_ONE:
            weAreNumberOneActive = true;
            Serial.println("Playing we are numer one");
            break;

        default:
            Serial.println("Disable music");
            break;
    }
}

//  ______    __    __                 _         
// |  ____|  / _|  / _|               | |        
// | |__    | |_  | |_    ___    ___  | |_   ___ 
// |  __|   |  _| |  _|  / _ \  / __| | __| / __|
// | |____  | |   | |   |  __/ | (__  | |_  \__ \
// |______| |_|   |_|    \___|  \___|  \__| |___/

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

// rainbow effect
void rainbowLED() {
    if (x >= 256) {
        x = 1;
    } else {
        x++;
    }
    fill_rainbow(leds, NUM_LEDS, x);
    FastLED.show();
}

// random effect
void randomLED() {
    fill_solid(leds, NUM_LEDS, CHSV(random8(), random8(), random8()));
    FastLED.show();
}

// asynchronous music
void playWeAreNumberOne() {
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

void onMessage() {
    if (count >= 44) {
        leds[0] = CRGB::Black;
        Serial.println(lastEffect);
        setActive(lastEffect);
    } else {
        leds[44 + count] = CRGB::Black;
        leds[44 - count] = CRGB::Black;
        count++;
    }
}

// not found
void notFound(AsyncWebServerRequest* request) {
    request->send(200, "text/html", "Error! Not found.");
}

void setup() {
    Serial.begin(9600);
    // Set hostname from chipId
    strcat(HOSTNAME, String(ESP.getChipId()).c_str());
    wifi_station_set_hostname(const_cast<char*>(HOSTNAME));
    // connect to wifi
    wifiManager.autoConnect("ESP-RGB-setup");
    FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    pinMode(15, OUTPUT);
    Serial.println("");
    // Print IP Adress, Hostname and Chip-ID
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Hostname: ");
    Serial.println(HOSTNAME);
    Serial.print("Chip-ID: ");
    Serial.println(ESP.getChipId());

    // OTA
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.begin();

    SPIFFS.begin();  // mount filesystem

    // __          __         _             _   _          
    // \ \        / /        | |           (_) | |         
    //  \ \  /\  / /    ___  | |__    ___   _  | |_    ___ 
    //   \ \/  \/ /    / _ \ | '_ \  / __| | | | __|  / _ \
    //    \  /\  /    |  __/ | |_) | \__ \ | | | |_  |  __/
    //     \/  \/      \___| |_.__/  |___/ |_|  \__|  \___|

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
                // Enable new effect
                setActive(EFFECT_LIGHT);
                fill_solid(leds, NUM_LEDS, CRGB::White);
                FastLED.show();
                request->send(200, "text/plain", response);
            } else if (response = "false") {
                // Disable effect
                setActive(EFFECT_DISABLE);
                FastLED.clear();
                FastLED.show();
                request->send(200, "text/plain", response);
            } else {
                request->send(200, "text/plain", "Error! Invalid parameter. (light)");
            }
        } else if (request->hasParam("color", true)) {  // color
            response = request->getParam("color", true)->value();
            if (response == "false") {
                // Disable effect
                setActive(EFFECT_DISABLE);
                FastLED.clear();
                FastLED.show();
                request->send(200, "text/plain", response);
            } else {
                if (request->hasParam("red", true) && request->hasParam("green", true) && request->hasParam("blue", true)) {
                    red = request->getParam("red", true)->value().toInt();
                    green = request->getParam("green", true)->value().toInt();
                    blue = request->getParam("blue", true)->value().toInt();
                    // Enable new effect
                    setActive(EFFECT_COLOR);
                    fill_solid(leds, NUM_LEDS, CRGB(red, green, blue));
                    FastLED.show();
                    request->send(200, "text/plain", response);
                }
            }
        } else if (request->hasParam("rainbow", true)) {  // rainbow
            response = request->getParam("rainbow", true)->value();
            if (response == "true") {
                // Enable new effect
                setActive(EFFECT_RAINBOW);
                request->send(200, "text/plain", response);
            } else if (response = "false") {
                // Disable effect
                setActive(EFFECT_DISABLE);
                FastLED.clear();
                FastLED.show();
                request->send(200, "text/plain", response);
            } else {
                request->send(200, "text/plain", "Error! Invalid parameter. (rainbow)");
            }
        } else if (request->hasParam("fire", true)) {  // fire
            response = request->getParam("fire", true)->value();
            if (response == "true") {
                // Enable new effect
                setActive(EFFECT_FIRE);
                request->send(200, "text/plain", response);
            } else if (response = "false") {
                // Disable effect
                setActive(EFFECT_DISABLE);
                FastLED.clear();
                FastLED.show();
                request->send(200, "text/plain", response);
            } else {
                request->send(200, "text/plain", "Error! Invalid parameter. (fire)");
            }
        } else if (request->hasParam("random", true)) {  // fire
            response = request->getParam("random", true)->value();
            if (response == "true") {
                // Enable new effect
                setActive(EFFECT_RANDOM);
                request->send(200, "text/plain", response);
            } else if (response = "false") {
                // Disable effect
                setActive(EFFECT_DISABLE);
                FastLED.clear();
                FastLED.show();
                request->send(200, "text/plain", response);
            } else {
                request->send(200, "text/plain", "Error! Invalid parameter. (fire)");
            }
        } else {
            request->send(200, "text/plain", "Error! No parameters found.");
        }
    });

    //             _____    _____ 
    //     /\     |  __ \  |_   _|
    //    /  \    | |__) |   | |  
    //   / /\ \   |  ___/    | |  
    //  / ____ \  | |       _| |_ 
    // /_/    \_\ |_|      |_____|

    server.on("/api", HTTP_POST, [](AsyncWebServerRequest* request) {
        if (request->hasParam("json", true)) {
            json = request->getParam("json", true)->value();
            Serial.print("JSON: ");
            Serial.println(json);
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, json);
            bool on = doc["on"];
            uint8 brightness = doc["brightness"];
            double hue = doc["color"]["spectrumHsv"]["hue"];
            double saturation = doc["color"]["spectrumHsv"]["saturation"];
            double value = doc["color"]["spectrumHsv"]["value"];
            FastLED.setBrightness(map(brightness, 0, 100, 0, 255));
            if (on == true) {
                // Enable new effect
                setActive(EFFECT_COLOR);
                fill_solid(leds, NUM_LEDS, CHSV(mapd(hue, 0, 359, 0, 255), mapd(saturation, 0, 1, 0, 255), mapd(value, 0, 1, 0, 255)));
                FastLED.show();
            } else {
                // Disable effect
                setActive(EFFECT_DISABLE);
                FastLED.clear();
                FastLED.show();
                request->send(200, "text/plain", response);
            }
            request->send(200, "text/plain", "ok ... success");
        } else if (request->hasParam("notification", true)) {
            response = request->getParam("notification", true)->value();
            if (response == "message" && notificationMessage == false) {
                Serial.println(getActive());
                lastEffect = getActive();
                Serial.println(lastEffect);
                fill_solid(leds, NUM_LEDS, CRGB::Aqua);
                FastLED.show();
                count = 0;
                nextShow = millis() + 100;
                setActive(NOTIFICATION_MESSAGE);
                request->send(200, "text/plain", "ok ... success");
            }
            else {
                request->send(200, "text/plain", "Error unknown notification type");
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
                setMusic(MUSIC_WE_ARE_NUMBER_ONE);
                request->send(200, "text/plain", response);
            } else if (response = "false") {
                // Disable music
                setMusic(MUSIC_DISABLE);
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
    server.on("/sync", HTTP_GET, [](AsyncWebServerRequest* requestSync) {
        json = "[";
        if (lightActive == true) {
            json += "\"true\"";
        } else {
            json += "\"false\"";
        }
        if (colorActive == true) {
            json += ",\"true\"";
        } else {
            json += ",\"false\"";
        }
        if (rainbowActive == true) {
            json += ",\"true\"";
        } else {
            json += ",\"false\"";
        }
        if (fireActive == true) {
            json += ",\"true\"";
        } else {
            json += ",\"false\"";
        }
        if (randomActive == true) {
            json += ",\"true\"";
        } else {
            json += ",\"false\"";
        }
        if (weAreNumberOneActive == true) {
            json += ",\"true\"";
        } else {
            json += ",\"false\"";
        }
        json += "]";
        requestSync->send(200, "application/json", json);
    });

    server.onNotFound(notFound);

    server.begin();
}

void loop() {
    ArduinoOTA.handle();
    handleAsync();
}

void handleAsync() {
    // Effects
    if (millis() >= nextShow) {
        if (rainbowActive == true) {
            rainbowLED();
            nextShow = millis() + 10;
        }
        if (fireActive == true) {
            fireLED();
            nextShow = millis() + 30;
        }
        if (randomActive == true) {
            randomLED();
            nextShow = millis() + random8(75, 150);
        }
        if (notificationMessage == true) {
            onMessage();
            FastLED.show();
            nextShow = millis() + 25;
        }
    }
    // Music
    if (millis() >= nextPlay) {
        if (weAreNumberOneActive == true) {
            playWeAreNumberOne();
        }
    }
}