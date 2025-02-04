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

#include <StarService.h>

#include "Sys/SysModModel.h"

#include "ESPFS.h"

void StarState::read(StarState &settings, JsonObject &root)
{
    root["fixtures"] = Variable("Fixture", "fixture").getOptions();
    root["effects"] = Variable("layers", "effect").getOptions();
    root["projections"] = Variable("layers", "projection").getOptions();
}

StateUpdateResult StarState::update(JsonObject &root, StarState &state)
{
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

    _server->on("/rest/fixdef", HTTP_GET, [&](PsychicRequest *request) {
        PsychicJsonResponse response = PsychicJsonResponse(request, false);
        JsonObject root = response.getRoot();

        //get filenames
        JsonDocument doc;
        doc["fixtures"] = Variable("Fixture", "fixture").getOptions();
        JsonArray fixArray = doc["fixtures"].as<JsonArray>();

        //get file name from request file index
        StarString path;
        path = "/";
        int index = request->getParam("f")->value().toInt();
        ppf("fixdef index %d\n", index);
        if (index >= 0 && index < fixArray.size()) {
            path += fixArray[index];

            File file = ESPFS.open(path.getString(), "r");
            root["contents"] = file.readString();
            file.close();

            ppf("fixdef url %s %s\n", request->url().c_str(), request->uri().c_str());
        }

        // createJSON(root);
        return response.send(); 
    });

    ESP_LOGV("FeaturesService", "Registered GET endpoint: %s", FEATURES_SERVICE_PATH);


}

void StarService::onConfigUpdated()
{
    Serial.printf("StarService::onConfigUpdated\n");
}