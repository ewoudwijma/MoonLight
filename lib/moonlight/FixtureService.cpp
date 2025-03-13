/**
    @title     MoonLight
    @file      FixtureService.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/fixture/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#include <FixtureService.h>

#include "App/LedModFixture.h" // use fix-> (and Variable)

#define EVENT_MONITOR "monitor"

void FixtureState::read(FixtureState &state, JsonObject &root)
{
    ESP_LOGI("", "FixtureState::read");
    root["lightsOn"] = state.lightsOn;
    root["brightness"] = state.brightness = Variable("Fixture", "brightness").getValue();
    root["fixture"] = state.fixture;
    // root["width"] = state.width;
    // root["height"] = state.height;
    // root["depth"] = state.depth;
    root["driverOn"] = state.driverOn;
    #if FT_ENABLED(FT_MONITOR)
        root["monitorOn"] = state.monitorOn;
    #endif
    // root["pin"] = state.pin;
}

StateUpdateResult FixtureState::update(JsonObject &root, FixtureState &state)
{
    bool changed = false;
    // bool sizeChanged = false;
    // bool pinChanged = false;

    print->printJson("FixtureState::update", root);

    if (state.lightsOn != root["lightsOn"]) {
        state.lightsOn = root["lightsOn"]; changed = true;
        Variable("Fixture", "on") = state.lightsOn;
    }
    if (state.brightness != root["brightness"]) {
        state.brightness = root["brightness"]; changed = true;
        Variable("Fixture", "brightness") = state.brightness;

        ESP_LOGI("", "Fixture.brightness.update %d", state.brightness);
    }
    if (state.fixture != root["fixture"]) {
        state.fixture = root["fixture"]; changed = true;

        ESP_LOGI("", "Fixture.fixture.update task: %s e:%d", pcTaskGetTaskName(nullptr), state.fixture);

        // if (!sys->safeMode && false) {
            Variable("Fixture", "fixture") = state.fixture;
        // }
    }

    // if (state.width != root["width"]) {
    //     state.width = root["width"]; changed = true;
    //     sizeChanged = true;
    // }
    // if (state.height != root["height"]) {
    //     state.height = root["height"]; changed = true;
    //     sizeChanged = true;
    // }
    // if (state.depth != root["depth"]) {
    //     state.depth = root["depth"]; changed = true;
    //     sizeChanged = true;
    // }
    if (state.driverOn != root["driverOn"]) {
        state.driverOn = root["driverOn"]; changed = true;
        fix->showDriver = state.driverOn;
    }
    #if FT_ENABLED(FT_MONITOR)
        if (state.monitorOn != root["monitorOn"]) {state.monitorOn = root["monitorOn"]; changed = true;}
    #endif
    // if (state.pin != root["pin"]) {
    //     state.pin = root["pin"]; changed = true;
    //     pinChanged = true;
    //     fix->doAllocPins = true;
    //     fix->currPin = state.pin;
    //     ESP_LOGI("", "Fixture.pin.update %d", state.pin);
    // }

    // if (sizeChanged) {
    //     ESP_LOGI("", "FixtureState::sizeChanged %dx%dx%d", state.width, state.height, state.depth);

    //     if (state.width > 0 && state.height > 0 && state.depth > 0) {
    //         // Coord3D fixSize = {state.width, state.height, state.depth};
    //         // Variable("Fixture", "fixture") = fixSize;

    //         fix->fixSize.x = state.width;
    //         fix->fixSize.y = state.height;
    //         fix->fixSize.z = state.depth;
    //     }
    // }

    // if (sizeChanged || pinChanged) {
    //     fix->mappingStatus = 1; // ask starlight to recalculate mapping (including pins)
    // }
    // if (changed)
    //     Variable("Model", "saveModel").setValue(true);

    return changed?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
}

FixtureService::FixtureService(PsychicHttpServer *server,
                                     ESP32SvelteKit *sveltekit) : _httpEndpoint(FixtureState::read,
                                                                                                         FixtureState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/fixtureState",
                                                                                                         sveltekit->getSecurityManager(),
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(FixtureState::read,
                                                                                                          FixtureState::update,
                                                                                                          this,
                                                                                                          sveltekit->getSocket(),
                                                                                                          "fixture"),
                                                                                           _webSocketServer(FixtureState::read,
                                                                                                            FixtureState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/fixtureState",
                                                                                                            sveltekit->getSecurityManager(),
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(sveltekit->getSocket()),
                                                                                             _fsPersistence(FixtureState::read,
                                                                                                      FixtureState::update,
                                                                                                      this,
                                                                                                      sveltekit->getFS(),
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
    ESP_LOGI("", "FixtureService::onConfigUpdated o:%d b:%d", _state.lightsOn, _state.brightness);
}

void FixtureService::loop50ms()
{
    #if FT_ENABLED(FT_MONITOR)
        static int monitorMillis = 0; //max 12000 leds per second
        if (_state.monitorOn && fix->mappingStatus == 0 && millis() - monitorMillis >= fix->nrOfLeds / 12) {
            monitorMillis = millis();
            _socket->emitEvent(EVENT_MONITOR, (char *)(&fix->ledsPExtended), MIN(fix->nrOfLeds, STARLIGHT_MAXLEDS) * sizeof(CRGB) + 3); //3 bytes for type and factor and ...
        }
    #endif
    if (fix->ledsPExtended.type == 1) {
        ESP_LOGI("", "New fixture!");
        fix->ledsPExtended.type = 0; //reset fixChange
    }
    //ran by httpd, is that okay or better to run in other task?
}