/**
    @title     MoonLight
    @file      EffectsService.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
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

        runInLoopTask.push_back([&] {
            // if (!sys->safeMode && false) {
                ESP_LOGD("", "Effects.effect.update %d call set effect", state.effect);
                Variable("layers", "effect")[0] = state.effect;
            // }
        });
    }

    if (state.projection != root["projection"]) {
        state.projection = root["projection"]; changed = true;

        ESP_LOGD("", "Effects.projection.update %d", state.projection);

        runInLoopTask.push_back([&] {
            // if (!sys->safeMode && false) {
                ESP_LOGD("", "Effects.projection.update %d call set projection", state.projection);
                Variable("layers", "projection")[0] = state.projection;
            // }
        });

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

    Variable("layers", "effect").subscribe(onChange, [&](EventArguments) {
        ESP_LOGD("", "EffectsState::update publish id:%s [%d] e:%d", variable.id(), rowNr, eventType);
        print->printJson("  controls", variable.var["n"]);
        //show the controls
        //send the controls through ws
    });
    
}

void EffectsService::onConfigUpdated()
{
    ESP_LOGI("", "EffectsService::onConfigUpdated");
}