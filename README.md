# MoonBase / MoonLight

April 2025: 

**Development has moved to [MoonModules/MoonLight](https://github.com/MoonModules/MoonLight)**. 
**See [Star-Mod-Base-Light-Moon-Svelte-Live](https://moonmodules.org/Star-Mod-Base-Light-Moon-Svelte-Live)**


<img width="500" alt="MoonLight" src="https://github.com/user-attachments/assets/de0ab735-d547-462e-b7e3-c3f819bf9283" />

* MoonBase and MoonLight are [MoonModules](https://moonmodules.org) projects.
* MoonBase = [ESP32 SvelteKit](https://github.com/theelims/ESP32-sveltekit) + [ESPLiveScript]([https://github.com/hpwit/StarLight/tree/StarAsAService](https://github.com/hpwit/ESPLiveScript)) + [StarBase](https://github.com/ewowi/StarBase). Check [Discord/MoonLight](https://discord.gg/TC8NSUSCdV) to discuss.
* MoonLight = MoonBase + [StarLight as a Service](https://github.com/MoonModules/StarLight/tree/StarAsAService). Check [Discord/MoonLight](https://discord.gg/TC8NSUSCdV) to discuss.
* MoonBase is the current work in progress repo, see [Star-Mod-Base-Light-Moon-Svelte-Live](https://moonmodules.org/Star-Mod-Base-Light-Moon-Svelte-Live). MoonLight will be updated later.
* Work in progress is shown in the [Kanban board](https://github.com/users/MoonModules/projects/2). You are more then welcome to add issues, comment on issues or help with issues.
You are more then welcome to add issues, comment on issues or help with issues.
* See also [ESP32-sveltekit POC](https://github.com/theelims/ESP32-sveltekit/issues/68)
* Documentation has been setup (but not customized), see [MoonLightDocs](https://MoonModules.github.io/MoonLight/).

[Release 0.5.2](https://github.com/MoonModules/MoonLight/releases/tag/v0.5.2) (Jan 21, 2025): 

<img src="https://github.com/user-attachments/assets/c655d610-53eb-4dd3-8e9e-0cfa23b97bb4"/>

## License

MoonBase, MoonLight: GPL-v3
ESP32 SvelteKit see [ESP32 SvelteKit license](https://github.com/theelims/ESP32-sveltekit?tab=License-1-ov-file#)

MoonLight is a fork of:

# ESP32 SvelteKit - Create Amazing IoT Projects

<div style="flex">
<img src="/docs/media/Screenshot_light.png" style="height:320px"> 
<img src="/docs/media/Screenshot_mobile.png" style="height:320px"> 
</div>

A simple and extensible framework for ESP32 based IoT projects with a feature-rich, beautiful, and responsive front-end build with [Sveltekit](https://kit.svelte.dev/), [TailwindCSS](https://tailwindcss.com/) and [DaisyUI](https://daisyui.com/). This is a project template to get you started in no time backed by a powerful back end service, an amazing front end served from the ESP32 and an easy to use build chain to get everything going.

It was forked from the fabulous [rjwats/esp8266-react](https://github.com/rjwats/esp8266-react) project, from where it inherited the mighty back end services.

> **Tip**: This template repository is not meant to be used stand alone. If you're just looking for a WiFi manager there are plenty of options available. This is a starting point when you need a rich web UI.

## Features

### :butterfly: Beautiful UI powered by DaisyUI and TailwindCSS

Beautiful, responsive UI which works equally well on desktop and on mobile. Gently animated for a snappy and modern feeling without ever being obtrusive or in the way. Easy theming with DaisyUI and media-queries to respect the users wish for a light or dark theme.

### :t-rex: Low Memory Footprint and Easy Customization by Courtesy of SvelteKit

SvelteKit is ideally suited to be served from constrained devices like an ESP32. It's unique approach leads to very slim files. No bloatware like other popular JS frameworks. Not only the low memory footprint make it ideal but the developer experience is also outstanding letting you customize the front end with ease. Adapt and add functionality as you need it. The back end has you covered as well.

### :telephone: Rich Communication Interfaces

Comes with a rich set of communication interfaces to cover most standard needs of an IoT application. Like MQTT client, HTTP RESTful API, a WebSocket based Event Socket and a classic Websocket Server. All communication channels are stateful and fully synchronized. Changes propagate and are communicated to all other participants. The states can be persisted on the file system as well. For accurate time keeping time can by synchronized over NTP.

### :file_cabinet: WiFi Provisioning and Management

Naturally ESP32 SvelteKit comes with rich features to manage all your WiFi needs. From pulling up an access point for provisioning or as fall back, to fully manage your WiFi networks. Scan for available networks and connect to them. Advanced configuration options like static IP are on board as well.

### :old_key: Secured API and User Management

Manage different user of your app with two authorization levels. An administrator and a guest user. Authenticate their API calls with a JWT token. Manage the user's profile from the admin interface. Use at own risk, as it is neither secure without the ability to use TLS/SSL encryption on the ESP32 server, nor very convenient, as only an admin can change passwords.

### :airplane: OTA Upgrade Service

The framework can provide two different channels for Over-the-Air updates. Either by uploading a \*.bin file from the web interface. Or by pulling a firmware image from an update server. This is implemented with the github release page as an example. It is even possible to have different build environments at the same time and the Github OTA process pulls the correct binary.

### :building_construction: Automated Build Chain

The automated build chain takes out the pain and tears of getting all the bits and pieces play nice together. The repository contains a PlatformIO project at its heart. A SvelteKit project for the frontend code and a mkdocs project for the documentation go alongside. The PlatformIO build tools not only build the SvelteKit frontend with Vite, but also ensure that the build results are gzipped and find their way into the flash memory of the ESP32. You have two choices to serve the frontend either from the flash partition, or embedded into the firmware binary. The latter is much more friendly if your frontend code should be distributed OTA as well, leaving all configuration files intact.

### :icecream: Compatible with all ESP32 Flavours

The code runs on many variants of the ESP32 chip family. From the plain old ESP32, the ESP32-S3 and ESP32-C3. Other ESP32 variants might work, but haven't been tested. Sorry, no support for the older ESP8266. Go with one of the ESP32's instead.

## Visit the Project Site

[https://theelims.github.io/ESP32-sveltekit/](https://theelims.github.io/ESP32-sveltekit/)

## Libraries Used

- [SvelteKit](https://kit.svelte.dev/)
- [Tailwind CSS](https://tailwindcss.com/)
- [DaisyUI](https://daisyui.com/)
- [tabler ICONS](https://tabler-icons.io/)
- [unplugin-icons](https://github.com/antfu/unplugin-icons)
- [svelte-modals](https://svelte-modals.mattjennings.io/)
- [svelte-dnd-list](https://github.com/tarb/svelte-dnd-list)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [PsychicHttp](https://github.com/hoeken/PsychicHttp)
- [PsychicMqttClient](https://github.com/theelims/PsychicMqttClient)

## Licensing

ESP32 SvelteKit is distributed with two licenses for different sections of the code. The back end code inherits the GNU LESSER GENERAL PUBLIC LICENSE Version 3 and is therefore distributed with said license. The front end code is distributed under the MIT License. 

MoonLight back and front end is licensed under GPL-3.0. See the [LICENSE](LICENSE) for a full text.
