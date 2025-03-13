/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2024 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <ESP32SvelteKit.h>
#include <LightMqttSettingsService.h>
#include <LightStateService.h>
#include <PsychicHttpServer.h>

#include <StarService.h>
#include <FilesService.h>
#include <FixtureService.h>
#include <EffectsService.h>

#define SERIAL_BAUD_RATE 115200

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 120);

LightMqttSettingsService lightMqttSettingsService = LightMqttSettingsService(&server,
                                                                             &esp32sveltekit);

LightStateService lightStateService = LightStateService(&server,
                                                        &esp32sveltekit,
                                                        &lightMqttSettingsService);

#if FT_ENABLED(FT_FILEMANAGER)
    FilesService filesService = FilesService(&server, &esp32sveltekit);
#endif
#if FT_ENABLED(FT_MOONLIGHT)
    StarService starService = StarService(&server, &esp32sveltekit);
    FixtureService fixtureService = FixtureService(&server, &esp32sveltekit);
    EffectsService effectsService = EffectsService(&server, &esp32sveltekit, &fixtureService);
    #include "mainStar.h"
#endif

// #define ESP_LOGD(tag, format, ...) ppf(format, ##__VA_ARGS__)
int my_vprintf(const char *format, va_list args)
{
    Serial.print(".");
    // Implement your logging logic here
    return 1;//vprintf(format, args);
}

WiFiUDP instanceUDP;
const int instanceUDPPort = 65506;

// AsyncUDP artnetudp;// AsyncUDP so we can just blast packets.

void setup()
{
    // sys->safeMode = true; //e.g. in case of a crash

    // start serial and filesystem
    Serial.begin(SERIAL_BAUD_RATE);

    // delay(500); //see WLED/StarLight / Serial needs time to init ..., add ifdef?

    esp_log_set_vprintf(my_vprintf); //not working yet ...

    #if FT_ENABLED(FT_MOONLIGHT)
        setupStar(); //before ESK as ESK loop uses star stuff
    #endif

    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    #if FT_ENABLED(FT_FILEMANAGER)
        filesService.begin();
    #endif
    #if FT_ENABLED(FT_MOONLIGHT)
        starService.begin();
        fixtureService.begin(); //before effectservice!
        effectsService.begin();
    #endif

    // load the initial light settings
    lightStateService.begin();
    // start the light service
    lightMqttSettingsService.begin();

    // ESP_LOGV("Star", "Starting loop task");
    // xTaskCreateUniversal(
    //     [](void * par) { loopStar(); },            // Function that should be called
    //     "StarLoop",     // Name of the task (for debugging)
    //     8192,                       // Stack size (bytes)
    //     nullptr,                       // Pass reference to this class instance
    //     (tskIDLE_PRIORITY + 1),     // task priority
    //     NULL,                       // Task handle
    //     ARDUINO_RUNNING_CORE // Pin to application core
    // );

    instanceUDP.begin(instanceUDPPort); //instances

    update_handler_id_t _updateHandlerId = filesService.addUpdateHandler([&](const String &originId)
    { 
        ESP_LOGD("", "FilesService::updateHandler %s", originId.c_str());
        filesService.read([&](FilesState &state) {
            for (auto changedFile : state.changedFiles) {
                ESP_LOGD("", "FilesService::updateHandler changedFiles %s", changedFile.c_str());
            }
        });
     });



}

void loop()
{
    // Delete Arduino loop task, as it is not needed in this example
    // vTaskDelete(NULL);

    uint32_t cycles = ESP.getCycleCount();
    esp32sveltekit.loopsPerSecond++;

    #if FT_ENABLED(FT_MOONLIGHT)
        static int fiftyMsMillis = 0;

        int packetSize = 0;
        if (millis() - fiftyMsMillis >= 50) {
            fiftyMsMillis = millis();

            fixtureService.loop50ms();

            if (false) {
                //send udp broadcast packet
                packetSize = instanceUDP.parsePacket();
                instanceUDP.remoteIP();
                if (packetSize > 0) {
                    byte *udpIn = (byte *)malloc(packetSize);
                    instanceUDP.read(udpIn, packetSize);
                    // free(udpIn);
                }

                if (instanceUDP.beginPacket(IPAddress(255, 255, 255, 255), instanceUDPPort)) {  // WLEDMM beginPacket == 0 --> error
                    JsonDocument message;
                    message["pid"] = "pid";
                    message["id"] = "id";
                    message["value"] = "value";
            
                    size_t len = measureJson(message);
            
                    char buffer[len];
            
                    serializeJson(message, buffer, len);
            
                    instanceUDP.write((byte *)buffer, len);
                    instanceUDP.endPacket();
                }

                // if (!artnetudp.writeTo(packet_buffer, packetSize+18, targetIp, ARTNET_DEFAULT_PORT)) {
            }
        }

        loopStar();
    #endif

    esp32sveltekit.cyclesPerSecond += (ESP.getCycleCount() - cycles); //add the new cycles to the total cpu time

    while (!runInLoopTask.empty())
    {
        ESP_LOGD("Main", "Running queued function");
        runInLoopTask.back()(); //calls the last function
        runInLoopTask.pop_back();
    }

}
