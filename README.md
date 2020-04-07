# 🌈 ESP-RGB-Controller
<p align="center"><img src="https://raw.githubusercontent.com/WoodyLetsCode/ESP-RGB-Controller/master/ESP-RGB-Controller.png"></p>

## 👋 About this project
This project is for controlling RGB-Strips (e.g. WS2801) using an ESP8266 board like the NodeMCU. You can controll it using a Web-GUI or a cross-platform software (Windows, Linux, Mac). If you want to do your own thing, you can just use this project to send POST requests to the ESP. This allows you to do much things with your RGB-Strips like integrate it into your home automation or controll it via Google Assistant.

## ⚙️ Features
- Many different effects ✅
- Web-GUI ✅
- Buzzer support ✅
- Asynchronous ✅
### Future releases:
- Multi-Platform software (Windows, Linux, Mac) 🔜
- Custom PCB 🔜
- OTA-Updates 🔜

## 💾 Supported platforms
- ESP8266 boards (like NodeMCU) ✅
- ESP32 boards ❔

## 🖼️ Images
<img src="https://github.com/WoodyLetsCode/ESP-RGB-Controller/raw/master/iphone-xs-with-macbook-air-in-the-background.jpg" width="50%"><img src="https://github.com/WoodyLetsCode/ESP-RGB-Controller/raw/master/ipad-pro-on-white-table.jpg" width="50%">

## 📲 Installation
You can find a full installation and setup guide in the <a href="https://github.com/WoodyLetsCode/ESP-RGB-Controller/wiki">Wiki</a>.
 - <a href="https://github.com/WoodyLetsCode/ESP-RGB-Controller/wiki/Installation">Installation</a>
 - <a href="https://github.com/WoodyLetsCode/ESP-RGB-Controller/wiki/Setup">Setup</a>
 
 ## 📍 Pin Layout
The following table shows how to connect your hardware.

|    Hardware    	| GPIO 	| NodeMCU 	|
|:--------------:	|:----:	|:-------:	|
|  WS2801 (Data) 	|  13  	|    D7   	|
| WS2801 (Clock) 	|  14  	|    D5   	|
|     Buzzer     	|  15  	|    D8   	|
|    Button 1    	|  16  	|    D0   	|
| Button 1 (LED) 	|   0  	|    D3   	|
|    Button 2    	|   5  	|    D1   	|
| Button 2 (LED) 	|   2  	|    D4   	|
|    Button 3    	|   4  	|    D2   	|
| Button 3 (LED) 	|  12  	|    D6   	|

## 📒 Used libraries
- <a href="https://github.com/me-no-dev/ESPAsyncWebServer">ESPAsyncWebServer</a>
- <a href="https://github.com/FastLED/FastLED">FastLED</a>
- <a href="https://github.com/alanswx/ESPAsyncWiFiManager">AsyncWiFiManager</a>

## ✌️ Credits
Thanks to all contributors and creators of the libraries that i used!
