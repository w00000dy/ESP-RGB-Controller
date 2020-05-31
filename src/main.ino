#define FASTLED_ESP8266_RAW_PIN_ORDER

//  _        _   _                             _
// | |      (_) | |                           (_)
// | |       _  | |__    _ __    __ _   _ __   _    ___   ___
// | |      | | | '_ \  | '__|  / _` | | '__| | |  / _ \ / __|
// | |____  | | | |_) | | |    | (_| | | |    | | |  __/ \__ \
// |______| |_| |_.__/  |_|     \__,_| |_|    |_|  \___| |___/

#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <FastLED.h>

#define MAX_LEDS 1024
#define DATA_PIN 13
#define CLOCK_PIN 14

#define EFFECT_DISABLE -1
#define EFFECT_LIGHT 0
#define EFFECT_COLOR 1
#define EFFECT_RAINBOW 2
#define EFFECT_FIRE 3
#define EFFECT_RANDOM 4
#define NOTIFICATION_MESSAGE 5

uint8 x = 1;
uint i = 0;
int count = 0;
int NUM_LEDS;
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
String response;
CRGB leds[MAX_LEDS];
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
            FastLED.clear();
            FastLED.show();
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
    static byte heat[MAX_LEDS];

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

// notification message effect
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

//   _____          _
//  / ____|        | |
// | (___     ___  | |_   _   _   _ __
//  \___ \   / _ \ | __| | | | | | '_ \ 
//  ____) | |  __/ | |_  | |_| | | |_) |
// |_____/   \___|  \__|  \__,_| | .__/
//                               | |
//                               |_|

