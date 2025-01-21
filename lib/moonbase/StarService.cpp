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

#include <StarService.h>

#include "Sys/SysModModel.h"

void StarState::read(StarState &settings, JsonObject &root)
{
    root["effects"] = Variable("layers", "effect").getOptions();
    root["projections"] = Variable("layers", "projection").getOptions();
}

StateUpdateResult StarState::update(JsonObject &root, StarState &state)
{
    //this must be changed to make it files specific
    boolean newState = root["xxx"];
    if (state.filesOn != newState)
    {
        state.filesOn = newState;
        return StateUpdateResult::CHANGED;
    }
    return StateUpdateResult::UNCHANGED;
}

StarService::StarService(PsychicHttpServer *server,
                                     EventSocket *socket,
                                     SecurityManager *securityManager) : _httpEndpoint(StarState::read,
                                                                                                         StarState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/starState",
                                                                                                         securityManager,
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(StarState::read,
                                                                                                          StarState::update,
                                                                                                          this,
                                                                                                          socket,
                                                                                                          "stars"),
                                                                                           _webSocketServer(StarState::read,
                                                                                                            StarState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/starState",
                                                                                                            securityManager,
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

}

void StarService::onConfigUpdated()
{
    Serial.printf("StarService::onConfigUpdated\n");
}