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

#include <EffectsService.h>

#include "App/LedModEffects.h" // set processEffectNr

void EffectsState::read(EffectsState &state, JsonObject &root)
{
    root["effect"] = state.effect;
    root["projection"] = state.projection;

    JsonArray array = root["nodes"].to<JsonArray>();
    JsonObject object = array.add<JsonObject>();
    object["name"] = "Ripples effect";
    array.add(object);
    object["name"] = "Lissajous effect";
    array.add(object);
    object["name"] = "Blender";
    array.add(object);
    object["name"] = "Game of life";
    // array.add(object);
}

StateUpdateResult EffectsState::update(JsonObject &root, EffectsState &state)
{
    bool changed = false;

    print->printJson("EffectsState::update", root);

    if (state.effect != root["effect"]) {
        state.effect = root["effect"]; changed = true;

        ppf("Effects.effect.update task: %s", pcTaskGetTaskName(nullptr));
        ppf(" e:%d\n", state.effect);

        Variable("layers", "effect").setValue(state.effect, 0);
        Variable("layers", "effect").setValue(state.effect, 0); //twice to init var["value"]correctly - workaround !!!
    }

    if (state.projection != root["projection"]) {
        state.projection = root["projection"]; changed = true;

        ppf("Effects.projection.update task: %s", pcTaskGetTaskName(nullptr));
        ppf(" e:%d\n", state.projection);

        Variable("layers", "projection").setValue(state.projection, 0);
        Variable("layers", "projection").setValue(state.projection, 0); //twice to init var["value"]correctly - workaround !!!
    }

    if (changed)
        Serial.printf("EffectsState::update");

    return changed?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
}

EffectsService::EffectsService(PsychicHttpServer *server,
                                     EventSocket *socket,
                                     SecurityManager *securityManager, FS *fs, FixtureService *fixtureService) : _httpEndpoint(EffectsState::read,
                                                                                                         EffectsState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/effectsState",
                                                                                                         securityManager,
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(EffectsState::read,
                                                                                                          EffectsState::update,
                                                                                                          this,
                                                                                                          socket,
                                                                                                          "effects"),
                                                                                           _webSocketServer(EffectsState::read,
                                                                                                            EffectsState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/effectsState",
                                                                                                            securityManager,
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(socket),
                                                                                             _fsPersistence(EffectsState::read,
                                                                                                      EffectsState::update,
                                                                                                      this,
                                                                                                      fs,
                                                                                                      "/config/effectsState.json"),
                                                                                            _fixtureService(fixtureService)
{

    // configure settings service update handler to update state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);

}

void EffectsService::begin()
{
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _fsPersistence.readFromFS();

    onConfigUpdated();
}

void EffectsService::onConfigUpdated()
{
    Serial.printf("EffectsService::onConfigUpdated\n");
}