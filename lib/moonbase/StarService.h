/**
    @title     MoonLight
    @file      StarService.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef StarService_h
#define StarService_h

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
