# Getting started

## Installation

* Open GitKraken Press + / New tab and select Clone a Repo (Or another git management tool)
* Select the folder on your local drive where to copy to (e.g. /github/MoonModules)
* Copy [MoonLight.git](https://github.com/MoonModules/MoonLight.git) (or [ESP32-svelte-live-mm.git](https://github.com/ewowi/ESP32-svelte-live-mm)) and paste in the URL field of GitKraken
* Press clone the repo
* Open VSCode
* Install the PlatformIO IDE extension
* Open the folder created with GitKraken
* Connect an ESP32 or an ESP32-S3 with USB to your computer
* On the status bar select the env to flash and the board to flash to
    * Select esp32dev for a normal ESP32
    * Select esp32-s3-devkitc-1-n16r8v for an ESP32-S2 ([recommended](https://s.click.aliexpress.com/e/_DBAtJ2H) or similar)
* Press PlaformIO:Upload on the statusbar
* The formware is now flashed to your board, after flashing the board will reboot

## Configuration

* Recommended: Press PlatformIO:Serial Monitor to see the debug information produced
* In case of a newly flashed board, a Wifi access point (AP) will be created. Go to your Wifi settings on your computer and find the new Wifi AP (starts with MoonLight or ESPSvelteKit)
* Connect to the AP (no password needed), a captive portal will show with a welcome screen.
* Go to the menu and select Wifi / Wifi Station
* Press (+) and enter the SSID and the password of your Wifi Network
* Press Add Network and Apply settings
* The board will reconnect to your Wifi Network
* Go back to your Wifi Network on your computer
* Find out the new IP of the board, or in your Wifi Network settings or by looking at the log in the Serial Monitor (see above)

## ESP32-Svelte-Live-MM specific

* Connect a 256 leds panel to the board
* Search for FastLED.addLeds in the code and update to the pin you use to drive LEDS, reflash the code to the board
* Go to the UI in the browser
* Go to Custom / Files and create or upload Live scripts 
* Go to Custom / Live Animation and select the 'hardcoded animations' (Random, Rainbow)
* Select any of the Live scripts you uploaded, check the Serial Output for results.
* Open the Edit area and change things in the code, see the Serial Output for results
* To do
    * Find a way to present feedback to the UI (e.g. error messages as comments in the sc file)
    * Extend the code to support led animations ('homework assignment')
    * see LiveAnimation.cpp (Server) and LiveAnimation.svelte (UI)

### Current script supported

void setup() {
  printf("Run Live Script good afternoon\n");
}
void main() {
  setup();
}

### Homework assignment

void setup() {
  printf("Run Live Script good morning\n");
}
void loop() {
  fadeToBlackBy(40);
  leds[random16(255)] = CRGB(0, 0, 255);
}
void main() {
  setup();
  while (true) {
    loop();
    sync(); //or show??
  }
}