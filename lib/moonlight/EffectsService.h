#ifndef EffectsService_h
#define EffectsService_h

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

#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>
#include <PsychicHttp.h>

#include "FastLED.h"
#define STARLIGHT_MAXLEDS 1024//*8

class EffectsState
{
public:
    bool filesOn;
    CRGB ledsP[STARLIGHT_MAXLEDS];

    static void read(EffectsState &settings, JsonObject &root);

    static StateUpdateResult update(JsonObject &root, EffectsState &effectsService);

};

class EffectsService : public StatefulService<EffectsState>
{
public:
    EffectsService(PsychicHttpServer *server,
                      EventSocket *socket,
                      SecurityManager *securityManager);

    void begin();
    void loop();

protected:
    EventSocket *_socket;

private:
    HttpEndpoint<EffectsState> _httpEndpoint;
    EventEndpoint<EffectsState> _eventEndpoint;
    WebSocketServer<EffectsState> _webSocketServer;

    void onConfigUpdated();
};

#endif
