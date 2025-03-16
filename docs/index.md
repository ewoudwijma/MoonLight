---
hide:
  - navigation
  - toc
---

# MoonLight

<img width="500" alt="MoonLight" src="https://github.com/user-attachments/assets/de0ab735-d547-462e-b7e3-c3f819bf9283" />

* MoonLight is a [MoonModules](https://moonmodules.org) project.
* MoonLight = [ESP32 SvelteKit](https://github.com/theelims/ESP32-sveltekit) + [StarLight as a Service](https://github.com/MoonModules/StarLight/tree/StarAsAService). Check [Discord/MoonLight](https://discord.gg/TC8NSUSCdV) to discuss.
* Work in progress is shown in the [Kanban board](https://github.com/users/ewowi/projects/2). You are more then welcome to add issues, comment on issues or help with issues.
* See also [ESP32-sveltekit POC](https://github.com/theelims/ESP32-sveltekit/issues/68)
* Documentation has been setup (but not customized), see [MoonLightDocs](https://MoonModules.github.io/MoonLight/).

[Release 0.5.2](https://github.com/MoonModules/MoonLight/releases/tag/v0.5.2) (Jan 21, 2025): 

<img src="https://github.com/user-attachments/assets/c655d610-53eb-4dd3-8e9e-0cfa23b97bb4"/>

## Customizing upstream

### Customize UI and app specifiv

* {custom} = MoonLight

* docs/media/
    * add {custom}-logo.png
* factory_settings.ini
    * FACTORY_AP_SSID=\"{custom}-#{unique_id}\"
    * FACTORY_AP_PASSWORD=\"\" (recommendation)
    * FACTORY_NTP_TIME_ZONE_LABEL=\"Europe/Amsterdam\"
* package.json
    * name = "{custom}"
    * version: "0.5.1",
* intrerface/source/lib/assets/logo.png
    * replace logo
* interface/source/routes/+layout.ts
    * title: '{custom}'
    * github:
    * copyright
    * appname: '{custom}'
* interface/source/routes/+page.svelte
    * Welcome to {custom}
    * Intro message
    * href="/" 
    * Start {custom}
* interface/source/routes/menu.svelte
    * const discord = { href: 'https://discord.gg/TC8NSUSCdV', active: true };
* interface/static/manifest.json
    * name: "{custom}"
* interface/static/favicon.png
    * replace favicon.png
* lib/framework/APSettingsService.h
    * FACTORY_AP_SSID "{custom}-#{unique_id}"
    * FACTORY_AP_PASSWORD ""
* mkdocs.yml
    * site_name: {custom}
    * nav: {custom}
    * repo_name and repo_url
    * theme logo: media/{custom}-logo.png
    * analytics: provider: google property: G-R6QYDG0126
    * Copyright
* platformio.ini
    * description = {custom}
    * APP_NAME=\"{custom}\" ;
    * APP_VERSION=\"0.x.0\"
    * CORE_DEBUG_LEVEL=4
* README.md
    *  Custom intro
* vite.config.ts
    * Set target: 'http://192.168.1.156'

### Other improvements

* Add esp32-s3-devkitc-1-n16r8v and LOLIN_WIFI_FIX in pio.ini (including boards folder)
* Add free_psram, used_psram and psram_size in Analytics (models.ts) and analytics_data (analytics.ts) and show in UI (SystemMetrics.svelte)
* Send psram data only if psramFound (SystemStatus.svelte, AnalyticsService.h, SystemStatus.cpp)
* Add File manager
* Add Monitor
    * socket.ts: add else listeners.get("monitor")?.forEach((listener) => listener(new Uint8Array(message.data)));
    * EventSocket.cpp: add void EventSocket::emitEvent with char * argument
* Add MoonLight specific functionality (Currently fixtures and effects)
* ESP32SvelteKit.cpp: 
    * CPU load, loops per second (and main.cpp)
    * comment response.addHeader("Cache-Control", "public, immutable, max-age=31536000");
* interface/source/routes/+layout.svelte
    * Don't show if captive portal: {#if (!window.location.href.includes("192.168.4.1") && $page.data.features.monitor)}
* interface/source/routes/statusbar.svelte
    * show help: <a href="https://moonmodules.org{window.location.pathname} target="_blank">?</a>
* main.cpp: esp_log_set_vprintf(my_vprintf); WIP
* ci pio

## License

MoonLight: GPL-v3
ESP32 SvelteKit see [ESP32 SvelteKit license](https://github.com/theelims/ESP32-sveltekit?tab=License-1-ov-file#)