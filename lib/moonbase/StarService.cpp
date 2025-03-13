/**
    @title     MoonLight
    @file      StarService.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#include <StarService.h>

#include "App/LedModFixture.h" // use fix->

#include "ESPFS.h"

void StarState::read(StarState &settings, JsonObject &root)
{
    ESP_LOGD("", "StarState::read");
    root["fixtures"] = Variable("Fixture", "fixture").getOptions();
    root["effects"] = Variable("layers", "effect").getOptions();
    root["projections"] = Variable("layers", "projection").getOptions();
}

StateUpdateResult StarState::update(JsonObject &root, StarState &state)
{
    return StateUpdateResult::UNCHANGED;
}

StarService::StarService(PsychicHttpServer *server,
                                     ESP32SvelteKit *sveltekit) : _httpEndpoint(StarState::read,
                                                                                                         StarState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/starState",
                                                                                                         sveltekit->getSecurityManager(),
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(StarState::read,
                                                                                                          StarState::update,
                                                                                                          this,
                                                                                                          sveltekit->getSocket(),
                                                                                                          "stars"),
                                                                                           _webSocketServer(StarState::read,
                                                                                                            StarState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/starState",
                                                                                                            sveltekit->getSecurityManager(),
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                             _server(server)
{

    // configure settings service update handler to update state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void StarService::begin()
{
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    onConfigUpdated();

    _server->on("/rest/starAPI", HTTP_POST, [&](PsychicRequest *request) {
        PsychicJsonResponse response = PsychicJsonResponse(request, false);
        JsonObject root = response.getRoot();

        JsonDocument doc;
        deserializeJson(doc, request->query());

        ESP_LOGD("", "%s %s %s %s", request->body().c_str(), request->query().c_str(), request->uri().c_str(), request->url().c_str());

        if (doc["map"]) { //send by monitor.svelte
            //
            runInLoopTask.push_back([&] {
                fix->mappingStatus = 1; //remap
            });
            root["ok"] = true;
        }
        // else if (doc["effects"]) {
        //     root["effects"] = Variable("layers", "effect").getOptions();
        // }
        // else if (doc["projections"]) {
        //     root["projections"] = Variable("layers", "projection").getOptions();
        // }

        return response.send();
    });

    ESP_LOGI("", "Registered POST endpoint: %s", "/rest/starAPI");
}

void StarService::onConfigUpdated()
{
    ESP_LOGI("", "onConfigUpdated");
}