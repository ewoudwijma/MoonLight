/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2024 theelims
 *file
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <FixtureService.h>

#define EVENT_MONITOR "monitor"

void FixtureState::read(FixtureState &state, JsonObject &root)
{
    root["lightsOn"] = state.lightsOn;
    root["brightness"] = state.brightness;
    root["width"] = state.width;
    root["height"] = state.height;
    root["depth"] = state.depth;
    root["driverOn"] = state.driverOn;
    root["monitorOn"] = state.monitorOn;
    root["pin"] = state.pin;
}

StateUpdateResult FixtureState::update(JsonObject &root, FixtureState &state)
{
    bool changed = false;
    bool reInitDriver = false;

    if (state.lightsOn != root["lightsOn"]) {state.lightsOn = root["lightsOn"]; changed = true;}
    if (state.brightness != root["brightness"]) {
        state.brightness = root["brightness"]; changed = true;

        FastLED.setBrightness(state.lightsOn?state.brightness/8:0);

        Serial.printf("Fixture.brightness.update %d\n", state.brightness);
    }
    if (state.width != root["width"]) {
        state.width = root["width"]; changed = true;
        reInitDriver = true;
    }
    if (state.height != root["height"]) {
        state.height = root["height"]; changed = true;
        reInitDriver = true;
    }
    if (state.depth != root["depth"]) {
        state.depth = root["depth"]; changed = true;
        reInitDriver = true;
    }
    if (state.driverOn != root["driverOn"]) {
        state.driverOn = root["driverOn"]; changed = true;
    }
    if (state.monitorOn != root["monitorOn"]) {state.monitorOn = root["monitorOn"]; changed = true;}
    if (state.pin != root["pin"]) {
        state.pin = root["pin"]; changed = true;
        reInitDriver = true;
        Serial.printf("Fixture.pin.update %d\n", state.pin);
    }

    if (changed)
        Serial.printf("FixtureState::update o:%d b:%d\n", state.lightsOn, state.brightness);

    if (reInitDriver) {
        Serial.printf("FixtureState::reInitDriver o:%d b:%d\n", state.lightsOn, state.brightness);
        uint16_t nrOfLeds = min(state.width * state.height * state.depth, STARLIGHT_MAXLEDS);

        switch (state.pin) {
            case 11: FastLED.addLeds<NEOPIXEL, 11>(state.ledsP, 0, nrOfLeds).setCorrection(TypicalLEDStrip); break;
            case 12: FastLED.addLeds<NEOPIXEL, 12>(state.ledsP, 0, nrOfLeds).setCorrection(TypicalLEDStrip); break;
            case 13: FastLED.addLeds<NEOPIXEL, 13>(state.ledsP, 0, nrOfLeds).setCorrection(TypicalLEDStrip); break;
            case 14: FastLED.addLeds<NEOPIXEL, 14>(state.ledsP, 0, nrOfLeds).setCorrection(TypicalLEDStrip); break;
            case 15: FastLED.addLeds<NEOPIXEL, 15>(state.ledsP, 0, nrOfLeds).setCorrection(TypicalLEDStrip); break;
            case 16: FastLED.addLeds<NEOPIXEL, 16>(state.ledsP, 0, nrOfLeds).setCorrection(TypicalLEDStrip); break;
        }
    }

    return changed?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
}

FixtureService::FixtureService(PsychicHttpServer *server,
                                     EventSocket *socket,
                                     SecurityManager *securityManager, FS *fs) : _httpEndpoint(FixtureState::read,
                                                                                                         FixtureState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/fixtureState",
                                                                                                         securityManager,
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(FixtureState::read,
                                                                                                          FixtureState::update,
                                                                                                          this,
                                                                                                          socket,
                                                                                                          "fixture"),
                                                                                           _webSocketServer(FixtureState::read,
                                                                                                            FixtureState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/fixtureState",
                                                                                                            securityManager,
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(socket),
                                                                                             _fsPersistence(FixtureState::read,
                                                                                                      FixtureState::update,
                                                                                                      this,
                                                                                                      fs,
                                                                                                      "/config/fixtureState.json")
{

    // configure settings service update handler to update state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);

    //difference with state::update ???
}

void FixtureService::begin()
{
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _fsPersistence.readFromFS();

    onConfigUpdated();

    FastLED.setMaxPowerInMilliWatts(10 * 1000); // 5v, 2000mA

    _socket->registerEvent(EVENT_MONITOR);
}

void FixtureService::onConfigUpdated()
{
    Serial.printf("FixtureService::onConfigUpdated o:%d b:%d\n", _state.lightsOn, _state.brightness);
}

void FixtureService::loop()
{
    if (_state.driverOn)
        FastLED.show();

}

void FixtureService::loop50ms()
{
    if (_state.monitorOn)
        _socket->emitEvent(EVENT_MONITOR, (char *)_state.ledsP, min(_state.width*_state.height*_state.depth, STARLIGHT_MAXLEDS) * sizeof(CRGB)); // * sizeof(CRGB)
}