void setup() {
    Serial.begin(9600);
    delay(2000);
    SPIFFS.begin();  // mount filesystem
    if (SPIFFS.exists("/settings/numLeds.txt") == true) {
        File file = SPIFFS.open("/settings/numLeds.txt", "r");
        String data = file.readString();
        NUM_LEDS = data.toInt();
        Serial.println("Settings found.");
    } else {
        Serial.println("No numLeds.txt file found. Generating file...");
        File file = SPIFFS.open("/settings/numLeds.txt", "w");
        file.print(25);
        NUM_LEDS = 25;
    }
    uint8 theme;
    if (SPIFFS.exists("/settings/theme.txt") == true) {
        File file = SPIFFS.open("/settings/theme.txt", "r");
        String data = file.readString();
        theme = data.toInt();
        Serial.println("Settings found.");
    } else {
        Serial.println("No theme.txt file found. Generating file...");
        File file = SPIFFS.open("/settings/theme.txt", "w");
        theme = 0;
    }

    Serial.print("NUM_LEDS: ");
    Serial.println(NUM_LEDS);
    Serial.print("Theme: ");
    Serial.println(theme);
    // Set hostname from chipId
    strcat(HOSTNAME, String(ESP.getChipId()).c_str());
    wifi_station_set_hostname(const_cast<char*>(HOSTNAME));
    // connect to wifi
    wifiManager.autoConnect("ESP-RGB-setup");
    FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // clear LEDs
    FastLED.clear();
    FastLED.show();
    // Print IP Adress, Hostname and Chip-ID
    Serial.println("");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Hostname: ");
    Serial.println(HOSTNAME);
    Serial.print("Chip-ID: ");
    Serial.println(ESP.getChipId());

    // OTA
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.begin();

    // __          __         _             _   _
    // \ \        / /        | |           (_) | |
    //  \ \  /\  / /    ___  | |__    ___   _  | |_    ___
    //   \ \/  \/ /    / _ \ | '_ \  / __| | | | __|  / _ \
    //    \  /\  /    |  __/ | |_) | \__ \ | | | |_  |  __/
    //     \/  \/      \___| |_.__/  |___/ |_|  \__|  \___|

    // Material Design Theme
    if (theme == 0) {
        server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/index.html", "text/html");
        });

        server.on("/css/materialize.min.css", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/css/materialize.min.css", "text/css");
        });

        server.on("/js/syncer.js", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/js/syncer.js", "text/javascript");
        });

        server.on("/js/materialize.min.js", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/js/materialize.min.js", "text/javascript");
        });

        // Port Theme
    } else if (theme == 1) {
        server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/port.html", "text/html");
        });

        server.on("/Farben", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/farbe.html", "text/html");
        });

        server.on("/Modi", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/modi.html", "text/html");
        });

        server.on("/js/abfragen.js", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/js/abfragen.js", "text/javascript");
        });

        server.on("/css/port.css", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/css/port.css", "text/css");
        });
    }

    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/settings.html", "text/html");
    });

    server.on("/js/darkmode.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/js/darkmode.js", "text/javascript");
    });

    server.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/css/style.css", "text/css");
    });

    //  ______    __    __                 _
    // |  ____|  / _|  / _|               | |
    // | |__    | |_  | |_    ___    ___  | |_
    // |  __|   |  _| |  _|  / _ \  / __| | __|
    // | |____  | |   | |   |  __/ | (__  | |_
    // |______| |_|   |_|    \___|  \___|  \__|

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
                request->send(200, "text/plain", response);
            } else {
                request->send(200, "text/plain", "Error! Invalid parameter. (light)");
            }
        } else if (request->hasParam("color", true)) {  // color
            response = request->getParam("color", true)->value();
            if (response == "false") {
                // Disable effect
                setActive(EFFECT_DISABLE);
                request->send(200, "text/plain", response);
            } else {
                if (request->hasParam("red", true) && request->hasParam("green", true) && request->hasParam("blue", true)) {
                    red = request->getParam("red", true)->value().toInt();
                    green = request->getParam("green", true)->value().toInt();
                    blue = request->getParam("blue", true)->value().toInt();
                    Serial.print("Rot: ");
                    Serial.println(red);
                    Serial.print("Grün: ");
                    Serial.println(green);
                    Serial.print("Blau: ");
                    Serial.println(blue);
                    // Enable new effect
                    setActive(EFFECT_COLOR);
                    fill_solid(leds, NUM_LEDS, CRGB(red, green, blue));
                    FastLED.show();
                    request->send(200, "text/plain", response);
                } else {
                    request->send(200, "text/plain", "Error! Invalid parameter. (color)");
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
            const String json = request->getParam("json", true)->value();
            Serial.print("JSON: ");
            Serial.println(json);
            StaticJsonDocument<256> doc;
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
            } else {
                request->send(200, "text/plain", "Error unknown notification type or notification message effect is already running");
            }
        } else if (request->hasParam("settings", true)) {
            response = request->getParam("settings", true)->value();
            if (response == "numLeds") {
                File file = SPIFFS.open("/settings/numLeds.txt", "r");
                String data = file.readString();
                request->send(200, "text/plain", data);
            } else {
                request->send(200, "text/plain", "Error unknown settings type");
            }
        } else if (request->hasParam("setNumLeds", true) && request->hasParam("setTheme", true)) {
            response = request->getParam("setNumLeds", true)->value();
            Serial.print("Set NumLeds: ");
            Serial.println(response);
            File file = SPIFFS.open("/settings/numLeds.txt", "w");
            file.print(response);

            response = request->getParam("setTheme", true)->value();
            Serial.print("Set Theme: ");
            Serial.println(response);
            file = SPIFFS.open("/settings/theme.txt", "w");
            file.print(response);
            request->send(200, "text/plain", "Settings saved!");
        } else if (request->hasParam("restart", true)) {
            ESP.restart();
        } else {
            request->send(200, "text/plain", "Error! No parameters found.");
        }
    });

    //    _____
    //   / ____|
    //  | (___    _   _   _ __     ___
    //   \___ \  | | | | | '_ \   / __|
    //   ____) | | |_| | | | | | | (__
    //  |_____/   \__, | |_| |_|  \___|
    //             __/ |
    //            |___/

    // this is for synchronizing the buttons on multiple devices
    server.on("/sync", HTTP_GET, [](AsyncWebServerRequest* requestSync) {
        StaticJsonDocument<256> doc;

        doc[EFFECT_LIGHT] = lightActive;
        doc[EFFECT_COLOR]["active"] = colorActive;
        doc[EFFECT_COLOR]["red"] = leds[NUM_LEDS / 2].r;
        doc[EFFECT_COLOR]["green"] = leds[NUM_LEDS / 2].g;
        doc[EFFECT_COLOR]["blue"] = leds[NUM_LEDS / 2].b;
        doc[EFFECT_RAINBOW] = rainbowActive;
        doc[EFFECT_FIRE] = fireActive;
        doc[EFFECT_RANDOM] = randomActive;

        String json = "";
        serializeJson(doc, json);

        requestSync->send(200, "application/json", json);
    });

    server.onNotFound(notFound);
    server.begin();
}

//  _
// | |
// | |        ___     ___    _ __
// | |       / _ \   / _ \  | '_ \ 
// | |____  | (_) | | (_) | | |_) |
// |______|  \___/   \___/  | .__/
//                          | |
//                          |_|

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
}