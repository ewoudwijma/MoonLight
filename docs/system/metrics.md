# System Metrics

<img width="532" alt="image" src="https://github.com/user-attachments/assets/7fd9a24f-593b-447f-be6d-f37881ecd32c" />

## Functional

Shows system info on a timeline

* <img width="30" src="https://github.com/user-attachments/assets/b0e8af99-ed76-422a-8bd1-bfbd9e0f4c44"/> Performance: show the loops per second of the main application loop. In case of running LEDs this is the Frames Per Second displayed on led fixtures.
* <img width="30" src="https://github.com/user-attachments/assets/b0e8af99-ed76-422a-8bd1-bfbd9e0f4c44"/> PSRAM: Shows the used size of PSRAM, if present

## Technical

### Server

[SystemStatus.h](https://github.com/MoonModules/MoonLight/blob/main/lib/framework/SystemStatus.h) and [SystemStatus.cpp](https://github.com/MoonModules/MoonLight/blob/main/lib/framework/SystemStatus.cpp)

### UI

[SystemStatus.svelte](https://github.com/MoonModules/MoonLight/blob/main/interface/src/routes/system/status/SystemStatus.svelte)
