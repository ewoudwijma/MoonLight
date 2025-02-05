#ifndef StarService_h
#define StarService_h

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

#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>
#include <PsychicHttp.h>
#include <ESP32SvelteKit.h>

class StarState
{
public:
    bool filesOn = false;

    static void read(StarState &settings, JsonObject &root);

    static StateUpdateResult update(JsonObject &root, StarState &filesState);

};

class StarService : public StatefulService<StarState>
{
public:
    StarService(PsychicHttpServer *server,
                      ESP32SvelteKit *sveltekit);

    void begin();

protected:
    EventSocket *_socket;

private:
    HttpEndpoint<StarState> _httpEndpoint;
    EventEndpoint<StarState> _eventEndpoint;
    WebSocketServer<StarState> _webSocketServer;
    PsychicHttpServer *_server;

    void onConfigUpdated();
};

#endif
