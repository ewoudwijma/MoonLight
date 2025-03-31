# Getting started

## Installation - Developer

* Open GitKraken Press + / New tab and select Clone a Repo (Or another git management tool)
* Select the folder on your local drive where to copy to (e.g. /github/MoonModules)
* Copy [MoonBase.git](https://github.com/ewowi/MoonBase.git) or [MoonLight.git](https://github.com/MoonModules/MoonLight.git) and paste in the URL field of GitKraken
* Press clone the repo
* Open VSCode
* Install the PlatformIO IDE extension
* Open the folder created with GitKraken
* Connect an ESP32 or an ESP32-S3 with USB to your computer
* On the status bar select the env to flash and the board to flash to
    * Select esp32dev for a normal ESP32
    * Select esp32-s3-devkitc-1-n16r8v for an ESP32-S2 ([recommended](https://s.click.aliexpress.com/e/_DBAtJ2H) or similar)

   <img width="617" alt="image" src="https://github.com/user-attachments/assets/349af246-30c7-45dd-92ed-4f2b3900557f" />

* Press PlaformIO:Upload (->) on the statusbar
* The firmware is now flashed to your board, after flashing the board will reboot
* Recommended: Press PlatformIO:Serial Monitor to see the debug information produced

## Installation - End user

[install](https://moonmodules.org/MoonLight/general/index.html)

* release 0.5.4, for latest build follow installation developer
* This is WIP, looks like issue with erasing the board and setting the partitions right ...

## Configuration

* In case of a newly flashed board, a Wifi access point (AP) will be created. Go to your Wifi settings on your computer and find the new Wifi AP (starts with MoonBase, MoonLight or ESPSvelteKit)
    * If the board AP is not showing up in your WiFi list it might be helpful to fully erase the board (vscode 👽, Erase flash)
* Connect to the AP (no password needed), a captive portal will show with a welcome screen.
* Go to the menu and select Wifi / Wifi Station
* Press (+) and enter the SSID and the password of your Wifi Network
* Press Add Network and Apply settings
* The board will reconnect to your Wifi Network
* Go back to your Wifi Network on your computer
* Find out the new IP of the board, or in your Wifi Network settings or by looking at the log in the Serial Monitor (see above)

## Developing

* Read the [ESP32 Sveltekit docs](https://moonmodules.org/MoonLight/eskIndex/) (Latest version (Svelte 5) here [ESP32 Sveltekit docs](https://theelims.github.io/ESP32-sveltekit/))
* Read [Customizing Sveltekit](https://moonmodules.org/MoonLight/general/customizingsveltekit/)
* UI dev: configure vite.config.ts, go to interface folder, npm install, npm run dev. A local webserver starts on localhost. UI changes will directly be shown via this webserver
* Want to make changes: fork the repo and submit pull requests

## MoonBase specific

* Connect a 256 leds panel to the board
* Search for FastLED.addLeds in the code and update to the pin you use to drive LEDS, reflash the code to the board
* Go to the UI in the browser
* Go to Custom / Files and create or upload Live scripts 
* Go to Custom / Module Animations and select the 'hardcoded animations' (Random, Sinelon, Rainbow), you should see it on your panel
* Select any of the Live scripts you uploaded, check the Serial Output for results. (No led output in current version, see below)
* Open the Edit area and change things in the code, see the Serial Output for results

<img width="350" alt="image" src="https://github.com/user-attachments/assets/56bdd019-927b-40cc-9199-9bc6344f8d8b" />


* To do
    * Find a way to present feedback to the UI (e.g. error messages as comments in the sc file)
    * Extend the code in [ModuleAnimations.h](https://github.com/ewowi/MoonBase/blob/main/src/custom/ModuleAnimations.h) to support led animations ('homework assignment')
    * see ModuleAnimation.h (Server)

### Current script supported

```
void setup() {
  printf("Run Live Script good afternoon\n");
}
void main() {
  setup();
}
```

### Homework assignment

```
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
```
