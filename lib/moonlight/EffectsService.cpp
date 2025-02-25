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

#include "Sys/SysModModel.h" // use Variable

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

    ESP_LOGD("", "EffectsState::update");

    if (state.effect != root["effect"]) {
        state.effect = root["effect"]; changed = true;

        ESP_LOGD("", "Effects.effect.update %d", state.effect);

        // if (!sys->safeMode && false) {
            Variable("layers", "effect").setValue(state.effect, 0);
            Variable("layers", "effect").setValue(state.effect, 0); //twice to init var["value"]correctly - workaround !!!
        // }
    }

    if (state.projection != root["projection"]) {
        state.projection = root["projection"]; changed = true;

        ESP_LOGD("", "Effects.projection.update %d", state.projection);

        Variable("layers", "projection").setValue(state.projection, 0);
        Variable("layers", "projection").setValue(state.projection, 0); //twice to init var["value"]correctly - workaround !!!
    }

    if (changed)
        ESP_LOGI("", "EffectsState::update");

    return changed?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
}

EffectsService::EffectsService(PsychicHttpServer *server,
                                     ESP32SvelteKit *sveltekit, FixtureService *fixtureService) : _httpEndpoint(EffectsState::read,
                                                                                                         EffectsState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/effectsState",
                                                                                                         sveltekit->getSecurityManager(),
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(EffectsState::read,
                                                                                                          EffectsState::update,
                                                                                                          this,
                                                                                                          sveltekit->getSocket(),
                                                                                                          "effects"),
                                                                                           _webSocketServer(EffectsState::read,
                                                                                                            EffectsState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/effectsState",
                                                                                                            sveltekit->getSecurityManager(),
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(sveltekit->getSocket()),
                                                                                             _fsPersistence(EffectsState::read,
                                                                                                      EffectsState::update,
                                                                                                      this,
                                                                                                      sveltekit->getFS(),
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
    ESP_LOGI("", "EffectsService::onConfigUpdated");
}