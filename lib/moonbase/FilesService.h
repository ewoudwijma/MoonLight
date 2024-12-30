#ifndef FilesService_h
#define FilesService_h

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

class FilesState
{
public:
    bool filesOn;

    static void read(FilesState &settings, JsonObject &root);

    static StateUpdateResult update(JsonObject &root, FilesState &filesService);

};

class FilesService : public StatefulService<FilesState>
{
public:
    FilesService(PsychicHttpServer *server,
                      EventSocket *socket,
                      SecurityManager *securityManager);

    void begin();

private:
    HttpEndpoint<FilesState> _httpEndpoint;
    EventEndpoint<FilesState> _eventEndpoint;
    WebSocketServer<FilesState> _webSocketServer;

    void onConfigUpdated();
};

#endif
