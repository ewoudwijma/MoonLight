/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2024 theelims
 *   Copyright (C) 2025 - 2025 ewowi
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <FixtureService.h>

#include "App/LedModFixture.h" // use fix->

#define EVENT_MONITOR "monitor"

void FixtureState::read(FixtureState &state, JsonObject &root)
{
    ppf("FixtureState::read\n");
    root["lightsOn"] = state.lightsOn;
    root["brightness"] = state.brightness = Variable("Fixture", "brightness").getValue();
    root["fixture"] = state.fixture;
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
    bool sizeChanged = false;
    bool pinChanged = false;

    print->printJson("FixtureState::update", root);

    if (state.lightsOn != root["lightsOn"]) {
        state.lightsOn = root["lightsOn"]; changed = true;
        Variable("Fixture", "on").setValue(state.lightsOn);
    }
    if (state.brightness != root["brightness"]) {
        state.brightness = root["brightness"]; changed = true;
        Variable("Fixture", "brightness").setValue(state.brightness);

        Serial.printf("Fixture.brightness.update %d\n", state.brightness);
    }
    if (state.fixture != root["fixture"]) {
        state.fixture = root["fixture"]; changed = true;

        ppf("Fixture.fixture.update task: %s", pcTaskGetTaskName(nullptr));
        ppf(" e:%d\n", state.fixture);

        Variable("Fixture", "fixture").setValue(state.fixture);
        Variable("Fixture", "fixture").setValue(state.fixture); //twice to init var["value"]correctly - workaround !!!
    }

    if (state.width != root["width"]) {
        state.width = root["width"]; changed = true;
        sizeChanged = true;
    }
    if (state.height != root["height"]) {
        state.height = root["height"]; changed = true;
        sizeChanged = true;
    }
    if (state.depth != root["depth"]) {
        state.depth = root["depth"]; changed = true;
        sizeChanged = true;
    }
    if (state.driverOn != root["driverOn"]) {
        state.driverOn = root["driverOn"]; changed = true;
        fix->showDriver = state.driverOn;
    }
    if (state.monitorOn != root["monitorOn"]) {state.monitorOn = root["monitorOn"]; changed = true;}
    if (state.pin != root["pin"]) {
        state.pin = root["pin"]; changed = true;
        pinChanged = true;
        fix->doAllocPins = true;
        fix->currPin = state.pin;
        Serial.printf("Fixture.pin.update %d\n", state.pin);
    }

    // if (changed)
    //     Serial.printf("FixtureState::update o:%d b:%d\n", state.lightsOn, state.brightness);

    if (sizeChanged) {
        Serial.printf("FixtureState::sizeChanged %dx%dx%d\n", state.width, state.height, state.depth);

        if (state.width > 0 && state.height > 0 && state.depth > 0) {
            // Coord3D fixSize = {state.width, state.height, state.depth};
            // Variable("Fixture", "fixture").setValue(fixSize);

            // fix->fixSize.x = state.width;
            // fix->fixSize.y = state.height;
            // fix->fixSize.z = state.depth;
        }
    }

    if (sizeChanged || pinChanged) {
        fix->mappingStatus = 1; // ask starlight to recalculate mapping (including pins)
    }
    if (changed)
        Variable("Model", "saveModel").setValue(true);

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

    _socket->registerEvent(EVENT_MONITOR);
}

void FixtureService::onConfigUpdated()
{
    Serial.printf("FixtureService::onConfigUpdated o:%d b:%d\n", _state.lightsOn, _state.brightness);
}

void FixtureService::loop50ms()
{
    if (_state.monitorOn)
        _socket->emitEvent(EVENT_MONITOR, (char *)fix->ledsP, min(_state.width*_state.height*_state.depth, STARLIGHT_MAXLEDS) * sizeof(CRGB));
    //ran by httpd, is that okay or better to run in other task?
}