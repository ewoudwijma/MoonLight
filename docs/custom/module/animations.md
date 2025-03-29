# Animations module

<img width="517" alt="image" src="https://github.com/user-attachments/assets/022fde4d-9a3b-456c-ade5-e18219e5a5d5" />

## Functional

* On: lights on or off
* Brightness: brightness of the LEDs when on
* driverOn: sends LED output to ESP32 gpio pins.
    * Switch off to see the effect framerate in System Status/Metrics
    * Switch on to see the effect framerate throttled by a LED driver in System Status/Metrics (800KHz, 256 leds, 24 bits is 130 fps theoretically - 120 practically)
* Nodes: One or more processes, can be fixture definitions, mappings, effects, projections. Currently all nodes are effects.
* Scrips: Running Live scripts (WIP)

## Technical

* See [Modules](https://moonmodules.org/MoonLight/custom/modules/)

### Server

[Instances.h](https://github.com/MoonModules/MoonLight/blob/main/lib/framework/Instances.h) and [Instances.cpp](https://github.com/MoonModules/MoonLight/blob/main/lib/framework/Instances.cpp)

### UI

[Instances.svelte](https://github.com/MoonModules/MoonLight/blob/main/interface/src/routes/system/status/Instances.svelte)
